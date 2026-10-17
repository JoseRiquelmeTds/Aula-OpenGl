#ifndef AULA_OPENGL_VERTEX_HPP
#define AULA_OPENGL_VERTEX_HPP

#include <glm/glm.hpp>

// =============================================================================
//  Formato de vértice do projeto: TODO mesh da cena usa esta struct.
// =============================================================================
//
//  Campo      Tipo        location no shader   Para quê
//  ---------  ----------  -------------------  ------------------------------------
//  position   glm::vec3   0  (aPosition)       onde o vértice está, em metros
//  normal     glm::vec3   1  (aNormal)         direção "para fora" da superfície (iluminação)
//  uv         glm::vec2   2  (aUV)             coordenada de textura, de (0,0) a (1,1)
//
//  Regras para quem modela (convenções do CLAUDE.md):
//   - 1 unidade = 1 metro, eixo Y para cima.
//   - Coordenadas no espaço do PRÓPRIO objeto, com o pivô na origem: na base do objeto
//     por padrão, ou no ponto da transformação (centro das hélices, topo da tela).
//   - A normal tem comprimento 1 e aponta para fora da face. Em faces planas (cubo, tampo),
//     cada canto da face tem seu próprio vértice com a normal daquela face. Por isso um
//     cubo tem 24 vértices (4 por face), não 8.
//   - UV: u cresce para a direita da textura e v para cima, com (0,0) no canto inferior
//     esquerdo (o carregador de textura vira a imagem com stbi_set_flip_vertically_on_load).
//     Valores acima de 1 repetem a textura (tiling), útil em peças esticadas.
//
//  Exemplo (uma face voltada para +Z, quadrado de 1 m centrado em x e com base em y = 0):
//
//      Vertex v0{{-0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}};  // inferior esquerdo
//      Vertex v1{{ 0.5f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}};  // inferior direito
//      Vertex v2{{ 0.5f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}};  // superior direito
//      Vertex v3{{-0.5f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}};  // superior esquerdo
//      // triângulos (sentido anti-horário visto de fora): 0,1,2 e 2,3,0
struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};

// Os vértices ficam lado a lado no VBO, 8 floats cada (3 + 3 + 2). Se alguém mudar a
// struct sem atualizar configureVertexAttributes(), o build quebra aqui em vez de a
// geometria sair embaralhada na tela.
static_assert(sizeof(Vertex) == 8 * sizeof(float), "Vertex precisa ter exatamente 8 floats, sem padding");

// Liga os atributos 0 (posição), 1 (normal) e 2 (UV) ao layout da struct Vertex no VBO
// atualmente ligado em GL_ARRAY_BUFFER. Chamar com o VAO do mesh ligado: o VAO guarda
// essa configuração e não é preciso repetir no draw.
void configureVertexAttributes();

#endif  // AULA_OPENGL_VERTEX_HPP
