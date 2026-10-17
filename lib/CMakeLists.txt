# Bibliotecas de terceiros embutidas no repositório (versões e licenças em README.md).
# Todas viram alvos CMake que o projeto linka: glfw, glad, glm e stb_image.

# --- GLFW: janela, contexto OpenGL e input ----------------------------------
# Só a biblioteca. No Linux só o backend X11: o WSLg e os desktops Wayland abrem
# janelas X11 pelo XWayland, e assim o build não depende dos pacotes do Wayland.
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
add_subdirectory(glfw)

# Código de terceiros compila sem warnings. GLFW_INCLUDE_NONE: o GLFW não inclui
# o GL/gl.h do sistema, quem declara as funções do OpenGL é o GLAD.
target_compile_options(glfw PRIVATE -w)
target_compile_definitions(glfw INTERFACE GLFW_INCLUDE_NONE)

# --- GLAD: loader das funções do OpenGL 3.3 core (código gerado) -------------
add_library(glad STATIC glad/src/gl.c)
target_include_directories(glad PUBLIC glad/include)
target_compile_options(glad PRIVATE -w)

# --- GLM: matemática (vetores, matrizes), só headers -------------------------
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE glm)

# --- stb_image: leitura de PNG/JPG para texturas -----------------------------
add_library(stb_image STATIC stb/stb_image.c)
target_include_directories(stb_image PUBLIC stb)
target_compile_options(stb_image PRIVATE -w)
