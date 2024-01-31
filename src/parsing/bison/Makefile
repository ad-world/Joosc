# Universal makefile for single C++ program
#
# Use gcc flag -MMD (user) or -MD (user/system) to generate dependences among source files.
# Use make default rules for commonly used file-name suffixes and make variables names.
#
# % make [ a.out ]

########## Variables ##########

CXX = g++					# compiler
CXXFLAGS = -g -Wall -MMD			# compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

PARSER_FILES = parser.cc parser.hh parser.output location.hh
SCANNER_FILES = scanner.cc

########## Targets ##########

.PHONY : all clean test					# not file names

all : ${SCANNER_FILES} ${PARSER_FILES}

${SCANNER_FILES} : scanner.ll
	flex -o scanner.cc scanner.ll

${PARSER_FILES} : parser.yy
	bison --report=itemsets -Wcex -dv parser.yy -o parser.cc

${SCANNER_FILES} ${PARSER_FILES} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

# make implicitly generates rules to compile C++ files that generate .o files

clean :						# remove files that can be regenerated
	rm -f ${DEPENDS} ${OBJECTS} ${EXEC} ${PARSER_FILES} ${SCANNER_FILES}

test :
	make -C ../../../	# make root project
