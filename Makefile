all: build test scanner-test

generate-parser:
	make -C src/parsing/bison

build: | generate-parser
	(mkdir -p build && cd build && cmake .. && make && cp joosc ../joosc)

test: build
	(cd build && ctest)

scanner-test: build
	(./tests/lexing/run_tests.sh)
	
integration-test:
	python3 tests/src/integration/valid_invalid_prog_test.py

clean:
	rm -f joosc && rm -f *.log && rm -f *.zip
	(cd src/parsing/bison && rm -f parser.hh parser.cc scanner.cc location.hh) 
	rm -r -f ./build

submission: build test
	git log > a${anum}.log && zip a${anum}.zip -r src/ tests/ CMakeLists.txt Makefile a${anum}.log && rm a${anum}.log

.PHONY: all build test clean submission generate-parser integration-test
