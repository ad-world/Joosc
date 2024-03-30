import os, subprocess, sys
from helpers.helpers import *

def single_valid_invalid_prog_test(program_path, print_pass_cases=True, expected_code=None) -> bool:
    """
    Run single valid invalid test on path. Returns true if test passed, otherwise returns false.
    """
    if expected_code is None:
        # Determine expected code to the best of our ability
        if "invalid/" in program_path:
            expected_code = 42
        elif "warning/" in program_path:
            expected_code = 43
        else:
            expected_code = 0

    compiler_args = ["-a"]
    program = os.path.basename(program_path)
    integration_log_file = "integration.log"

    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")
    stdlib_dir = os.path.join(integration_dir, "../../stdlib")
    stdlib_files = get_all_files(stdlib_dir, ".java")

    joosc_executable = resolve_path(programs_dir, "../../joosc")

    files = [program_path]
    if os.path.isdir(program_path):
        files = get_all_files(program_path, ".java")

    with open(integration_log_file, "w") as outfile:
        result = subprocess.run([joosc_executable, *compiler_args, *files, *stdlib_files], stderr=outfile)

    if result.returncode == expected_code:
        # Test passed, display output if -f is not set
        if print_pass_cases:
            print(f"{colors.OKGREEN}SUCCESS: Running joosc on {program} successfully returned {expected_code}{colors.ENDC}")
            print_file_contents(integration_log_file)
        return True
    else:
        # Test failed, display output always
        print(f"{colors.FAIL}FAIL: Running joosc on {program} returned {result.returncode}. Expected: {expected_code}{colors.ENDC}")
        print_file_contents(integration_log_file)
        return False

def valid_invalid_prog_test():
    """
    Runs joosc on all programs in valid and invalid directories.
    Tests that joosc correctly validates valid and invalidates invalid programs.
    """
    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")

    # Optional argument: -f indicates we should only print failed cases
    print_pass_cases = True
    if "-f" in sys.argv:
        print_pass_cases = False

    # Get configured testable assignments from environment
    assignments_to_test = os.getenv('VALID_INVALID_TEST', '').split(',')
    test_results = {}
    failures = False

    for assignment in sorted(os.listdir(programs_dir)):
        if assignment in assignments_to_test:
            pass_count = 0
            fail_count = 0

            assignment_path = os.path.join(programs_dir, assignment)

            for source in ("local", "marmoset"):
                for test_type, expected_code in (("valid", 0), ("invalid", 42), ("warning", 43)):

                    directory = os.path.join(assignment_path, f"{source}/{test_type}")
                    if not os.path.exists(directory): continue

                    print(f"\n{colors.HEADER_BOLD_UNDERLINE}Testing all programs in {resolve_path(directory, '')}:{colors.ENDC}")

                    for program in os.listdir(directory):
                        program_path = resolve_path(directory, program)

                        if os.path.exists(program_path):
                            passed = single_valid_invalid_prog_test(program_path, print_pass_cases, expected_code)
                            if passed:
                                pass_count += 1
                            else:
                                fail_count += 1
                                failures = True
    
            test_results[assignment] = { 'pass_count': pass_count, 'fail_count': fail_count, 'total_count': pass_count + fail_count }

    return print_test_results(failures, test_results)

if __name__ == "__main__":
    # Load variables from .env file
    load_env_file()

    if "-s" in sys.argv:
        if len(sys.argv) < 3:
            print("Must pass test file / test directory.")
            sys.exit(1)
        sys.exit(int(not single_valid_invalid_prog_test(sys.argv[2])))
    else:
        sys.exit(valid_invalid_prog_test())
