import os, subprocess, sys
from pathlib import Path

# Imported from Blender
class colors:
    HEADER = '\033[95m'
    OKBLUE = '\033[94m'
    OKCYAN = '\033[96m'
    OKGREEN = '\033[92m'
    WARNING = '\033[93m'
    FAIL = '\033[91m'
    ENDC = '\033[0m'
    BOLD = '\033[1m'
    UNDERLINE = '\033[4m'

# Function to load environment variables from .env file
def load_env_file(filename=".env"):
    with open(filename, "r") as f:
        for line in f:
            # Skip comments and empty lines
            if line.strip() and not line.strip().startswith("#"):
                key, value = line.strip().split("=")
                os.environ[key] = value

# Load variables from .env file
load_env_file()


def resolve_path(path1, path2):
    return str(Path(os.path.join(path1, path2)).resolve())

def print_file_contents(file):
    with open(file, "r") as outfile: 
        print(outfile.read())

def get_all_files(directory):
    file_list = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.java'):
                file_list.append(os.path.join(root, file))
    return file_list

def valid_invalid_prog_test():
    """
    Runs joosc on all programs in valid and invalid directories.
    Tests that joosc correctly validates valid and invalidates invalid programs.
    """
    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")
    stdlib_dir = os.path.join(integration_dir, "../../stdlib")
    valid_programs = os.path.join(programs_dir, "valid")
    invalid_programs = os.path.join(programs_dir, "invalid")

    stdlib_files = get_all_files(stdlib_dir)

    joosc_executable = resolve_path(programs_dir, "../../joosc")

    integration_log_file = "integration.log"

    # Optional argument: -f indicates we should only print failed cases
    print_pass_cases = True
    if len(sys.argv) > 1 and sys.argv[1] == "-f":
        print_pass_cases = False
        failures = False

    # get current assignments from env
    currently_testing = os.getenv('TESTING_COVERAGE', '') # todo: replace with environment variable
    split_assignments = currently_testing.split(',')

    test_numbers = {}

    for assignment in os.listdir(programs_dir):
        if assignment in split_assignments:
            pass_count = 0
            fail_count = 0

            assignment_path = os.path.join(programs_dir, assignment)
            local_valid_tests = os.path.join(assignment_path, "local/valid")
            local_invalid_tests = os.path.join(assignment_path, "local/invalid")

            marmo_valid_tests = os.path.join(assignment_path, "marmoset/valid")
            marmo_invalid_tests = os.path.join(assignment_path, "marmoset/invalid")

            for directory, expected_code in ((local_valid_tests, 0), (local_invalid_tests, 42), (marmo_valid_tests, 0), (marmo_invalid_tests, 42)):
                header_bold_underline = colors.HEADER + colors.BOLD + colors.UNDERLINE
                print(f"\n{header_bold_underline}Testing all programs in {resolve_path(directory, '')}:{colors.ENDC}")

                if os.path.exists(directory):
                    for program in os.listdir(directory):
                        program_path = resolve_path(directory, program)

                      
                        if os.path.exists(program_path):
                            # if program is a directory, get all files from the direcory and add to a list
                            files = [program_path]
                            if os.path.isdir(program_path):
                                files = get_all_files(program_path)

                            with open(integration_log_file, "w") as outfile:
                                result = subprocess.run([joosc_executable, *files, *stdlib_files], stderr=outfile)

                            if result.returncode == expected_code:
                                if print_pass_cases: # Test passed, display output if -f is not set
                                    print(f"{colors.OKGREEN}SUCCESS: Running joosc on {program} successfully returned {expected_code}{colors.ENDC}")
                                    print_file_contents(integration_log_file)
                                pass_count += 1
                            else:
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
