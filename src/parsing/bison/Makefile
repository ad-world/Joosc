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
OBJECTS = lex_tester.o driver.o scanner.o parser.o					# object files forming executable
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXEC = lex_tester					# executable name

LEX_TESTING_DIR = ../../../tests/lexing

########## Targets ##########

.PHONY : clean					# not file names

${EXEC} : ${OBJECTS}				# link step
	${CXX} ${CXXFLAGS} $^ -o $@ && mv ${EXEC} ${LEX_TESTING_DIR}
${SCANNER_FILES} : scanner.ll
	flex -o scanner.cc scanner.ll

${PARSER_FILES} : parser.yy
	bison -dv parser.yy -o parser.cc

${OBJECTS} : ${SCANNER_FILES} ${PARSER_FILES}
${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

# make implicitly generates rules to compile C++ files that generate .o files

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f ${DEPENDS} ${OBJECTS} ${EXEC} ${PARSER_FILES} ${SCANNER_FILES}
