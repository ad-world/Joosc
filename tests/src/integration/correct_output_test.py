import os, subprocess, sys
from helpers.helpers import *
from assemble import assemble_all_files

def correct_output_test():
    """
    Runs joosc on all programs in valid and invalid directories.
    Tests that compiled programs produce the correct output by compiling and running them.
    """
    compiler_args = ["-i"]

    root_dir = resolve_path(os.path.dirname(__file__), "../../../")
    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")

    stdlib_dir = os.path.join(integration_dir, "../../stdlib")
    stdlib_files = get_all_files(stdlib_dir, ".java")

    joosc_executable = resolve_path(programs_dir, "../../joosc")

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
                            # if program is a directory, get all files from the direcory and add to a list
                            files = get_all_files(program_path, ".java") if os.path.isdir(program_path) else [program_path]

                            result = subprocess.run([joosc_executable, *compiler_args, *files, *stdlib_files])

                            if result.returncode in (0, 43):
                                # Program compiled, check output is correct
                                assemble_all_files()
                                program_result = subprocess.run(["./main"], cwd=root_dir)

                                with open(resolve_path(root_dir, "ir_result.tmp"), "w+") as file:
                                    ir_result = file.read()
                                    ir_result = ir_result if ir_result else "NOTHING"
                                    print(f"{colors.OKCYAN}program {program} returned {ir_result} after interpreting IR{colors.ENDC}")
                                
                                with open(resolve_path(root_dir, "ir_result_canon.tmp"), "w+") as file:
                                    ir_canon_result = file.read()
                                    ir_canon_result = ir_canon_result if ir_canon_result else "NOTHING"
                                    print(f"{colors.OKCYAN}program {program} returned {ir_canon_result} after interpreting Canonical IR{colors.ENDC}")

                                print(f"{colors.OKCYAN}program {program} returned {program_result.returncode} after compiling and executing{colors.ENDC}")

                                if not (str(program_result.returncode) == ir_canon_result and ir_canon_result == ir_result):
                                    print(f"{colors.FAIL}FAIL: IR, Canonical IR, and binary results are not the same!{colors.ENDC}\n")
                                    failures = True
                                    fail_count += 1
                                else:
                                    print("")
                                    pass_count += 1
                                
                            else:
                                # Test failed due to compile failing
                                print(f"{colors.FAIL}FAIL: joosc failed to compile {program}, so it couldn't be ran.{colors.ENDC}\n")
                                failures = True
                                fail_count += 1
    
            test_results[assignment] = { 'pass_count': pass_count, 'fail_count': fail_count, 'total_count': pass_count + fail_count }

    return print_test_results(failures, test_results)

if __name__ == "__main__":
    # Load variables from .env file
    load_env_file()

    sys.exit(correct_output_test())
