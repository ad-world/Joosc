all: build

generate-parser:
	make -C src/parsing/bison

graph: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake -DGRAPHVIZ=ON .. -G Ninja  && ninja && cp joosc ../joosc)) || \
	(mkdir -p build && cd build && cmake -DGRAPHVIZ=ON .. && make && cp joosc ../joosc)

build: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake .. -G Ninja && ninja && cp joosc ../joosc)) || \
	(mkdir -p build && cd build && cmake .. && make && cp joosc ../joosc)

fast-build: | generate-parser
	(ninja --version && (mkdir -p build && cd build && cmake .. -G Ninja && ninja && cp joosc ../joosc))

slow-build: | generate-parser
	(mkdir -p build && cd build && cmake .. && make && cp joosc ../joosc)

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

integration-test: 
	-make valid-invalid-test 
	-make compiled-output-test

clean:
	rm -f joosc && rm -f *.log && rm -f *.zip
	make clean -C src/parsing/bison
	rm -r -f ./build
	rm -f integration.log

submission: build test
	git log > a${anum}.log && zip a${anum}.zip -r src/ tests/ CMakeLists.txt Makefile a${anum}.log -x src/parsing/bison/location.hh src/parsing/bison/parser.cc src/parsing/bison/parser.hh src/parsing/bison/parser.output src/parsing/bison/scanner.cc 'tests/programs/*/marmoset/*'
	rm a${anum}.log 

.PHONY: all build test clean submission generate-parser integration-test integration-test-full
