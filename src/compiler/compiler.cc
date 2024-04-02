#include "compiler.h"
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

int Compiler::finishWith(ReturnCode code) {
    if ( output_rc ) { cerr << "RETURN CODE " << code << endl; }
    return code;
}

int Compiler::run() {
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

        std::list<std::string> files = infiles;

        for ( auto it = strfiles.rbegin(); it != strfiles.rend(); it++ ) {
            std::string &strfile = *it;

            // Add in reverse to both list's front
            drv.strfiles.push_front(strfile);
            files.push_front("Foo.java");   // dummy filename for strfile
        }

        for (auto file : files) {
            bool is_strfile = !drv.strfiles.empty();
            std::string strfile = is_strfile ? drv.strfiles.front() : "";
            int rc = drv.parse(file);

            if (rc != 0) {
                cerr << "Parsing failed" << endl;
                return finishWith(ReturnCode::INVALID_PROGRAM);
            }

            AstNodeVariant ast = std::move(*drv.root);            

            rc = weeder.weed(ast, file, strfile);

            if (rc != 0) {
                cerr << "Weeding failed" << endl;
                return finishWith(ReturnCode::INVALID_PROGRAM);
            }

            asts.emplace_back(std::move(ast));
        }

    } catch ( ... ) {
        cerr << "Unknown exception occured in syntactic analysis" << endl;
        return finishWith(ReturnCode::INVALID_PROGRAM);
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
        // TODO: cleanup memleaks
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
        return finishWith(ReturnCode::COMPILER_DEVELOPMENT_ERROR);
    } catch (const SimulatorError &e ) {
        cerr << e.what() << "\n";
        return finishWith(ReturnCode::COMPILER_DEVELOPMENT_ERROR);
    } catch (const std::exception &e) {
        cerr << e.what() << "\n";
        return finishWith(ReturnCode::INVALID_PROGRAM);
    } catch (...) {
        cerr << "Unknown error occurred\n";
        return finishWith(ReturnCode::COMPILER_DEVELOPMENT_ERROR);
    }

    return finishWith(ReturnCode::VALID_PROGRAM);
}
