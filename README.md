# bspsGPU

bspsGPU is research-purpose software for evaluating non-uniform B-spline surfaces on GPUs, including an implementation of the following paper:

Takashi Kanai: “Fragment-based Evaluation of Non-Uniform B-spline Surfaces on GPUs”, Computer-Aided Design and Applications (Proc. CAD’07 Conference, Honolulu, Hawaii, 25–29 June, 2007), Vol.4, Nos.1–4, pp.287–294, 2007.

The original code (2004–2007) targeted **NVIDIA Cg**. The current tree uses **OpenGL + GLSL** (via GLEW), builds with **CMake**, and has been verified on **macOS**, **Ubuntu**, and **Windows** (Visual Studio with **vcpkg**). 

Rhino **.3dm** import uses **[OpenNURBS](https://github.com/mcneel/opennurbs)** as a **git submodule** at `external/opennurbs` (static library `opennurbsStatic`, C++17). The reader path uses current OpenNURBS APIs (`ONX_Model` / `ON_ModelGeometryComponent`, and so on).

**Raster images (e.g. `lattice.png`):** decoding uses **[stb_image](https://github.com/nothings/stb)** (`external/stb/stb_image.h`, [MIT / public domain](https://github.com/nothings/stb/blob/master/LICENSE)). A small header `PNGImage.hxx` wraps `stbi_load` for the few call sites in `bspsgpu.cxx`; `**stb_image_impl.cxx`** is the only translation unit that defines `STB_IMAGE_IMPLEMENTATION` (see stb’s usage notes). **libpng** is not required for the viewer build.

## Getting started (CMake)

Clone **with submodules** so `external/opennurbs`, `external/vecmath-cpp`, and `external/render` are populated:

```bash
git clone https://github.com/kanait/bspsGPU.git --recursive
# If you already cloned without submodules:
git submodule update --init --recursive

cmake -B build -S . -DCMAKE_BUILD_TYPE=Release
cmake --build build
./build/bspsgpu path/to/model.3dm
```

### Windows (Visual Studio + vcpkg)

The repo includes `**vcpkg.json**` (GLEW, zlib, and **freeglut** on Windows; PNG loading is via vendored **stb_image**, not vcpkg’s libpng). From a **Developer Command Prompt** or Visual Studio’s CMake integration, point CMake at the vcpkg toolchain file, then configure and build:

```bat
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
cmake --build build --config Release
build\Release\bspsgpu.exe path\to\model.3dm
```

If `%VCPKG_ROOT%` is not set, replace it with the path to your vcpkg clone (for example `C:\dev\vcpkg`).

Usage: pass a **Rhino 3DM** (or bsps) file as the first argument. Optional view files and other flags depend on your local `bspsgpu.cxx` build.

### GLSL shaders (embedded at build time)

GPU programs live under `shaders/*.glsl`. At **build** time, `cmake/embed_shaders.cmake` generates `build/generated/bspsgpu_embedded_shaders.h` so the executable does **not** need to read shader files from disk at runtime. Constants shared with C++ (for example draw modes) are documented in `BSPS.hxx` and must stay in sync with `shaders/bsps.frag.glsl`.

Installed layouts still ship the `shaders/` directory for reference (`cmake` install rule).

### Legacy Windows binary

An older **Win32/x64** zip release (Cg-era) is still available for reference:  
[Release v1.0](https://github.com/kanait/bspsGPU/releases/tag/v1.0) — use `run_surface.bat` / `run_spray.bat` only with that bundle.

## Prerequisites (CMake build)


| Component                                                | Notes                                                                                                                                                                       |
| -------------------------------------------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **CMake**                                                | 3.16 or newer (matches OpenNURBS and this project)                                                                                                                          |
| **C++17** compiler                                       | GCC, Clang, MSVC (OpenNURBS upstream expectation)                                                                                                                           |
| **[OpenNURBS](https://github.com/mcneel/opennurbs)**     | Git submodule `external/opennurbs` (initialized with `--recursive` or `git submodule update --init`)                                                                        |
| **OpenGL** + **GLU**                                     | System frameworks / packages                                                                                                                                                |
| **[GLEW](http://glew.sourceforge.net/)**                 | e.g. `libglew-dev` (Debian/Ubuntu), `glew` (Homebrew), or vcpkg port `glew` (CONFIG) on Windows                                                                             |
| **GLUT**                                                 | macOS **GLUT.framework**, Linux **freeglut** / **libglut**; Windows + vcpkg uses port `**freeglut`** (`FreeGLUT::freeglut`)                                                 |
| **zlib**                                                 | e.g. `zlib1g-dev` on Debian/Ubuntu (OpenNURBS / system packages)                                                                                                            |
| **stb_image**                                            | Vendored at `external/stb/stb_image.h` ([nothings/stb](https://github.com/nothings/stb)); used for optional PNG/JPEG/etc. textures (**no libpng** in CMake or `vcpkg.json`) |
| **[vecmath-cpp](https://github.com/yuki12/vecmath-cpp)** | Submodule `external/vecmath-cpp`                                                                                                                                            |
| **[render](https://github.com/kanait/render)**           | Submodule `external/render`                                                                                                                                                 |


**Cg Toolkit** is no longer used for the CMake viewer path.

### Updating the OpenNURBS submodule

After `git submodule update --remote external/opennurbs`, run a full rebuild. Keep **upstream OpenNURBS unmodified**; platform glue (for example bundled zlib C symbols on Linux/GNU ld, and CMake deprecation noise from nested `cmake_minimum_required`) lives in this repository (`opennurbs_zlib_alloc_glue.cxx`, `CMakeLists.txt` around `add_subdirectory`).

## Authors

- **[Takashi Kanai](https://graphics.c.u-tokyo.ac.jp/hp/en/)** — The University of Tokyo

## License

This software is licensed under the MIT License — see [LICENSE.md](LICENSE.md).