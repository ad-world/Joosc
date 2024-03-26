import os, subprocess, sys
from helpers.helpers import *

# Load variables from .env file
load_env_file()

def valid_invalid_prog_test():
    """
    Runs joosc on all programs in valid and invalid directories.
    Tests that joosc correctly validates valid and invalidates invalid programs.
    """
    pre_a5_args = ["-a"]

    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")
    stdlib_dir = os.path.join(integration_dir, "../../stdlib")

    stdlib_files = get_all_files(stdlib_dir)

    joosc_executable = resolve_path(programs_dir, "../../joosc")

    integration_log_file = "integration.log"

    # Optional argument: -f indicates we should only print failed cases
    print_pass_cases = True
    if len(sys.argv) > 1 and sys.argv[1] == "-f":
        print_pass_cases = False
        failures = False

    # get current assignments from env
    currently_testing = os.getenv('TESTING_COVERAGE', '')
    split_assignments = currently_testing.split(',')

    test_numbers = {}

    for assignment in sorted(os.listdir(programs_dir)):
        if assignment in split_assignments:
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
                            # if program is a directory, get all files from the direcory and add to a list
                            files = [program_path]
                            if os.path.isdir(program_path):
                                files = get_all_files(program_path)

                            with open(integration_log_file, "w") as outfile:
                                result = subprocess.run([joosc_executable, *pre_a5_args, *files, *stdlib_files], stderr=outfile)

                            if result.returncode == expected_code:
                                # Test passed, display output if -f is not set
                                if print_pass_cases:
                                    print(f"{colors.OKGREEN}SUCCESS: Running joosc on {program} successfully returned {expected_code}{colors.ENDC}")
                                    print_file_contents(integration_log_file)
                                pass_count += 1
                            else:
                                # Test failed, display output always
                                print(f"{colors.FAIL}FAIL: Running joosc on {program} returned {result.returncode}. Expected: {expected_code}{colors.ENDC}")
                                print_file_contents(integration_log_file)
                                failures = True
                                fail_count += 1
    
            test_numbers[assignment] = { 'pass_count': pass_count, 'fail_count': fail_count, 'total_count': pass_count + fail_count }

    if failures:
        print(f"{colors.FAIL}\nERROR: Tests had failures")
        for assignment, results in test_numbers.items():
            pass_count, fail_count, total_count = results['pass_count'], results['fail_count'], results['total_count']
            print(f"{colors.HEADER}\nTest suite for assignment {assignment}")
            print(f"{colors.OKGREEN}Passing tests: {pass_count}/{total_count}")
            print(f"{colors.FAIL}Failing tests: {fail_count}/{total_count}")
            print(f"{colors.ENDC}")
        return 1
    else:
        print(f"{colors.OKGREEN}All tests succeeded!{colors.ENDC}")
        return 0

if __name__ == "__main__":
    sys.exit(valid_invalid_prog_test())
