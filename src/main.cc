#include <unistd.h>
#include <cstring>
#include <iostream>
#include <exception>
#include "disambiguation/disambiguation.h"
#include "exceptions/exceptions.h"
#include "exceptions/exceptions.h"
#include "parsing/bison/location.hh"
#include "parsing/bison/driver.h"
#include "parsing/bison/parser.hh"
#include "weeder/astweeder.h"
#include "environment-builder/environmentbuilder.h"
#include "environment-builder/symboltable.h"
#include "exceptions/compilerdevelopmenterror.h"
#include "exceptions/semanticerror.h"
#include "type-linking/typelinker.h"
#include "hierarchy-checking/hierarchy-checking.h"
#include "exceptions/exceptions.h"
#include "disambiguation/forward-decl.h"
#include "disambiguation/search-unclassified.h"
#include "type-checker/typechecker.h"

#ifdef GRAPHVIZ
#include "graph/graph.h"
#endif

using namespace std;

enum return_codes {
    VALID_PROGRAM = 0,
    INVALID_PROGRAM = 42,
    COMPILER_DEVELOPMENT_ERROR = 1
};

struct cmd_error {};

int main(int argc, char *argv[]) {
    vector<string> infiles;
    bool trace_parsing = false,
        trace_scanning = false,
        output_rc = false;

    try {
        // Handle optional arguments (eg. enable parse debugging)
        int opt;
        while ((opt = getopt(argc, argv, "psr")) != -1) {
            switch (opt) {
                case 'r':
                    output_rc = true;
                    break;
                case 'p':
                    trace_parsing = true;
                    break;
                case 's':
                    trace_scanning = true;
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
            << " [ -p -s ]"
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
    try {
        for (auto &ast : asts) {
            EnvironmentBuilder(default_package).visit(ast);
        }
    } catch (const EnvBuilderError &e) {
        cerr << "EnvBuilderError Exception occured: " << e.message << "\n";
        return INVALID_PROGRAM;
    } catch (const CompilerDevelopmentError &e) {
        cerr << "CompilerDevelopmentError Exception occured: " << e.message << "\n";
        return COMPILER_DEVELOPMENT_ERROR;
    } catch (...) {
        cerr << "Unknown Exception occured\n";
        return COMPILER_DEVELOPMENT_ERROR;
    }

    // Type linking
    try {
        for (auto& ast : asts) {
            CompilationUnit &current_ast = std::get<CompilationUnit>(ast);
            TypeLinker(default_package).visit(ast);
        } 
    } catch (const TypeLinkerError &e) {
        cerr << "TypeLinkerError Exception occured: " << e.message << "\n";
        return INVALID_PROGRAM;
    } catch (const CompilerDevelopmentError &e) {
        cerr << "Development error occured: " << e.message << "\n";
        return COMPILER_DEVELOPMENT_ERROR; 
    } catch (const std::exception &e) {
        cerr << e.what() << "\n";
        return COMPILER_DEVELOPMENT_ERROR;
    }

    try {
        // Hierarchy checking
        for ( auto& ast : asts ) {
            HierarchyCheckingVisitor(default_package).visit(ast);
        }
    } catch (const std::exception &e) {
        cerr << e.what() << "\n";
        return INVALID_PROGRAM;
    } catch (...) {
        cerr << "Unknown hierarchy checking error occurred\n";
        return COMPILER_DEVELOPMENT_ERROR;
    }

    try {
        // Disambiguation of names
        for (auto &ast: asts ) {
            DisambiguationVisitor(default_package).visit(ast);
        }
    } catch (DisambiguationError &e) {
        cerr << "Disambiguation error occurred: " << e.what() << "\n";
        return INVALID_PROGRAM;
    } catch (std::exception &e) {
        cerr << "Unknown disambiguation error occurred:" << e.what() << endl;
        return COMPILER_DEVELOPMENT_ERROR;
    }

    try {
    // Disambiguation of names
    for (auto &ast: asts ) {
        ForwardDeclarationVisitor(default_package).visit(ast);
    }
    } catch (DisambiguationError &e) {
        cerr << "Disambiguation error occurred: " << e.what() << "\n";
        return INVALID_PROGRAM;
    } catch (std::exception &e) {
        cerr << "Unknown disambiguation error occurred:" << e.what() << endl;
        return COMPILER_DEVELOPMENT_ERROR;
    }

    // Error on unclassified identifiers
    try {
        for (auto &ast: asts ) {
            SearchUnclassifiedVisitor().visit(ast);
        }
    } catch (std::exception &e) {
        cerr << e.what() << endl;
        return COMPILER_DEVELOPMENT_ERROR;
    }

    try {
        // Type checking
        for (auto &ast: asts ) {
            TypeChecker(default_package).visit(ast);
        }
    } catch (std::exception &e) {
        cerr << e.what() << endl;
        return INVALID_PROGRAM;
    }

    if ( output_rc ) { cerr << "RETURN CODE " << rc << endl; }

    return rc;
}
