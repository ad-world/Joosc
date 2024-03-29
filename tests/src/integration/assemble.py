import os, subprocess, shutil
from helpers.helpers import *

def assemble_all_files(binary_name="main", output_path="../../../output"):
    """
    Emit the binary produced by assembling and linking all the assembly files in output_path.

    Return True if success, False if fail
    """
    output_dir = resolve_path(os.path.dirname(__file__), output_path)
    root_dir = resolve_path(os.path.dirname(__file__), "../../../")

    # Place runtime.s in the output directory
    runtime_source_path = resolve_path(root_dir, "./tests/stdlib/runtime.s")
    runtime_dest_path = resolve_path(output_dir, "./runtime.s")
    shutil.copyfile(runtime_source_path, runtime_dest_path)

    # Assemble
    for assembly_file in get_all_files(output_dir, ".s"):

        assemble_res = subprocess.run(["nasm", "-O1", "-f", "elf", "-g", "-F", "dwarf", assembly_file], cwd=output_dir)

        if assemble_res.returncode != 0:
            return False

    # Link
    link_res = subprocess.run(f"ld -melf_i386 -o {binary_name} output/*.o", shell=True, cwd=root_dir)
    return link_res.returncode == 0
