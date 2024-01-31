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

    failures = False
    for directory, expected_code in ((valid_programs, 0), (invalid_programs, 42)):
        print(f"Testing all programs in {resolve_path(directory, '')}:")
        for program in os.listdir(directory):
            program_path = resolve_path(directory, program)
            result = subprocess.run([joosc_executable, program_path])
            if result.returncode == expected_code:
                print(colors.OKGREEN + f"Running joosc on {program} successfully returned {expected_code}" + colors.ENDC)
            else:
                print(colors.FAIL + f"FAIL: Running joosc on {program} returned {result.returncode}. Expected: {expected_code}" + colors.ENDC)
                failures = True
    
    print(colors.FAIL + "Tests had failures." + colors.ENDC if failures else colors.OKGREEN + "All tests succeeded!" + colors.ENDC)
    return 1 if failures else 0

if __name__ == "__main__":
    sys.exit(valid_invalid_prog_test())
