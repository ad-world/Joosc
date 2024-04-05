import os, subprocess, sys
from helpers.helpers import *

def interpret_java(program_path):
    root_dir = resolve_path(os.path.dirname(__file__), "../../../")
    integration_dir = os.path.dirname(__file__)
    java_ir_dir = resolve_path(root_dir, "java-ir")
    java_ir_bin_dir = resolve_path(root_dir, "java-ir-bin")
    stdlib_dir = os.path.join(integration_dir, "../../stdlib")
    stdlib_files = get_all_files(stdlib_dir, ".java")
    java_ir_files = get_all_files(java_ir_dir, ".java")

    joosc_executable = resolve_path(root_dir, "./joosc")

    files = get_all_files(program_path, ".java") if os.path.isdir(program_path) else [program_path]

    result = subprocess.run([joosc_executable, "-j", *files, *stdlib_files])

    if result.returncode == 0:
        print(f"{colors.OKGREEN}Program successfully outputted as Java, now running Java contents\n{colors.ENDC}")
 

        result = subprocess.run(["javac", "-d", java_ir_bin_dir, *java_ir_files])
        result = subprocess.run(["java", "-cp", java_ir_bin_dir, "joosc.ir.interpret.Main"])
    else:
        print(f"{colors.FAIL} program could not be outputted as Java. FAIL{colors.ENDC}\n")

if __name__ == "__main__":
    load_env_file()

    if len(sys.argv) < 2:
        print("Usage: python3 interpret_java.py [filepath]")
        sys.exit(1)

    sys.exit(interpret_java(sys.argv[1]))