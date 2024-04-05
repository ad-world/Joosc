#include <unistd.h>
#include <iostream>
#include <exception>
#include <sstream>
#include "compiler/compiler.h"

using namespace std;

enum class CommandLineArg {
    OUTPUT_RETURN = 'r',
    TRACE_PARSING = 'p',
    TRACE_SCANNING = 's',
    STATIC_ANALYSIS_ONLY = 'a', // Don't emit IR/assembly; used for pre-A5 tests
    RUN_AND_TEST_IR = 'i',
    RUN_AND_TEST_JAVA_IR = 'j'
};

struct cmd_error {};

int main(int argc, char *argv[]) {
    Compiler compiler;

    try {
        // Handle optional arguments (eg. enable parse debugging)
        char opt;
        while ((opt = getopt(argc, argv, "psraij")) != -1) {
            switch (opt) {
                case static_cast<char>(CommandLineArg::OUTPUT_RETURN):
                    compiler.setOutputRC(true);
                    break;
                case static_cast<char>(CommandLineArg::TRACE_PARSING):
                    compiler.setTraceParsing(true);
                    break;
                case static_cast<char>(CommandLineArg::TRACE_SCANNING):
                    compiler.setTraceScanning(true);
                    break;
                case static_cast<char>(CommandLineArg::STATIC_ANALYSIS_ONLY):
                    compiler.setEmitCode(false);
                    break;
                case static_cast<char>(CommandLineArg::RUN_AND_TEST_IR):
                    compiler.setRunIR(true);
                    break;
                case static_cast<char>(CommandLineArg::RUN_AND_TEST_JAVA_IR):
                    compiler.setRunJavaIR(true);
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
                return compiler.finishWith(Compiler::USAGE_ERROR);
            }

            compiler.addInFile(argv[i]);
        }

        if (compiler.inFilesEmpty()) {
            throw cmd_error();
        }
    } catch ( cmd_error & e ) {
        cerr 
            << "Usage:\n\t"
            << argv[0]
            << " <filename>"
            << " [ -p -s -r -a -i ]"
            << "\n";
        return compiler.finishWith(Compiler::USAGE_ERROR);
    } catch ( ... ) {
        cerr << "Error occured on input\n";
        return compiler.finishWith(Compiler::USAGE_ERROR);
    }

    return compiler.run();
}
