# =============================================================================
#  Aula-OpenGl — build com CMake (C++17 + OpenGL 3.3 core)
#  Roda no Windows (MinGW-w64 via MSYS2), no Linux e no WSL2.
#  Setup do ambiente: docs/SETUP.md
# =============================================================================
#
#  Fluxo de compilação:
#    lib/{glfw,glad,stb}            -> bibliotecas estáticas (veja lib/CMakeLists.txt)
#    src/**/*.cpp (exceto main.cpp) -> libaula-opengl.a
#    src/main.cpp + libaula-opengl.a + bibliotecas de lib/ -> bin/aula-opengl[.exe]
#
#  Uso (troque linux por windows no Windows):
#    cmake --preset linux                       configura em build/linux/
#    cmake --build --preset linux               compila
#    cmake --build --preset linux --target run  compila e abre a janela

cmake_minimum_required(VERSION 3.21)
project(aula-opengl LANGUAGES C CXX)

if(MSVC)
  message(FATAL_ERROR
    "MSVC não é suportado. No Windows use o MinGW-w64 do MSYS2: cmake --preset windows (veja docs/SETUP.md)")
endif()

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Debug ou Release" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_subdirectory(lib)

# --- Fontes do projeto -------------------------------------------------------
# CONFIGURE_DEPENDS: um .cpp novo em src/ entra no build sem editar este arquivo
file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS src/*.cpp)
list(REMOVE_ITEM APP_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)

set(APP_WARNINGS -Wall -Wextra -Wpedantic)

add_library(aula-opengl-core STATIC ${APP_SOURCES})
set_target_properties(aula-opengl-core PROPERTIES OUTPUT_NAME aula-opengl)
target_include_directories(aula-opengl-core PUBLIC include)
target_compile_options(aula-opengl-core PRIVATE ${APP_WARNINGS})
target_link_libraries(aula-opengl-core PUBLIC glfw glad glm stb_image)

add_executable(aula-opengl src/main.cpp)
target_compile_options(aula-opengl PRIVATE ${APP_WARNINGS})
target_link_libraries(aula-opengl PRIVATE aula-opengl-core)

if(WIN32)
  # -static: o .exe não depende das DLLs do MinGW e abre com dois cliques
  target_link_options(aula-opengl PRIVATE -static)
endif()

# Roda a partir da raiz do repositório, para shaders/ e assets/ abrirem com caminho relativo
add_custom_target(run
  COMMAND aula-opengl
  DEPENDS aula-opengl
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  USES_TERMINAL)
