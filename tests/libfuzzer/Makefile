.PHONY : build clean

build : 
	ninja --version && (mkdir -p build && cd build && cmake .. -G Ninja && ninja)

clean :
	rm -rf build

# CXX = g++
# CXXFLAGS = -g -Wall

# EXEC = a.out
# OBJECTS = proto_to_joos.o proto_to_joos_main.o
# DEPENDS = ${OBJECT:.o=.d}

# # SRC_FILE = proto_to_joos_main.cc

# PROTO_FILE = joosc_proto.proto
# PROTO_HDR = $(basename ${PROTO_FILE}).pb.h
# PROTO_SRC = $(basename ${PROTO_FILE}).pb.cc
# PROTO_OBJS = ${PROTO_HDR} ${PROTO_SRC}

# ####################################################

# .PHONY : clean

# ${EXEC} : ${PROTO_OBJS} ${OBJECTS}
# 	${CXX} ${CXXFLAGS} $^ -o $@
# 	# ninja --version && (mkdir -p build && cd build && cmake .. -G Ninja && ninja)

# ${PROTO_OBJS} : ${PROTO_FILE}
# 	protoc --cpp_out=. ${PROTO_FILE}

# -include ${DEPENDS}

# clean:
# 	rm -rf ${PROTO_OBJS} ${DEPENDS} ${OBJECTS} ${EXEC}
