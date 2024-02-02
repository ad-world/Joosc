all: build

generate-parser:
	make -C src/parsing/bison

graph:
	(mkdir -p build && cd build && cmake -DGRAPHVIZ=ON .. && make && cp joosc ../joosc)

build: | generate-parser
	(mkdir -p build && cd build && cmake .. && make && cp joosc ../joosc)

unit-test: build
	(cd build && ctest)

scanner-test: build
	(./tests/lexing/run_tests.sh)
	
integration-test: build
	python3 tests/src/integration/valid_invalid_prog_test.py

clean:
	rm -f joosc && rm -f *.log && rm -f *.zip
	make clean -C src/parsing/bison
	rm -r -f ./build

submission: build test
	git log > a${anum}.log && zip a${anum}.zip -r src/ tests/ CMakeLists.txt Makefile a${anum}.log -x src/parsing/bison/location.hh src/parsing/bison/parser.cc src/parsing/bison/parser.hh src/parsing/bison/parser.output src/parsing/bison/scanner.cc
	rm a${anum}.log 

.PHONY: all build test clean submission generate-parser integration-test
