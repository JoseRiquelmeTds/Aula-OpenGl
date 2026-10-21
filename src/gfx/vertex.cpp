#include "gfx/vertex.hpp"

#include <glad/gl.h>

#include <cstddef>

void configureVertexAttributes() {
    // stride: distância em bytes do início de um vértice até o próximo.
    // offsetof: onde cada campo começa dentro da struct (0, 12 e 24 bytes).
    const GLsizei stride = sizeof(Vertex);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<const void*>(offsetof(Vertex, position)));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<const void*>(offsetof(Vertex, normal)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride,
                          reinterpret_cast<const void*>(offsetof(Vertex, uv)));
}
