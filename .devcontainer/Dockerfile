FROM ubuntu:24.04

ARG DEBIAN_FRONTEND=noninteractive
ARG USERNAME=developer
ARG UID=1000
ARG GID=1000

# ---------------------------------------------------------------------------
# System packages
# ---------------------------------------------------------------------------
RUN apt-get update && apt-get install -y --no-install-recommends \
    # Build toolchain
    build-essential \
    gcc-13 g++-13 \
    clang-17 clang-format-17 clang-tidy-17 \
    ninja-build \
    cmake \
    git \
    pkg-config \
    # Debugger
    gdb \
    gdbserver \
    # PETSc + MPI
    libpetsc-real-dev \
    libopenmpi-dev \
    mpi-default-dev \
    libblas-dev \
    liblapack-dev \
    # Useful dev tools
    wget curl vim \
    python3 python3-pip \
    && rm -rf /var/lib/apt/lists/*

# ---------------------------------------------------------------------------
# Make GCC 13 the default
# ---------------------------------------------------------------------------
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-13 100 \
 && update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-13 100

# ---------------------------------------------------------------------------
# Non-root developer user (avoids file permission issues on bind mounts)
# ---------------------------------------------------------------------------
RUN groupadd --gid ${GID} ${USERNAME} \
 && useradd  --uid ${UID} --gid ${GID} \
             --create-home --shell /bin/bash \
             ${USERNAME}

USER ${USERNAME}
WORKDIR /workspace

# Verify key deps are visible to the user
RUN pkg-config --modversion PETSc \
 && mpirun --version \
 && cmake --version \
 && gcc --version