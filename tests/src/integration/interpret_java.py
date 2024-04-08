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

    passed = True

    if result.returncode == 0:
        print(f"{colors.OKGREEN}Running regular IR tree with Java simulation.{colors.ENDC}")

        java_files_no_canonical = [s for s in java_ir_files if not s.endswith("MainCanonical.java")]

        result = subprocess.run(["javac", "-d", java_ir_bin_dir, *java_files_no_canonical])
        if (result.returncode == 0):
            result = subprocess.run(["java", "-cp", java_ir_bin_dir, "joosc.ir.interpret.MainNonCanonical"])     
            print(f"{colors.OKCYAN}Interpreting regular IR tree resulted in: {result.returncode}. {colors.ENDC}")
            passed = passed and (result.returncode == 123)
        else:
            print(f"{colors.FAIL}Could not compile regular IR tree for Java simulation.{colors.ENDC}")
            passed = False

        print(f"{colors.OKGREEN}Running canonical IR tree with Java simulation.{colors.ENDC}")

        java_files_no_main = [s for s in java_ir_files if not s.endswith("MainNonCanonical.java")]
        shutil.rmtree(java_ir_bin_dir)

        result = subprocess.run(["javac", "-d", java_ir_bin_dir, *java_files_no_main])

        if (result.returncode == 0):
            result = subprocess.run(["java", "-cp", java_ir_bin_dir, "joosc.ir.interpret.MainCanonical"])  
            print(f"{colors.OKCYAN}Interpreting canonical IR tree resulted in: {result.returncode}. {colors.ENDC}")
            passed = passed and (result.returncode == 123)
        else:
            print(f"{colors.FAIL}Could not compile canonical IR tree for Java simulation.{colors.ENDC}")
            passed = False

    else:
        print(f"{colors.FAIL}Program could not be outputted as Java, joosc failed with return code {result.returncode} FAIL{colors.ENDC}\n")
        passed = False

    return passed

def interpret_java_all_test():
    """
    Runs interpret_java test on all programs
    """
    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")

    # Get configured testable assignments from environment
    assignments_to_test = os.getenv('COMPILED_OUTPUT_TEST', '').split(',')
    test_results = {}
    failures = False

    for assignment in sorted(os.listdir(programs_dir)):
        if assignment in assignments_to_test:
            pass_count = 0
            fail_count = 0

            assignment_path = os.path.join(programs_dir, assignment)

            for source in ("local", "marmoset"):
                for test_type in ("valid", "warning"):

                    directory = os.path.join(assignment_path, f"{source}/{test_type}")
                    if not os.path.exists(directory): continue

                    print(f"\n{colors.HEADER_BOLD_UNDERLINE}Testing all programs in {resolve_path(directory, '')}:{colors.ENDC}")

                    for program in os.listdir(directory):
                        program_path = resolve_path(directory, program)

                        if os.path.exists(program_path):
                            print(f"\n{colors.HEADER_BOLD_UNDERLINE}Testing {program_path}:{colors.ENDC}")
                            passed = interpret_java(program_path)
                            if passed:
                                pass_count += 1
                            else:
                                fail_count += 1
                                failures = True
    
            test_results[assignment] = { 'pass_count': pass_count, 'fail_count': fail_count, 'total_count': pass_count + fail_count }

    return print_test_results(failures, test_results)

if __name__ == "__main__":
    load_env_file()

    if "-s" in sys.argv:
        if len(sys.argv) < 3:
            print("Must pass test file / test directory.")
            sys.exit(1)
        sys.exit(int(not interpret_java(sys.argv[2])))
    else:
        sys.exit(interpret_java_all_test())
