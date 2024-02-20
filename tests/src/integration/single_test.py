import os, subprocess, sys
from pathlib import Path

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

def get_all_files(directory):
    file_list = []
    for root, _, files in os.walk(directory):
        for file in files:
            if file.endswith('.java'):
                file_list.append(Path(os.path.join(root, file)).resolve())
    return file_list


def run_test(test_path):
    # test path may be directory, or may be file
    integration_dir = os.path.dirname(__file__)
    stdlib_dir = os.path.join(integration_dir, "../../stdlib")
    stdlib_files = get_all_files(stdlib_dir)

    joosc_executable = resolve_path(integration_dir, '../../../joosc')

    if not os.path.exists(test_path):
        sys.exit("Test path doesn't exist")
    
    test_files = []
    if os.path.isdir(test_path):
        test_files = get_all_files(test_path)
    else:
        test_files = [test_path]
    
    result = subprocess.run([joosc_executable, *test_files, *stdlib_files])
    
    expected_returncode = 42 if "invalid" in test_path else 0

    if result.returncode == expected_returncode:
        print(f"{colors.OKGREEN}SUCCESS: Running joosc on {test_path} successfully returned {result.returncode}{colors.ENDC}")
    else:
        print(f"{colors.FAIL}FAIL: Running joosc on {test_path} returned {result.returncode}, expected {expected_returncode}.{colors.ENDC}")



if __name__ == "__main__":
    args = sys.argv
    if len(args) < 2:
        print("Must pass test file / test directory.")
        sys.exit(1)
    test_path = args[1]

    sys.exit(run_test(test_path))
