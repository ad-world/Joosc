#include <unistd.h>
#include <iostream>
#include <exception>
#include <ostream>

#include "exceptions/exceptions.h"
#include "utillities/util.h"

#include "parsing/bison/driver.h"
#include "parsing/bison/parser.hh"
#include "weeder/astweeder.h"

#include "environment-builder/environmentbuilder.h"
#include "type-linking/typelinker.h"
#include "hierarchy-checking/hierarchy-checking.h"
#include "disambiguation/forward-decl.h"
#include "disambiguation/search-unclassified.h"
#include "disambiguation/disambiguation.h"
#include "type-checker/typechecker.h"
#include "cfg-builder/cfg-builder.h"
#include "reachability/reachability.h"
#include "reachability/reached-statement.h"
#include "local-variables/local-variable-visitor.h"

#include "IR/ir.h"
#include "IR-builder/ast-to-ir.h"
#include "IR-interpreter/simulation/simulation.h"
#include "IR-canonicalizer/ir-canonicalizer.h"

#ifdef GRAPHVIZ
    #include "graph/graph.h"
    #include "graph/ir_graph.h"
#endif

using namespace std;

enum ReturnCode {
    VALID_PROGRAM = 0,
    INVALID_PROGRAM = 42,
    WARN_PROGRAM = 43,
    COMPILER_DEVELOPMENT_ERROR = 1,
    USAGE_ERROR = 2,
};

int finishWith(ReturnCode code, bool output_rc) {
    if ( output_rc ) { cerr << "RETURN CODE " << code << endl; }
    return code;
}

enum class CommandLineArg {
    OUTPUT_RETURN = 'r',
    TRACE_PARSING = 'p',
    TRACE_SCANNING = 's',
    STATIC_ANALYSIS_ONLY = 'a', // Don't emit IR/assembly; used for pre-A5 tests
    RUN_AND_TEST_IR = 'i'
};

struct cmd_error {};

