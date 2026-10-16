#include "renderer.hpp"

#include <GL/glut.h>

namespace renderer {

void init() {
    glClearColor(0.10f, 0.10f, 0.15f, 1.0f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_TRIANGLES);
    glColor3f(1.0f, 0.0f, 0.0f);
    glVertex2f(-0.6f, -0.5f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(0.6f, -0.5f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(0.0f, 0.6f);
    glEnd();

    glutSwapBuffers();
}

void reshape(int width, int height) {
    if (height == 0) {
        height = 1;
    }
    glViewport(0, 0, width, height);

    // Mantém a proporção: o lado menor da janela vai de -1 a 1
    const double aspect = static_cast<double>(width) / height;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect >= 1.0) {
        gluOrtho2D(-aspect, aspect, -1.0, 1.0);
    } else {
        gluOrtho2D(-1.0, 1.0, -1.0 / aspect, 1.0 / aspect);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

}  // namespace renderer
