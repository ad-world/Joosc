all : build

ROOT_DIR="../../"
INPUTS_DIR="INPUTS"

# CMAKE_ARGS=-DCMAKE_CXX_COMPILER='afl-clang-lto++' -DCMAKE_AR='llvm-ar-12' -DCMAKE_RANLIB='llvm-ranlib-12'
CMAKE_ARGS=-DCMAKE_CXX_COMPILER='afl-c++'

build: | generate-parser
	cd ${ROOT_DIR} && mkdir -p build && cd build \
		&& cmake ${CMAKE_ARGS} .. -G Ninja \
		&& ninja && cp joosc ../joosc

generate-parser:
	make generate-parser -C ${ROOT_DIR}

clean:
	make clean -C ${ROOT_DIR}
	rm -f ${INPUTS_DIR}/*

collect-inputs:
	mkdir -p ${INPUTS_DIR}
	(find ../programs/*/marmoset/valid/ -type f -maxdepth 1 | xargs cp -t ${INPUTS_DIR}) || true

.PHONY: all build generate-parser clean