int main(int argc, char *argv[]) {
    vector<string> infiles;

    bool trace_parsing = false;
    bool trace_scanning = false;
    bool output_rc = false;
    bool emit_code = true;
    bool run_ir = false;

    try {
        // Handle optional arguments (eg. enable parse debugging)
        char opt;
        while ((opt = getopt(argc, argv, "psrai")) != -1) {
            switch (opt) {
                case static_cast<char>(CommandLineArg::OUTPUT_RETURN):
                    output_rc = true;
                    break;
                case static_cast<char>(CommandLineArg::TRACE_PARSING):
                    trace_parsing = true;
                    break;
                case static_cast<char>(CommandLineArg::TRACE_SCANNING):
                    trace_scanning = true;
                    break;
                case static_cast<char>(CommandLineArg::STATIC_ANALYSIS_ONLY):
                    emit_code = false;
                    break;
                case static_cast<char>(CommandLineArg::RUN_AND_TEST_IR):
                    run_ir = true;
                    break;
                default:
                    throw cmd_error();
            }
        }

        // For each non-option argument, add to infiles
        for (int i = optind; i < argc; i++) {
            // Check that the file exists
            if (access(argv[i], F_OK) == -1) {
                cerr << "File " << argv[i] << " does not exist" << endl;
                return finishWith(ReturnCode::USAGE_ERROR, output_rc);
            }

            infiles.push_back(argv[i]);
        }

        if (infiles.size() == 0) {
            throw cmd_error();
        }
    } catch ( cmd_error & e ) {
        cerr 
            << "Usage:\n\t"
            << argv[0]
            << " <filename>"
            << " [ -p -s -r -a -i ]"
            << "\n";
        return finishWith(ReturnCode::USAGE_ERROR, output_rc);
    } catch ( ... ) {
        cerr << "Error occured on input\n";
        return finishWith(ReturnCode::USAGE_ERROR, output_rc);
    }

    Driver drv;
    AstWeeder weeder;
    vector<AstNodeVariant> asts;
    Util::linked_asts = &asts;
    
    #ifdef GRAPHVIZ
    GraphVisitor gv(asts); // runs on return/destruct
    #endif

    // Lexing and parsing
    try {
        drv.trace_scanning = trace_scanning;
        drv.trace_parsing = trace_parsing;

        for (auto infile : infiles) {
            int rc = drv.parse(infile);

            if (rc != 0) {
                cerr << "Parsing failed" << endl;
                return finishWith(ReturnCode::INVALID_PROGRAM, output_rc);
            }

            AstNodeVariant ast = std::move(*drv.root);            

            rc = weeder.weed(ast, infile);

            if (rc != 0) {
                cerr << "Weeding failed" << endl;
                return finishWith(ReturnCode::INVALID_PROGRAM, output_rc);
            }

            asts.emplace_back(std::move(ast));
        }

    } catch ( ... ) {
        cerr << "Unknown exception occured in syntactic analysis" << endl;
        return finishWith(ReturnCode::INVALID_PROGRAM, output_rc);
    }

    PackageDeclarationObject default_package;
    Util::root_package = &default_package;

    try {
        // Environment building
        for (auto &ast : asts) {
            EnvironmentBuilder(default_package).visit(ast);
        }

        // Type linking
        for (auto& ast : asts) {
            TypeLinker(default_package).visit(ast);
        } 

        // Hierarchy checking
        for ( auto& ast : asts ) {
            HierarchyCheckingVisitor(default_package).visit(ast);
        }

        // Disambiguation of names
        for (auto &ast: asts ) {
            DisambiguationVisitor(default_package).visit(ast);
        }

        // Disambiguation of names (forward decl)
        for (auto &ast: asts ) {
            ForwardDeclarationVisitor(default_package).visit(ast);
        }

        // Check for unclassified identifiers (optional)
        for (auto &ast: asts ) {
            SearchUnclassifiedVisitor().visit(ast);
        }

        // Type checking
        for (auto &ast: asts ) {
            TypeChecker(default_package).visit(ast);
        }

        // CfgBuilder
        for (auto &ast: asts) {
            CfgBuilderVisitor().visit(ast);
        } 

        // Reachability testing
        for (auto &ast : asts) {
            CfgReachabilityVisitor().visit(ast);
        }

        // Reachability testing
        for (auto &ast : asts) {
            StatementVisitor().visit(ast);
        }

        // Local variable checking
        for (auto &ast: asts) {
            LocalVariableVisitor().visit(ast);
        }

        // Code generation
        if (emit_code) {
            // Convert to IR
            auto &main_ast = asts.front();
            IR main_ir = IRBuilderVisitor().visit(main_ast);

            #ifdef GRAPHVIZ
                // Graph IR
                IRGraphVisitor().visit(main_ir);
            #endif

            if (run_ir) {
                // Run interpreter on IR and get value
                try {
                    std::ofstream result_file {"ir_result.tmp"};
                    int result = Simulator(&main_ir).call("test", {});
                    result_file << result;
                } catch (const SimulatorError &e ) {
                    cerr << e.what() << "\n";
                }
            }

            // Canonicalize IR
            IRCanonicalizer().convert(main_ir);

            if (run_ir) {
                // Run interpreter on Canonical IR and get value
                try {
                    std::ofstream result_file {"ir_canon_result.tmp"};
                    int result = Simulator(&main_ir).call("test", {});
                    result_file << result;
                } catch (const SimulatorError &e ) {
                    cerr << e.what() << "\n";
                }
            }

            // Emit assembly (TODO)

        }

    } catch (const CompilerError &e ) {
        cerr << e.what() << "\n";
        return finishWith(ReturnCode::COMPILER_DEVELOPMENT_ERROR, output_rc);
    } catch (const SimulatorError &e ) {
        cerr << e.what() << "\n";
        return finishWith(ReturnCode::COMPILER_DEVELOPMENT_ERROR, output_rc);
    } catch (const std::exception &e) {
        cerr << e.what() << "\n";
        return finishWith(ReturnCode::INVALID_PROGRAM, output_rc);
    } catch (...) {
        cerr << "Unknown error occurred\n";
        return finishWith(ReturnCode::COMPILER_DEVELOPMENT_ERROR, output_rc);
    }

    return finishWith(ReturnCode::VALID_PROGRAM, output_rc);
}
