FROM ubuntu

RUN apt-get update -y -qq
RUN apt-get install -y -qq python3 python3-pip ninja-build gcc flex bison valgrind time git zip wget clang

RUN pip install cmake

WORKDIR /root

# Build protobuf 25.2
# RUN wget https://github.com/protocolbuffers/protobuf/releases/download/v25.2/protobuf-25.2.zip -O protobuf.zip
# RUN unzip protobuf.zip
RUN git clone https://github.com/protocolbuffers/protobuf/ protobuf
RUN cd protobuf && \
    git checkout v25.2 && \
    git submodule update --init --recursive && \
    cmake -GNinja -DABSL_PROPAGATE_CXX_STD=ON -Dprotobuf_BUILD_TESTS=OFF -DCMAKE_CXX_STANDARD=17 . && \
    cmake --build . && \
    cmake --install .

WORKDIR /myapp
