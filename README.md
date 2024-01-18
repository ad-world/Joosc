# CS444 Compiler

## Setup

- Install CMake version 3.16 or later with `sudo apt-get install cmake`

## Usage

- `make build`: Builds the compiler with CMake, and puts `joosc` executable in root directory. Object files are placed in `build` directory.
- `make test`: Runs all GoogleTest unit tests in `tests/src`.
- `make clean`: Deletes `build` directory, causing all files to be recompiled on next build.
- `make submission anum=<x>`: Creates zip file `a<x>.zip` for marmoset submission. This zip contains the source code, tests, the files needed to build, and the git log (all requirements).

## Development

New classes must be placed in subdirectories of `src` (otherwise they can't be built)