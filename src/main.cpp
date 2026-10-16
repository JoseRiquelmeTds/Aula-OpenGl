#include <GL/glut.h>

#include <cstdlib>

#include "renderer.hpp"

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr unsigned char kKeyEsc = 27;

void onKeyboard(unsigned char key, int /*x*/, int /*y*/) {
    if (key == kKeyEsc || key == 'q') {
        std::exit(EXIT_SUCCESS);
    }
}

}  // namespace

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(kWindowWidth, kWindowHeight);
    glutCreateWindow("Aula OpenGL");

    renderer::init();

    glutDisplayFunc(renderer::display);
    glutReshapeFunc(renderer::reshape);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop();
    return EXIT_SUCCESS;
}
