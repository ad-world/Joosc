FROM fedora:latest

WORKDIR /myapp

RUN dnf makecache
RUN dnf install -y clang american-fuzzy-lop-clang cmake make flex bison ninja-build git xz-devel zlib-devel tmux
ENV AFL_SKIP_CPUFREQ=1
ENV AFL_AUTORESUME=1
ENV AFL_LLVM_DOCUMENT_IDS=/myapp/ids
ENV AFL_LLVM_ALLOWLIST=/myapp/allow.txt
COPY . .
RUN make fuzzer

RUN mkdir corpus
RUN echo -ne '\xa\x0' > corpus/blanktest
