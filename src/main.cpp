#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

#include "renderer.hpp"

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;

// Versão do OpenGL do projeto: 3.3 core (a mesma do loader gerado em lib/glad)
constexpr int kGlMajor = 3;
constexpr int kGlMinor = 3;

void onGlfwError(int code, const char* description) {
    std::fprintf(stderr, "GLFW (%d): %s\n", code, description);
}

void onKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void onFramebufferSize(GLFWwindow* /*window*/, int width, int height) {
    renderer::resize(width, height);
}

}  // namespace

int main() {
    glfwSetErrorCallback(onGlfwError);
    if (!glfwInit()) {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, kGlMajor);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, kGlMinor);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    GLFWwindow* window = glfwCreateWindow(kWindowWidth, kWindowHeight, "Aula OpenGL", nullptr, nullptr);
    if (window == nullptr) {
        glfwTerminate();
        return EXIT_FAILURE;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (gladLoadGL(glfwGetProcAddress) == 0) {
        std::fprintf(stderr, "GLAD: não foi possível carregar o OpenGL %d.%d\n", kGlMajor, kGlMinor);
        glfwDestroyWindow(window);
        glfwTerminate();
        return EXIT_FAILURE;
    }
    std::printf("OpenGL %s | %s\n", reinterpret_cast<const char*>(glGetString(GL_VERSION)),
                reinterpret_cast<const char*>(glGetString(GL_RENDERER)));

    glfwSetKeyCallback(window, onKey);
    glfwSetFramebufferSizeCallback(window, onFramebufferSize);

    renderer::init();
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window, &width, &height);
    renderer::resize(width, height);

    while (!glfwWindowShouldClose(window)) {
        renderer::draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return EXIT_SUCCESS;
}
