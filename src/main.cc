#include <unistd.h>
#include <cstring>
#include <iostream>
#include "parsing/bison/location.hh"
#include "parsing/bison/driver.h"
#include "parsing/bison/parser.hh"
#include "weeder/weeder.h"
using namespace std;

enum return_codes {
    PARSING_SUCCESS = 0,
    PARSING_FAILURE = 42,
};

struct cmd_error {};

int main(int argc, char *argv[]) {
    string infile;
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

        switch ( argc - optind ) {
            case 1:
                infile = argv[optind];
                break;
            default:
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
    Weeder weeder;

    try {
        drv.trace_scanning = trace_scanning;
        drv.trace_parsing = trace_parsing;
        rc = drv.parse(infile);
    } catch ( ... ) {
        cerr << "Exception occured" << endl;
        return PARSING_FAILURE;
    }

    int parsing_rc = ((rc == 0) ? PARSING_SUCCESS : PARSING_FAILURE);

    // TODO: pass this function the root of the parse tree / AST
    // parsing_rc = weeder.weed(drv.root, infile);

    if ( output_rc ) { cerr << "RETURN CODE " << parsing_rc << endl; }
    return parsing_rc;
}
