#include <unistd.h>
#include <cstring>
#include <iostream>
#include "parsing/bison/location.hh"
#include "parsing/bison/driver.h"
#include "parsing/bison/parser.hh"
#include "weeder/astweeder.h"
#include "environment-builder/environmentbuilder.h"
#include "environment-builder/symboltable.h"
#include "exceptions/compilerdevelopmenterror.h"
#include "exceptions/semanticerror.h"
#include "type-linking/typelinker.h"

using namespace std;

enum return_codes {
    VALID_PROGRAM = 0,
    INVALID_PROGRAM = 42,
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
    vector<AstNodeVariant*> asts;

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

            // AstNodeVariant& var = *drv.root;

            // if (std::holds_alternative<CompilationUnit>(var)) {
            //     std::cout << "Holds the alternative\n";
            // }

            rc = weeder.weed(*drv.root, infile);

            if(rc != 0) {
                cerr << "Parsing failed" << endl;
                if ( output_rc ) { cerr << "RETURN CODE " << rc << endl; }

                return INVALID_PROGRAM;
            }

            // asts.push_back(drv.root);
            break;
        }

    } catch ( ... ) {
        cerr << "Exception occured" << endl;
        return INVALID_PROGRAM;
    }

    // Environment building
    // PackageDeclarationObject default_package;
    // try {
    //     for (auto &ast : asts) {
    //         EnvironmentBuilder(default_package).visit(*ast);
    //         break;
    //     }
    // } catch (const SemanticError &e) {
    //     cerr << "SemanticError Exception occured: " << e.message << "\n";
    //     return INVALID_PROGRAM;
    // } catch (const CompilerDevelopmentError &e) {
    //     cerr << "CompilerDevelopmentError Exception occured: " << e.message << "\n";
    // } catch (...) {
    //     cerr << "Unknown Exception occured\n";
    // }


    // // Type linking
    // try {
    //     for (auto ast : asts) {
    //         CompilationUnit &current_ast = get<CompilationUnit>(*ast);
    //         TypeLinker(default_package, current_ast, asts).visit(*ast);
    //     } 
    // } catch (const SemanticError &e) {
    //     cerr << "SemanticError Exception occured: " << e.message << "\n";
    // } catch (...) {
    //     cerr << "Unknown Exception occured\n";
    // }


    if ( output_rc ) { cerr << "RETURN CODE " << rc << endl; }

    return rc;
}
