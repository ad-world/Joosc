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

def resolve_path(path1, path2):
    return str(Path(os.path.join(path1, path2)).resolve())

def print_file_contents(file):
    with open(file, "r") as outfile: 
        print(outfile.read())

def valid_invalid_prog_test():
    """
    Runs joosc on all programs in valid and invalid directories.
    Tests that joosc correctly validates valid and invalidates invalid programs.
    """
    integration_dir = os.path.dirname(__file__)
    programs_dir = os.path.join(integration_dir, "../../programs")

    valid_programs = os.path.join(programs_dir, "valid")
    invalid_programs = os.path.join(programs_dir, "invalid")

    joosc_executable = resolve_path(programs_dir, "../../joosc")

    integration_log_file = "integration.log"

    # Optional argument: -f indicates we should only print failed cases
    print_pass_cases = True
    if len(sys.argv) > 1 and sys.argv[1] == "-f":
        print_pass_cases = False

    failures = False
    for directory, expected_code in ((valid_programs, 0), (invalid_programs, 42)):

        header_bold_underline = colors.HEADER + colors.BOLD + colors.UNDERLINE
        print(f"\n{header_bold_underline}Testing all programs in {resolve_path(directory, '')}:{colors.ENDC}")

        for program in os.listdir(directory):

            program_path = resolve_path(directory, program)
            with open(integration_log_file, "w") as outfile:
                result = subprocess.run([joosc_executable, program_path], stderr=outfile)

            if result.returncode == expected_code:
                if print_pass_cases: # Test passed, display output if -f is not set
                    print(f"{colors.OKGREEN}SUCCESS: Running joosc on {program} successfully returned {expected_code}{colors.ENDC}")
                    print_file_contents(integration_log_file)
            else:
                print(f"{colors.FAIL}FAIL: Running joosc on {program} returned {result.returncode}. Expected: {expected_code}{colors.ENDC}")
                print_file_contents(integration_log_file)
                failures = True
    
    if failures:
        print(f"{colors.FAIL}\nERROR: Tests had failures.{colors.ENDC}")
        return 1
    else:
        print(f"{colors.OKGREEN}All tests succeeded!{colors.ENDC}")
        return 0

if __name__ == "__main__":
    sys.exit(valid_invalid_prog_test())
