#ifndef AULA_OPENGL_GFX_MESH_HPP
#define AULA_OPENGL_GFX_MESH_HPP

#include <glad/gl.h>

#include <vector>

#include "gfx/vertex.hpp"

// =============================================================================
//  Mesh: geometria na GPU (VAO + VBO + EBO). TODO objeto da cena é desenhado por aqui.
// =============================================================================
//
//  VBO (vertex buffer)   a lista de Vertex (posição, normal, UV), copiada para a GPU.
//  EBO (element buffer)  a lista de índices: cada 3 índices formam um triângulo. Um vértice
//                        compartilhado por vários triângulos é guardado uma vez só.
//  VAO (vertex array)    "lembra" qual VBO/EBO usar e o layout dos atributos. No draw basta
//                        ligar o VAO, sem reconfigurar nada.
//
//  Os triângulos são anti-horários vistos de fora (regra da mão direita: o polegar aponta
//  para o mesmo lado da normal).
//
//  Uso:
//      Mesh caixa(primitives::box({1.2f, 0.75f, 0.6f}));   // ou Mesh m(vertices, indices);
//      shader.use();
//      shader.setMat4("uModel", model);
//      caixa.draw();
//
//  Para montar um objeto de várias peças, cada uma com sua transformação, use SceneNode
//  (include/gfx/scene_node.hpp). Primitivas prontas (caixa, cilindro, plano): include/gfx/primitives.hpp.

// Geometria ainda na CPU: o que as primitivas geram e o que o construtor do Mesh recebe.
// Por ficar na CPU, dá para editar (mover vértices, ajustar UVs) antes de enviar à GPU.
struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
};

class Mesh {
public:
    // Cria o VAO/VBO/EBO e copia os dados para a GPU. Precisa de um contexto OpenGL ativo
    // (depois do gladLoadGL). Os vetores podem ser descartados depois: a GPU tem a cópia.
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    explicit Mesh(const MeshData& data);
    ~Mesh();

    // Como o Shader: uma cópia apagaria os mesmos buffers no destrutor.
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Desenha os triângulos. Antes: shader.use() e o uModel da peça definidos.
    void draw() const;

    GLsizei indexCount() const { return indexCount_; }

private:
    GLuint vao_ = 0;
    GLuint vbo_ = 0;
    GLuint ebo_ = 0;
    GLsizei indexCount_ = 0;
};

#endif  // AULA_OPENGL_GFX_MESH_HPP
