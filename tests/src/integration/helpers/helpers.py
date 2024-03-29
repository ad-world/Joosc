from pathlib import Path
import os

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
    HEADER_BOLD_UNDERLINE = HEADER + BOLD + UNDERLINE

# Function to load environment variables from .env file
def load_env_file(filename=".env"):
    with open(filename, "r") as f:
        for line in f:
            # Skip comments and empty lines
            if line.strip() and not line.strip().startswith("#"):
                key, value = line.strip().split("=")
                os.environ[key] = value

def resolve_path(path1, path2):
    return str(Path(os.path.join(path1, path2)).resolve())

def get_all_files(directory, ext):
    """
    Get all files from a directory with extension ext
    """
    file_list = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith(ext):
                file_list.append(os.path.join(root, file))
    return file_list

def print_file_contents(file):
    with open(file, "r") as outfile: 
        print(outfile.read())


def print_test_results(failures, test_results):
    if failures:
        print(f"{colors.FAIL}\nERROR: Tests had failures")
        for assignment, results in test_results.items():
            pass_count, fail_count, total_count = results['pass_count'], results['fail_count'], results['total_count']
            print(f"{colors.HEADER}\nTest suite for assignment {assignment}")
            print(f"{colors.OKGREEN}Passing tests: {pass_count}/{total_count}")
            print(f"{colors.FAIL}Failing tests: {fail_count}/{total_count}")
            print(f"{colors.ENDC}")
        return 1
    else:
        print(f"{colors.OKGREEN}All tests succeeded!{colors.ENDC}")
        return 0