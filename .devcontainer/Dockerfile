FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    gcc-13 g++-13 \
    clang-17 clang-format-17 clang-tidy-17 \
    ninja-build \
    cmake \
    git \
    pkg-config \
    gdb \
    gdbserver \
    libpetsc-real-dev \
    libopenmpi-dev \
    mpi-default-dev \
    libblas-dev \
    liblapack-dev \
    wget curl vim \
    python3 python3-pip \
    && rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

RUN pkg-config --modversion PETSc \
 && mpirun --version \
 && cmake --version \
 && gcc --version

WORKDIR /workspace