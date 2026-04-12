# CMake build for bspsGPU (Ubuntu, macOS, Windows + Visual Studio / vcpkg).
# OpenNURBS is built from external/opennurbs as a static library.

cmake_minimum_required(VERSION 3.16)
project(bspsgpu LANGUAGES CXX)

# Upstream mcneel/opennurbs builds with C++17; keep the executable compatible with its headers.
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- OpenNURBS (zlib is resolved inside external/opennurbs/CMakeLists.txt) ---
add_subdirectory(external/opennurbs)

# --- Dependencies for the viewer / I/O ---
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

# GLEW: vcpkg supplies CONFIG mode (GLEW::GLEW); otherwise fall back to FindGLEW / manual find.
find_package( GLEW CONFIG QUIET )
if( NOT TARGET GLEW::GLEW )
  find_package( GLEW QUIET )
  if( NOT GLEW_FOUND )
    find_path( GLEW_INCLUDE_DIR GL/glew.h )
    find_library( GLEW_LIBRARY NAMES GLEW glew32 glew libglew32 )
    if( GLEW_INCLUDE_DIR AND GLEW_LIBRARY )
      add_library( GLEW::GLEW UNKNOWN IMPORTED )
      set_target_properties( GLEW::GLEW PROPERTIES
        IMPORTED_LOCATION "${GLEW_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}" )
      set( GLEW_FOUND TRUE )
    endif()
  endif()
endif()
if( NOT TARGET GLEW::GLEW )
  message( FATAL_ERROR
    "GLEW not found. On Windows with vcpkg, install port 'glew' and pass "
    "-DCMAKE_TOOLCHAIN_FILE=.../vcpkg.cmake. On Debian/Ubuntu: libglew-dev; Homebrew: glew." )
endif()

# GLUT: macOS uses the system Apple GLUT framework (avoids freeglut/Cocoa + GLEW context issues).
#       Windows (vcpkg) prefers FreeGLUT CONFIG; other platforms fall back to FindGLUT.
set( _bspsgpu_glut_target "" )
if( APPLE )
  find_package( GLUT REQUIRED )
  if( NOT TARGET GLUT::GLUT )
    add_library( GLUT::GLUT UNKNOWN IMPORTED )
    set_target_properties( GLUT::GLUT PROPERTIES
      IMPORTED_LOCATION "${GLUT_glut_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GLUT_INCLUDE_DIR}"
      INTERFACE_LINK_LIBRARIES "${GLUT_LIBRARIES}" )
  endif()
  set( _bspsgpu_glut_target GLUT::GLUT )
else()
  find_package( FreeGLUT CONFIG QUIET )
  if( TARGET FreeGLUT::freeglut )
    set( _bspsgpu_glut_target FreeGLUT::freeglut )
  elseif( TARGET FreeGLUT::freeglut_static )
    set( _bspsgpu_glut_target FreeGLUT::freeglut_static )
  endif()
  if( NOT _bspsgpu_glut_target )
    find_package( GLUT REQUIRED )
    if( NOT TARGET GLUT::GLUT )
      add_library( GLUT::GLUT UNKNOWN IMPORTED )
      set_target_properties( GLUT::GLUT PROPERTIES
        IMPORTED_LOCATION "${GLUT_glut_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GLUT_INCLUDE_DIR}" )
    endif()
    set( _bspsgpu_glut_target GLUT::GLUT )
  endif()
endif()

# Embed shaders/*.glsl into a generated header (no runtime disk read for GLSL).
set( _bspsgpu_generated_dir "${CMAKE_CURRENT_BINARY_DIR}/generated" )
file( MAKE_DIRECTORY "${_bspsgpu_generated_dir}" )
set( _bspsgpu_embedded_h "${_bspsgpu_generated_dir}/bspsgpu_embedded_shaders.h" )
add_custom_command(
  OUTPUT "${_bspsgpu_embedded_h}"
  COMMAND ${CMAKE_COMMAND} -E chdir "${CMAKE_CURRENT_SOURCE_DIR}"
    ${CMAKE_COMMAND} -DOUT=${_bspsgpu_embedded_h}
    -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_shaders.cmake"
  DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/embed_shaders.cmake"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/bsps.vert.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/bsps.frag.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/isophoto.vert.glsl"
    "${CMAKE_CURRENT_SOURCE_DIR}/shaders/isophoto.frag.glsl"
  COMMENT "Generating bspsgpu_embedded_shaders.h"
  VERBATIM )

add_executable( bspsgpu
  bspsgpu.cxx
  BSPSIO.cxx
  DM3IO.cxx
  POLIO.cxx
)

target_sources( bspsgpu PRIVATE "${_bspsgpu_embedded_h}" )
set_source_files_properties( "${_bspsgpu_embedded_h}" PROPERTIES GENERATED TRUE )

target_include_directories( bspsgpu PRIVATE
  "${_bspsgpu_generated_dir}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/render"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/vecmath-cpp"
)

# Linux (GNU ld): bundled zlib calls zcalloc/zcfree in libopennurbsStatic.a; libzlib.a must
# be rescanned with that archive. Keep only those two inside --start-group/--end-group.
if( UNIX AND NOT APPLE )
  # Do not use "LINKER:..." here: Makefile generators may pass -lLINKER:--start-group.
  target_link_libraries( bspsgpu PRIVATE "-Wl,--start-group" opennurbsStatic zlib
    "-Wl,--end-group" )
else()
  target_link_libraries( bspsgpu PRIVATE opennurbsStatic )
endif()
target_link_libraries( bspsgpu PRIVATE
  OpenGL::GL
  OpenGL::GLU
  GLEW::GLEW
  PNG::PNG
  ZLIB::ZLIB
  ${_bspsgpu_glut_target}
)

if( APPLE )
  target_compile_definitions( bspsgpu PRIVATE GL_SILENCE_DEPRECATION )
endif()

if( MSVC )
  target_compile_definitions( bspsgpu PRIVATE NOMINMAX )
  # Source comments may be UTF-8; avoids C4819 on Japanese (932) code pages.
  target_compile_options( bspsgpu PRIVATE /utf-8 )
endif()

if( UNIX AND NOT APPLE )
  target_link_libraries( bspsgpu PRIVATE m )
endif()

if( WIN32 )
  target_link_libraries( bspsgpu PRIVATE Rpcrt4 )
endif()

# Optional: copy runtime assets next to the executable when present
if( EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lattice.png" )
  add_custom_command( TARGET bspsgpu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/lattice.png"
      "$<TARGET_FILE_DIR:bspsgpu>/lattice.png"
    COMMENT "Copying lattice.png next to bspsgpu" )
endif()

install( TARGETS bspsgpu RUNTIME DESTINATION bin )
install( DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/" DESTINATION share/bspsgpu/shaders )
