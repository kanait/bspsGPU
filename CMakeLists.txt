# CMake build for bspsGPU (Ubuntu, macOS, etc.)
# OpenNURBS is built from external/opennurbs as a static library.

cmake_minimum_required(VERSION 3.15)
project(bspsgpu LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- OpenNURBS (zlib is resolved inside external/opennurbs/CMakeLists.txt) ---
add_subdirectory(external/opennurbs)

# --- Dependencies for the viewer / I/O ---
find_package(OpenGL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(PNG REQUIRED)

find_package(GLEW QUIET)
if(NOT GLEW_FOUND)
  find_path(GLEW_INCLUDE_DIR GL/glew.h)
  find_library(GLEW_LIBRARY NAMES GLEW glew32 glew libglew32)
  if(GLEW_INCLUDE_DIR AND GLEW_LIBRARY)
    add_library(GLEW::GLEW UNKNOWN IMPORTED)
    set_target_properties(GLEW::GLEW PROPERTIES
      IMPORTED_LOCATION "${GLEW_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${GLEW_INCLUDE_DIR}")
    set(GLEW_FOUND TRUE)
  endif()
endif()
if(NOT GLEW_FOUND)
  message(FATAL_ERROR "GLEW not found. Install libglew-dev (Debian/Ubuntu) or glew (Homebrew), or set GLEW_INCLUDE_DIR / GLEW_LIBRARY.")
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

add_executable(bspsgpu
  bspsgpu.cxx
  BSPSIO.cxx
  DM3IO.cxx
  POLIO.cxx
)

target_sources( bspsgpu PRIVATE "${_bspsgpu_embedded_h}" )
set_source_files_properties( "${_bspsgpu_embedded_h}" PROPERTIES GENERATED TRUE )

target_include_directories(bspsgpu PRIVATE
  "${_bspsgpu_generated_dir}"
  "${CMAKE_CURRENT_SOURCE_DIR}"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/render"
  "${CMAKE_CURRENT_SOURCE_DIR}/external/vecmath-cpp"
)

target_link_libraries(bspsgpu PRIVATE
  opennurbs
  OpenGL::GL
  OpenGL::GLU
  GLEW::GLEW
  PNG::PNG
  ZLIB::ZLIB
)

find_package( GLUT REQUIRED )
if( NOT TARGET GLUT::GLUT )
  add_library( GLUT::GLUT UNKNOWN IMPORTED )
  set_target_properties( GLUT::GLUT PROPERTIES
    IMPORTED_LOCATION "${GLUT_glut_LIBRARY}"
    INTERFACE_INCLUDE_DIRECTORIES "${GLUT_INCLUDE_DIR}" )
endif()
target_link_libraries( bspsgpu PRIVATE GLUT::GLUT )
if( APPLE )
  target_compile_definitions( bspsgpu PRIVATE GL_SILENCE_DEPRECATION )
endif()

if(UNIX AND NOT APPLE)
  target_link_libraries(bspsgpu PRIVATE m)
endif()

if(WIN32)
  target_link_libraries(bspsgpu PRIVATE Rpcrt4)
endif()

# Optional: copy runtime assets next to the executable when present
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/lattice.png")
  add_custom_command(TARGET bspsgpu POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/lattice.png"
      "$<TARGET_FILE_DIR:bspsgpu>/lattice.png"
    COMMENT "Copying lattice.png next to bspsgpu")
endif()

install(TARGETS bspsgpu RUNTIME DESTINATION bin)
install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/shaders/" DESTINATION share/bspsgpu/shaders)
