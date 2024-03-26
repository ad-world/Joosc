import os, subprocess
from helpers.helpers import *

def assemble_all_files(binary_name="main", output_path="../../output"):
    """
    Emit the binary produced by assembling and linking all the assembly files in output_path.
    """
    output_dir = resolve_path(os.path.dirname(__file__), output_path)
    root_dir = resolve_path(output_path, "../")

    # Assemble
    for assembly_file in get_all_files(output_dir, ".s"):
        subprocess.run(["nasm", "-O1", "-f", "elf", "-g", "-F", "dwarf", assembly_file], cwd=output_dir)

    # Link
    subprocess.run(["ld", "-melf_i386", "-o", binary_name, "output/*.o"], cwd=root_dir)
