#pragma once

#include <list>
#include <string>

class Compiler {
public:
    enum ReturnCode {
        VALID_PROGRAM = 0,
        INVALID_PROGRAM = 42,
        WARN_PROGRAM = 43,
        COMPILER_DEVELOPMENT_ERROR = 1,
        USAGE_ERROR = 2,
    };
private:

    bool trace_parsing = false;
    bool trace_scanning = false;
    bool output_rc = false;
    bool emit_code = true;
    bool run_ir = false;
    bool run_java_ir = false;
    std::list<std::string> strfiles; // Strings inputted as files
    std::list<std::string> infiles; // File input

    // Return the label of the entry point method
    std::string getEntryPointMethod();
public:
    Compiler() {};
    void setTraceParsing(bool value) { trace_parsing = value; }
    void setTraceScanning(bool value) { trace_scanning = value; }
    void setOutputRC(bool value) { output_rc = value; }
    void setEmitCode(bool value) { emit_code = value; }
    void setRunIR(bool value) { run_ir = value; }
    void setRunJavaIR(bool value) { run_java_ir = value; }

    // File names
    void addInFile(std::string filename) { infiles.push_back(filename); }
    void setInFiles(std::list<std::string> files) { infiles = files; }
    bool inFilesEmpty() { return infiles.empty(); }

    // String files
    void addStringFile(std::string strfile) { strfiles.push_back(strfile); }
    void setStringFiles(std::list<std::string> files) { strfiles = files; }

    int finishWith(ReturnCode code);
    int run();
};
