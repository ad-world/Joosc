FROM fedora:latest

# Dependencies
RUN dnf makecache
RUN dnf install -y clang american-fuzzy-lop-clang cmake make flex bison ninja-build git xz-devel zlib-devel tmux ccache

# Get protobuf mutator
WORKDIR /protobuf_mutator
RUN git clone https://github.com/google/libprotobuf-mutator.git .
RUN git checkout af3bb18
RUN mkdir build
RUN cd build && cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DLIB_PROTO_MUTATOR_DOWNLOAD_PROTOBUF=ON \
    -DCMAKE_C_COMPILER:FILEPATH=clang \
    -DCMAKE_CXX_COMPILER:FILEPATH=clang++ \
    -DCMAKE_CXX_STANDARD=17 \
    -GNinja
RUN cd build && cmake --build .

# Copy local app
WORKDIR /myapp
ENV CCACHE_DIR=/ccache
ENV AFL_SKIP_CPUFREQ=1
ENV AFL_AUTORESUME=1
ENV AFL_LLVM_ALLOWLIST=/myapp/fuzz/allow.txt
COPY . .
# RUN make fuzzer

RUN echo 'make afl-fuzzer && afl-fuzz -i fuzz/corpus/ -o fuzz/output/ -- ./fuzzer' > start_fuzzing.sh
RUN chmod +x start_fuzzing.sh
