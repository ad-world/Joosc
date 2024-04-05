import shutil
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
        print(f"{colors.OKGREEN}Running regular IR tree with Java simulation.{colors.ENDC}")

        java_files_no_canonical = [s for s in java_ir_files if not s.endswith("MainCanonical.java")]

        result = subprocess.run(["javac", "-d", java_ir_bin_dir, *java_files_no_canonical])
        if (result.returncode == 0):
            result = subprocess.run(["java", "-cp", java_ir_bin_dir, "joosc.ir.interpret.MainNonCanonical"])     
            print(f"{colors.OKCYAN}Interpreting regular IR tree resulted in: {result.returncode}. {colors.ENDC}");  
        else:
            print(f"{colors.FAIL}Could not compile regular IR tree for Java simulation.{colors.ENDC}")

        print(f"{colors.OKGREEN}Running canonical IR tree with Java simulation.{colors.ENDC}")

        java_files_no_main = [s for s in java_ir_files if not s.endswith("MainNonCanonical.java")]
        shutil.rmtree(java_ir_bin_dir)

        result = subprocess.run(["javac", "-d", java_ir_bin_dir, *java_files_no_main])

        if (result.returncode == 0):
            result = subprocess.run(["java", "-cp", java_ir_bin_dir, "joosc.ir.interpret.MainCanonical"])  
            print(f"{colors.OKCYAN}Interpreting canonical IR tree resulted in: {result.returncode}. {colors.ENDC}");
        else:
            print(f"{colors.FAIL}Could not compile canonical IR tree for Java simulation.{colors.ENDC}")
            

    else:
        print(f"{colors.FAIL} program could not be outputted as Java. FAIL{colors.ENDC}\n")

if __name__ == "__main__":
    load_env_file()

    if len(sys.argv) < 2:
        print("Usage: python3 interpret_java.py [filepath]")
        sys.exit(1)

    sys.exit(interpret_java(sys.argv[1]))