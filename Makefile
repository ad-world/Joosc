all: build

CMAKE_ARGS=-DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++
CMAKE_FUZZER_ARGS=-DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DFUZZER=ON -DCMAKE_CXX_FLAGS=-O3

generate-parser:
	make -C src/parsing/bison

graph: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake -DGRAPHVIZ=ON ${CMAKE_ARGS} .. -G Ninja  && ninja && cp joosc ../joosc)) || \
	(mkdir -p build && cd build && cmake -DGRAPHVIZ=ON ${CMAKE_ARGS} .. && make && cp joosc ../joosc)

fuzzer: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake ${CMAKE_FUZZER_ARGS} -GNinja .. && cmake --build . && cp fuzz/fuzz_joosc ../fuzzer))

build: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake ${CMAKE_ARGS} .. -G Ninja && ninja && cp joosc ../joosc)) || \
	(mkdir -p build && cd build && cmake ${CMAKE_ARGS} .. && make && cp joosc ../joosc)

fast-build: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake ${CMAKE_ARGS} .. -G Ninja && ninja && cp joosc ../joosc))

slow-build: | generate-parser
	(mkdir -p build && cd build && cmake ${CMAKE_ARGS} .. && make && cp joosc ../joosc)

unit-test: build
	(cd build && ctest -V)

scanner-test: build
	(./tests/lexing/run_tests.sh)
	
valid-invalid-test: build
	python3 tests/src/integration/valid_invalid_prog_test.py -f

valid-invalid-test-full: build
	python3 tests/src/integration/valid_invalid_prog_test.py

valid-invalid-test-single: build
	python3 tests/src/integration/valid_invalid_prog_test.py -s ${path}

compiled-output-test: build
	python3 tests/src/integration/correct_output_test.py

compiled-output-test-single: build
	python3 tests/src/integration/correct_output_test.py -s ${path}

interpret-java-test: build
	python3 tests/src/integration/interpret_java.py ${path}

interpret-java-test-single: build
	python3 tests/src/integration/interpret_java.py -s ${path}

# Used to exit with non-zero if any test fails, but run them all anyway (no short-circuiting)
_integration-test-helper: valid-invalid-test compiled-output-test
integration-test:
	make --keep-going _integration-test-helper

clean:
	rm -f joosc && rm -f *.log && rm -f *.zip
	make clean -C src/parsing/bison
	rm -r -f ./build
	rm -f integration.log
	rm -f *.tmp
	rm -f fuzzer

submission: build
	@[ "${anum}" ] || ( echo "Must pass assignment number; e.g. make submission anum=1"; exit 1 )
	git log > a${anum}.log && zip a${anum}.zip -r src/ tests/ CMakeLists.txt Makefile a${anum}.log -x src/parsing/bison/location.hh src/parsing/bison/parser.cc src/parsing/bison/parser.hh src/parsing/bison/parser.output src/parsing/bison/scanner.cc 'tests/programs/*/marmoset/*'
	rm a${anum}.log 

.PHONY: clean submission generate-parser graph fuzzer
.PHONY: build fast-build slow-build
.PHONY: scanner-test
.PHONY: integration-test _integration-test-helper
.PHONY: valid-invalid-test valid-invalid-test-full valid-invalid-test-single
.PHONY: compiled-output-test compiled-output-test-single
