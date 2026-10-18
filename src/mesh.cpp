#include "mesh.hpp"

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices)
    : indexCount_(static_cast<GLsizei>(indices.size())) {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    // Tudo o que for ligado enquanto o VAO está ligado fica gravado nele.
    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(Vertex)), vertices.data(),
                 GL_STATIC_DRAW);  // STATIC: enviado uma vez, desenhado muitas

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)), indices.data(),
                 GL_STATIC_DRAW);

    // Atributos 0, 1 e 2 (posição, normal, UV) lendo do VBO ligado acima.
    configureVertexAttributes();

    // Desliga o VAO primeiro. O EBO NÃO pode ser desligado com o VAO ainda ligado:
    // o VAO guardaria "nenhum EBO" e o draw não acharia os índices.
    glBindVertexArray(0);
}

Mesh::Mesh(const MeshData& data) : Mesh(data.vertices, data.indices) {}

Mesh::~Mesh() {
    glDeleteBuffers(1, &ebo_);
    glDeleteBuffers(1, &vbo_);
    glDeleteVertexArrays(1, &vao_);
}

void Mesh::draw() const {
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, indexCount_, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}
