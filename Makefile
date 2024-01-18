all: build test

build:
	(mkdir -p build && cd build && cmake .. && make && cp joosc ../joosc)

test: build
	(cd build && ctest)

quickclean:
	rm joosc && rm *.log && rm *.zip

clean: quickclean
	rm -r ./build

submission: build test
	git log > a${anum}.log && zip a${anum}.zip -r src/ tests/ CMakeLists.txt Makefile a${anum}.log && rm a${anum}.log

.PHONY: all build test quickclean clean submission
