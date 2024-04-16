FROM fedora:latest

WORKDIR /myapp

RUN dnf makecache
RUN dnf install -y clang american-fuzzy-lop-clang cmake make flex bison ninja-build git xz-devel zlib-devel tmux ccache
ENV CCACHE_DIR=/ccache
ENV AFL_SKIP_CPUFREQ=1
ENV AFL_AUTORESUME=1
ENV AFL_LLVM_ALLOWLIST=/myapp/fuzz/allow.txt
COPY . .
# RUN make fuzzer

RUN mkdir corpus
RUN echo -ne '\xa\x0' > corpus/blanktest

RUN echo 'afl-fuzz -i corpus/ -o fuzzout/ -- ./fuzzer' > start_fuzzing.sh
RUN chmod +x start_fuzzing.sh
