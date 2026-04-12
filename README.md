# bspsGPU

bspsGPU is research-purpose software for evaluating non-uniform B-spline surfaces on GPUs, including an implementation of the following paper:

Takashi Kanai: “Fragment-based Evaluation of Non-Uniform B-spline Surfaces on GPUs”, Computer-Aided Design and Applications (Proc. CAD’07 Conference, Honolulu, Hawaii, 25–29 June, 2007), Vol.4, Nos.1–4, pp.287–294, 2007.

The original code (2004–2007) targeted **NVIDIA Cg**. The current tree uses **OpenGL + GLSL** (via GLEW), builds with **CMake**, and has been verified on **macOS** and **Ubuntu**. A **Visual Studio** solution (`bspsgpu.sln`) may still be present for Windows-oriented workflows; the maintained build path is CMake.

## Getting started (CMake)

Clone submodules (vecmath-cpp and render), then place **OpenNURBS** under `external/opennurbs` (this repository expects it as a CMake subdirectory; see Prerequisites).

```bash
git clone https://github.com/kanait/bspsGPU.git --recursive
# Ensure external/opennurbs exists and contains its CMakeLists.txt
cmake -B build -S .
cmake --build build
./build/bspsgpu path/to/model.3dm
```

Usage: pass a **Rhino 3DM** (or bsps) file as the first argument. Optional view files and other flags depend on your local `bspsgpu.cxx` build.

### GLSL shaders (embedded at build time)

GPU programs live under `shaders/*.glsl`. At **build** time, `cmake/embed_shaders.cmake` generates `build/generated/bspsgpu_embedded_shaders.h` so the executable does **not** need to read shader files from disk at runtime. Constants shared with C++ (for example draw modes) are documented in `BSPS.hxx` and must stay in sync with `shaders/bsps.frag.glsl`.

Installed layouts still ship the `shaders/` directory for reference (`cmake` install rule).

### Legacy Windows binary

An older **Win32/x64** zip release (Cg-era) is still available for reference:  
[Release v1.0](https://github.com/kanait/bspsGPU/releases/tag/v1.0) — use `run_surface.bat` / `run_spray.bat` only with that bundle.

## Prerequisites (CMake build)

| Component | Notes |
|-----------|--------|
| **CMake** | 3.15 or newer |
| **C++11** compiler | GCC, Clang, MSVC |
| **[OpenNURBS](https://github.com/mcneel/opennurbs)** | Expected at `external/opennurbs` (static lib via `add_subdirectory`) |
| **OpenGL** + **GLU** | System frameworks / packages |
| **[GLEW](http://glew.sourceforge.net/)** | e.g. `libglew-dev` (Debian/Ubuntu), `glew` (Homebrew) |
| **GLUT** | System **GLUT** (e.g. macOS `GLUT.framework`, Linux `freeglut` / `libglut`) |
| **zlib**, **libpng** | e.g. `zlib1g-dev`, `libpng-dev` on Debian/Ubuntu |
| **[vecmath-cpp](https://github.com/yuki12/vecmath-cpp)** | Submodule `external/vecmath-cpp` |
| **[render](https://github.com/kanait/render)** | Submodule `external/render` |

**Cg Toolkit** is no longer used for the CMake viewer path.

## Authors

* **[Takashi Kanai](https://graphics.c.u-tokyo.ac.jp/hp/en/)** — The University of Tokyo

## License

This software is licensed under the MIT License — see [LICENSE.md](LICENSE.md).
