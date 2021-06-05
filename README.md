# bspsgpu

bspsgpu is a research code for evaluating non-uniform B-spline surfaces on GPUs, which includes an implementation of the following paper:

Takashi Kanai: “Fragment-based Evaluation of Non-Uniform B-spline Surfaces on GPUs”, Computer-Aided Design and Applications (Proc. CAD’07 Conference, Honolulu, Hawaii, 25-29 June, 2007), Vol.4, Nos.1-4, pp.287-294, 2007.

This software was originally developed in 2004-2007 and was renovated in 2021 so as to build successfully by Visual Studio 2019.

## Getting Started

This software can run only on Windows. 
At first, you may try to use binary release (Win32, x64), 
which is available from [here](https://github.com/kanait/bspsgpu/releases/tag/v1.0).
Uncompress zip file and then execute run_surface.bat or run_spray.bat.

## Prerequisites

The following libraries are at least required for successfully compiling this software.

### [openNURBS](https://www.rhino3d.com/opennurbs/)

Our code uses openNURBS library. Currently, only version 3 is supported.
You may compile our code by using other versions (including the latest version). In this case, however, several code modifications are needed.

### [Cg Toolkit](https://developer.nvidia.com/cg-toolkit/)

Our code uses Cg Toolkit library from nVIDIA. The latest version (Cg 3.1 release) is supported.

### [zlib](https://zlib.net/)

### [libpng](http://www.libpng.org/pub/png/libpng.html)

### [glew](http://glew.sourceforge.net/)

### glut (included in Cg Toolkit library)

### [vecmath-cpp](https://github.com/yuki12/vecmath-cpp)
### [render](https://github.com/kanait/render)

When you execute "git clone" with "--recursive" option, you will also get vecmath-cpp and render libraries as a submodule "external/vecmath-cpp" and "external/render":

```
git clone https://github.com/kanait/bspsgpu.git --recursive
```

## Authors

* **[Takashi Kanai](https://graphics.c.u-tokyo.ac.jp/hp/en/)** - The University of Tokyo

## License

This software is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
