#include "renderer.hpp"

#include <glad/gl.h>

namespace renderer {

void init() {
    glClearColor(0.10f, 0.10f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}

void draw() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
}

}  // namespace renderer
