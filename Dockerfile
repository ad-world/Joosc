FROM gcc:11
RUN apt update -y -qq
RUN apt install -y -qq python3 python3-pip ninja-build gcc flex bison valgrind time git zip cmake
WORKDIR /myapp
