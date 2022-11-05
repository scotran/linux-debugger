FROM ubuntu
WORKDIR /debugger

RUN apt-get update && \
    apt-get -y install \
    cmake \
    protobuf-compiler \
    clang \
    clang-tidy \
    g++ \
    git

COPY extern ./extern

WORKDIR /debugger/extern/libelfin

RUN make install

WORKDIR /debugger

COPY . .

WORKDIR /debugger/build

RUN CC=clang CXX=clang++ cmake ..
RUN cmake --build . -v
