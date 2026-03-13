# Building ITurbo

This document covers everything you need to configure, build, test, and package ITurbo across all supported environments.

---

## Table of Contents

- [Prerequisites](#prerequisites)
  - [Dev Container (recommended)](#dev-container-recommended)
  - [Windows (no PETSc)](#windows-no-petsc)
  - [WSL2 manual setup](#wsl2-manual-setup)
- [Cloning the repository](#cloning-the-repository)
- [Quick start](#quick-start)
- [CMake presets reference](#cmake-presets-reference)
- [Building](#building)
- [Testing](#testing)
- [Packaging and distribution](#packaging-and-distribution)
- [Dependency notes](#dependency-notes)
- [Troubleshooting](#troubleshooting)

---

## Prerequisites

### Dev Container (recommended)

This is the standard development environment. Every developer gets identical toolchain versions regardless of their host machine.

**Required software on your Windows host:**

| Software | Purpose | Download |
|---|---|---|
| Docker Desktop | Container runtime (enable WSL2 backend) | https://www.docker.com/products/docker-desktop |
| Visual Studio 2022 | IDE with WSL2/container support | https://visualstudio.microsoft.com |
| VS workload: `Desktop development with C++` | Core C++ tooling | Via VS Installer |
| VS workload: `Linux and embedded development with C++` | Dev Container + WSL2 support | Via VS Installer |

Or if you prefer VS Code:

| Software | Purpose |
|---|---|
| VS Code | Editor |
| Extension: `Dev Containers` (ms-vscode-remote.remote-containers) | Container integration |
| Extension: `C/C++ Extension Pack` (ms-vscode.cpptools-extension-pack) | IntelliSense, CMake |

Once Docker Desktop is running, open the repository folder and choose **Reopen in Container** when prompted. VS / VS Code will build the image on first launch (takes a few minutes) and drop you into a fully configured environment.

The container provides:

- GCC 13 and Clang 17
- CMake 3.28+, Ninja
- PETSc 3.20 (real variant) + LAPACK/BLAS
- OpenMPI
- GDB + gdbserver
- clang-tidy, clang-format
- MinGW-w64 (for cross-compiling Windows binaries from inside the container)

---

### Windows (no PETSc)

Use this if you want a fast native Windows build for iterating on core logic, tests, and non-solver code. PETSc is not available here.

**Required:**

- Visual Studio 2022 with `Desktop development with C++`
- [CMake 3.26+](https://cmake.org/download/) (add to PATH)
- [Ninja](https://ninja-build.org/) (add to PATH, or install via `winget install Ninja-build.Ninja`)
- Git for Windows with submodule support

**Optional (enables MPI on Windows):**

```powershell
winget install Microsoft.MPI
```

Verify after install:

```powershell
mpiexec --version
cmake --version
ninja --version
```

---

### WSL2 manual setup

If you prefer not to use Docker but still want PETSc, you can configure WSL2 directly.

```powershell
# Elevated PowerShell
wsl --install -d Ubuntu-24.04
wsl --set-default Ubuntu-24.04
```

Inside WSL2:

```bash
sudo apt update && sudo apt upgrade -y

sudo apt install -y \
    build-essential gcc-13 g++-13 \
    clang-17 clang-format-17 clang-tidy-17 \
    ninja-build cmake git pkg-config gdb \
    libpetsc-real-dev \
    libopenmpi-dev mpi-default-dev \
    libblas-dev liblapack-dev

# Verify
pkg-config --modversion PETSc
mpirun --version
cmake --version
```

Then open the repository from Visual Studio via **Tools → Options → Cross Platform → Connection Manager → Add → localhost (WSL)**.

---

## Cloning the repository

Always clone with `--recurse-submodules` to initialize vendored dependencies (Catch2, etc.):

```bash
git clone --recurse-submodules https://github.com/your-org/iturbo.git
cd iturbo
```

If you already cloned without it:

```bash
git submodule update --init --recursive
```

Vendored dependencies live in `vendor/` and are pinned to specific release tags. Do not run `git submodule update` without checking the pinned versions in `.gitmodules` first.

---

## Quick start

The fastest path from clone to passing tests, depending on your environment:

**Dev container / WSL2 (full stack):**

```bash
cmake --workflow --preset dev
```

This runs configure → build → test in one command using the `container-debug` preset.

**Windows (no PETSc):**

```powershell
cmake --workflow --preset windows-dev
```

---

## CMake presets reference

All presets are defined in `CMakePresets.json` at the repository root. Visual Studio and VS Code read this file automatically and populate their configuration dropdowns.

### Linux / Dev Container presets

| Preset | Build type | PETSc | MPI | Sanitizers | LTO | Notes |
|---|---|---|---|---|---|---|
| `container-debug` | Debug | ✓ | ✓ | ASan + UBSan | ✗ | **Primary dev preset** |
| `container-reldebug` | RelWithDebInfo | ✓ | ✓ | ✗ | ✗ | Performance testing with symbols |
| `container-release` | Release | ✓ | ✓ | ✗ | ✓ | Production Linux build |
| `container-clang-debug` | Debug | ✓ | ✓ | ASan + UBSan | ✗ | Clang 17 build |
| `container-clang-tidy` | Debug | ✓ | ✓ | ✗ | ✗ | Static analysis, `-Werror` |
| `ci-linux` | RelWithDebInfo | ✓ | ✓ | ✗ | ✗ | Used by GitHub Actions |

### Windows presets

| Preset | Build type | PETSc | MPI | Notes |
|---|---|---|---|---|
| `windows-debug` | Debug | ✗ | ✗ | Fast iteration, no MPI |
| `windows-reldebug` | RelWithDebInfo | ✗ | ✗ | |
| `windows-release` | Release | ✗ | ✗ | LTO on, tests off |
| `windows-mpi-debug` | Debug | ✗ | ✓ | Requires MS-MPI SDK |
| `ci-windows` | RelWithDebInfo | ✗ | ✓ | Used by GitHub Actions |

### Cross-compilation presets (Linux host → Windows target)

| Preset | Build type | Toolchain | Notes |
|---|---|---|---|
| `mingw-cross-release` | Release | MinGW-w64 | LTO on, produces `.exe` |
| `mingw-cross-reldebug` | RelWithDebInfo | MinGW-w64 | Debug symbols in `.exe` |

### Workflow presets (configure + build + test in one command)

| Preset | Steps | Use case |
|---|---|---|
| `dev` | configure → build → test | Daily development |
| `ci-linux` | configure → build → test | CI gate check |
| `ci-windows` | configure → build → test | CI gate check |
| `release-linux` | configure → build → package | Produce Linux `.tar.gz` |
| `release-windows` | configure → build → package | Produce Windows `.zip` |
| `release-mingw` | configure → build → package | Cross-compile Windows `.zip` |

---

## Building

### Configure only

```bash
cmake --preset container-debug
```

Build files are written to `build/<preset-name>/`. The `build/` directory is gitignored.

### Build only (after configure)

```bash
cmake --build --preset container-debug
```

Ninja uses all available cores by default (`"jobs": 0` in the preset).

### Configure + build

```bash
cmake --preset container-debug
cmake --build --preset container-debug
```

Or with a workflow preset:

```bash
cmake --workflow --preset dev
```

### Passing extra options at configure time

Override any cache variable without editing the preset:

```bash
cmake --preset container-debug \
    -DITURBO_ENABLE_CGNS=ON \
    -DITURBO_ENABLE_BENCHMARKS=ON
```

### Cleaning

```bash
cmake --build --preset container-debug --target clean
```

Or delete the build directory entirely:

```bash
rm -rf build/container-debug
```

---

## Testing

Tests are built with [Catch2 v3](https://github.com/catchorg/Catch2) and discovered by CTest.

### Run all tests

```bash
ctest --preset container-debug
```

### Run with verbose output

```bash
ctest --preset container-debug --verbose
```

### Run a specific test by name (regex match)

```bash
ctest --preset container-debug -R "mesh"
```

### Run tests in parallel

```bash
ctest --preset container-debug -j 8
```

The `ci-linux` and `ci-windows` presets set `stopOnFailure: true` — they abort on the first failure. The dev presets run all tests and report all failures at the end.

### Running MPI tests manually

CTest handles MPI test launch automatically via `mpirun`. To run a specific MPI test manually:

```bash
mpirun -n 4 ./build/container-debug/bin/iturbo_tests "[mpi]"
```

### Running with sanitizers

The `container-debug` preset enables AddressSanitizer and UBSan. If a sanitizer fires, the test binary prints the full stack trace and exits non-zero, which CTest reports as a failure. No extra flags needed.

---

## Packaging and distribution

ITurbo ships two distinct artifacts:

- **Linux binary** — built in the container with full PETSc + MPI support, targeted at HPC clusters
- **Windows binary** — built with MSVC or MinGW, no PETSc, targeted at desktop preprocessing and smaller runs

### Linux package (`.tar.gz`)

```bash
cmake --workflow --preset release-linux
# Output: dist/ITurbo-<version>-Linux.tar.gz
```

### Windows package (`.zip`) — native MSVC build

Run on a Windows machine or in a `windows-latest` GitHub Actions runner:

```powershell
cmake --workflow --preset release-windows
# Output: dist/ITurbo-<version>-Windows.zip
```

### Windows package — cross-compiled from Linux

Build a native Windows `.exe` entirely from inside the dev container using MinGW-w64:

```bash
cmake --workflow --preset release-mingw
# Output: dist/ITurbo-<version>-Windows.zip
```

The cross-compiled binary links the MinGW runtime statically, so it runs on a clean Windows machine with no additional DLLs.

### Windows installer (NSIS)

Requires [NSIS](https://nsis.sourceforge.io) installed on the build machine:

```powershell
cmake --preset windows-release
cmake --build --preset windows-release
cd build/windows-release
cpack -G NSIS
# Output: dist/ITurbo-<version>-win64-installer.exe
```

---

## Dependency notes

### Vendored (git submodules in `vendor/`)

| Library | Version | Purpose |
|---|---|---|
| Catch2 | v3.7.1 | Unit and integration tests |

Submodule versions are pinned. To update a submodule to a new release:

```bash
cd vendor/catch2
git fetch --tags
git checkout v3.x.y
cd ../..
git add vendor/catch2
git commit -m "chore: bump catch2 to v3.x.y"
```

### System dependencies (not vendored)

| Library | Why not vendored | How to install |
|---|---|---|
| PETSc | Build is enormous, deeply system-specific | `apt install libpetsc-real-dev` (Ubuntu) / `pacman -S mingw-w64-x86_64-petsc` (MSYS2) |
| OpenMPI / MS-MPI | Platform-specific ABI | `apt install libopenmpi-dev` (Linux) / `winget install Microsoft.MPI` (Windows) |
| BLAS / LAPACK | Hardware-optimised, must match system | `apt install libblas-dev liblapack-dev` |
| OpenMP | Compiler built-in | GCC and Clang ship it; MSVC has `/openmp` |

### C++ standard library (`std::format`)

ITurbo uses C++23 `std::format` directly. No external formatting library (e.g. fmtlib) is required. GCC 13 and Clang 17 both provide full `std::format` support. MSVC 17.4+ (VS 2022) also supports it.

If you see a compile error like `'format' is not a member of 'std'`, your compiler is too old — check the minimum version table:

| Compiler | Minimum version |
|---|---|
| GCC | 13 |
| Clang | 17 |
| MSVC | 19.34 (VS 2022 17.4) |

### PETSc discovery

CMake locates PETSc via `pkg-config` first (works for all apt/pacman installs), then falls back to the `PETSC_DIR` / `PETSC_ARCH` environment variables (manual builds). To verify CMake found PETSc:

```bash
cmake --preset container-debug 2>&1 | grep -i petsc
# Should print: [deps] PETSc 3.20.x found at /usr/include/petsc
```

If PETSc is not found:

```bash
pkg-config --modversion PETSc      # check pkg-config sees it
echo $PETSC_DIR                    # check env var fallback
apt list --installed | grep petsc  # check it's actually installed
```

---

## Troubleshooting

### Submodules are empty after clone

```bash
git submodule update --init --recursive
```

### CMake can't find Ninja

```bash
# Linux
sudo apt install ninja-build

# Windows
winget install Ninja-build.Ninja
# then restart your terminal so PATH is updated
```

### PETSc not found on Ubuntu

The apt package installs headers to a versioned path. Confirm pkg-config resolves it:

```bash
pkg-config --cflags PETSc
pkg-config --libs   PETSc
```

If those commands fail but the package is installed, the `pkg-config` path may need updating:

```bash
export PKG_CONFIG_PATH=/usr/lib/x86_64-linux-gnu/pkgconfig:$PKG_CONFIG_PATH
pkg-config --modversion PETSc
```

Add the export to your `~/.bashrc` or `~/.profile` to make it permanent.

### `std::format` not available (MSVC)

Ensure you are using VS 2022 17.4 or later. Check in VS: **Help → About Microsoft Visual Studio** and look for the version number. Update via the VS Installer if needed.

### Sanitizer false positive from MPI

OpenMPI allocates memory in ways that can trigger AddressSanitizer. If you see ASan reports from inside MPI internals, suppress them with:

```bash
export ASAN_OPTIONS=detect_leaks=0
ctest --preset container-debug
```

For a more targeted suppression, create `asan_suppressions.txt` at the repo root:

```
leak:libmpi
leak:libopen-pal
```

Then:

```bash
export ASAN_OPTIONS=suppressions=/workspace/asan_suppressions.txt
```

### Docker build fails on first container launch

Check Docker Desktop is running and the WSL2 backend is enabled: **Docker Desktop → Settings → General → Use the WSL2 based engine**. If the image build fails mid-way due to a network error, try:

```bash
docker system prune          # clear failed layers
# then reopen in container
```

### Wrong compiler selected on Linux

If CMake picks up an older GCC instead of GCC 13, force it explicitly:

```bash
cmake --preset container-debug \
    -DCMAKE_C_COMPILER=gcc-13 \
    -DCMAKE_CXX_COMPILER=g++-13
```

The `base-linux` hidden preset already sets this, but a stale CMake cache can override it. Delete the build directory and reconfigure from scratch if the issue persists.