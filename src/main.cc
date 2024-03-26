#include <unistd.h>
#include <iostream>
#include <exception>
#include "disambiguation/disambiguation.h"
#include "exceptions/exceptions.h"
#include "exceptions/exceptions.h"
#include "parsing/bison/driver.h"
#include "parsing/bison/parser.hh"
#include "weeder/astweeder.h"
#include "environment-builder/environmentbuilder.h"
#include "type-linking/typelinker.h"
#include "hierarchy-checking/hierarchy-checking.h"
#include "exceptions/exceptions.h"
#include "disambiguation/forward-decl.h"
#include "disambiguation/search-unclassified.h"
#include "type-checker/typechecker.h"
#include "cfg-builder/cfg-builder.h"
#include "reachability/reachability.h"
#include "reachability/reached-statement.h"
#include "utillities/util.h"
#include "local-variables/local-variable-visitor.h"
#include "IR/ir.h"
#include "IR-builder/ast-to-ir.h"
#include "IR-interpreter/simulation/simulation.h"

#ifdef GRAPHVIZ
#include "graph/graph.h"
#include "graph/ir_graph.h"
#endif

using namespace std;

enum return_codes {
    VALID_PROGRAM = 0,
    INVALID_PROGRAM = 42,
    WARN_PROGRAM = 43,
    COMPILER_DEVELOPMENT_ERROR = 1
};

enum class CommandLineArg {
    OUTPUT_RETURN = 'r',
    TRACE_PARSING = 'p',
    TRACE_SCANNING = 's',
    STATIC_ANALYSIS_ONLY = 'a', // Don't emit IR/assembly; used for pre-A5 tests
};

struct cmd_error {};

int main(int argc, char *argv[]) {
    vector<string> infiles;

    bool trace_parsing = false;
    bool trace_scanning = false;
    bool output_rc = false;
    bool emit_code = true;

    try {
        // Handle optional arguments (eg. enable parse debugging)
        char opt;
        while ((opt = getopt(argc, argv, "psra")) != -1) {
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
                default:
                    throw cmd_error();
            }
        }

        // For each non-option argument, add to infiles
        for (int i = optind; i < argc; i++) {
            // Check that the file exists
            if (access(argv[i], F_OK) == -1) {
                cerr << "File " << argv[i] << " does not exist" << endl;
                return INVALID_PROGRAM;
            }

            infiles.push_back(argv[i]);
        }

        if (infiles.size() == 0) {
            throw cmd_error();
        }
    } catch ( cmd_error & e ) {
        cerr << "Usage:\n\t"
            << argv[0]
            << " <filename>"
            << " [ -p -s -r -a ]"
            << endl;
        return EXIT_FAILURE;
    } catch ( ... ) {
        cerr << "Error occured on input" << endl;
        return EXIT_FAILURE;
    }

    int rc = 0;
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
            rc = drv.parse(infile);

            if(rc != 0) {
                cerr << "Parsing failed" << endl;
                if ( output_rc ) { cerr << "RETURN CODE " << rc << endl; }

                return INVALID_PROGRAM;
            }

            AstNodeVariant ast = std::move(*drv.root);            

            rc = weeder.weed(ast, infile);

            if(rc != 0) {
                cerr << "Parsing failed" << endl;
                if ( output_rc ) { cerr << "RETURN CODE " << rc << endl; }

                return INVALID_PROGRAM;
            }

            asts.emplace_back(std::move(ast));
        }

    } catch ( ... ) {
        cerr << "Exception occured" << endl;
        return INVALID_PROGRAM;
    }

    // Environment building
    PackageDeclarationObject default_package;
    Util::root_package = &default_package;

    try {
        // Environment building
        for (auto &ast : asts) {
            EnvironmentBuilder(default_package).visit(ast);
        }

        // Type linking
        for (auto& ast : asts) {
            CompilationUnit &current_ast = std::get<CompilationUnit>(ast);
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

        if (emit_code) {
            // Convert to IR
            auto &main_ast = asts.front();
            IR main_ir = IRBuilderVisitor().visit(main_ast);
        }

        // Interpret the IR
        Simulator sim(&main_ir);


#ifdef GRAPHVIZ
        // Graph IR
        IRGraphVisitor().visit(main_ir);
#endif
    } catch (const CompilerError &e ) {
        cerr << e.what() << "\n";
        return COMPILER_DEVELOPMENT_ERROR;
    } catch (const std::exception &e) {
        cerr << e.what() << "\n";
        return INVALID_PROGRAM;
    } catch (...) {
        cerr << "Unknown error occurred\n";
        return COMPILER_DEVELOPMENT_ERROR;
    }

    if ( output_rc ) { cerr << "RETURN CODE " << rc << endl; }

    return rc;
}
