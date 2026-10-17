#include "primitives.hpp"

#include <glm/gtc/constants.hpp>

#include <algorithm>
#include <cmath>

namespace primitives {

namespace {

// UV máxima de um lado que mede `meters`: 1 (estica) ou metros / textureSize (repete).
float uvExtent(float meters, float textureSize) {
    return textureSize > 0.0f ? meters / textureSize : 1.0f;
}

// Acrescenta um quadrilátero plano (2 triângulos) com os cantos em ordem anti-horária
// vistos de fora: inferior esquerdo, inferior direito, superior direito, superior esquerdo.
//
// A normal sai do produto vetorial das duas arestas que partem do canto inferior esquerdo:
// (direita) x (cima) aponta para fora da face, pela regra da mão direita.
void addQuad(MeshData& data, const glm::vec3& bottomLeft, const glm::vec3& bottomRight, const glm::vec3& topRight,
             const glm::vec3& topLeft, float textureSize) {
    const glm::vec3 right = bottomRight - bottomLeft;
    const glm::vec3 up = topLeft - bottomLeft;
    const glm::vec3 normal = glm::normalize(glm::cross(right, up));

    const float u = uvExtent(glm::length(right), textureSize);
    const float v = uvExtent(glm::length(up), textureSize);

    const GLuint base = static_cast<GLuint>(data.vertices.size());
    data.vertices.push_back({bottomLeft, normal, {0.0f, 0.0f}});
    data.vertices.push_back({bottomRight, normal, {u, 0.0f}});
    data.vertices.push_back({topRight, normal, {u, v}});
    data.vertices.push_back({topLeft, normal, {0.0f, v}});

    const GLuint quad[6] = {base, base + 1, base + 2, base + 2, base + 3, base};
    data.indices.insert(data.indices.end(), quad, quad + 6);
}

}  // namespace

MeshData box(const glm::vec3& size, float textureSize) {
    // Meias medidas em x e z (centrado), altura de 0 a h (pivô na base).
    const float x = size.x * 0.5f;
    const float h = size.y;
    const float z = size.z * 0.5f;

    MeshData data;
    // Cada face listada como se você estivesse de frente para ela, do lado de fora.
    addQuad(data, {-x, 0, z}, {x, 0, z}, {x, h, z}, {-x, h, z}, textureSize);      // +Z (frente)
    addQuad(data, {x, 0, -z}, {-x, 0, -z}, {-x, h, -z}, {x, h, -z}, textureSize);  // -Z (trás)
    addQuad(data, {x, 0, z}, {x, 0, -z}, {x, h, -z}, {x, h, z}, textureSize);      // +X (direita)
    addQuad(data, {-x, 0, -z}, {-x, 0, z}, {-x, h, z}, {-x, h, -z}, textureSize);  // -X (esquerda)
    addQuad(data, {-x, h, z}, {x, h, z}, {x, h, -z}, {-x, h, -z}, textureSize);    // +Y (topo)
    addQuad(data, {-x, 0, -z}, {x, 0, -z}, {x, 0, z}, {-x, 0, z}, textureSize);    // -Y (base)
    return data;
}

MeshData cylinder(float radius, float height, int segments, float textureSize) {
    segments = std::max(segments, 3);
    const float step = glm::two_pi<float>() / static_cast<float>(segments);
    const float uMax = uvExtent(glm::two_pi<float>() * radius, textureSize);  // volta inteira
    const float vMax = uvExtent(height, textureSize);

    MeshData data;

    // --- Lateral -------------------------------------------------------------
    // Ângulo a partir de +Z, girando para +X: visto de fora, a volta anda para a direita,
    // então u cresce para a direita como nas outras primitivas.
    // São segments + 1 colunas: a última repete a posição da primeira com u = uMax, senão a
    // costura iria de u = uMax de volta a 0 e a textura inteira se espremeria numa só face.
    for (int i = 0; i <= segments; ++i) {
        const float angle = step * static_cast<float>(i);
        // A normal da lateral é a direção do eixo até o ponto: aponta para fora, sem
        // componente Y. Vértices vizinhos compartilham normal, por isso a lateral parece lisa.
        const glm::vec3 normal(std::sin(angle), 0.0f, std::cos(angle));
        const float u = uMax * static_cast<float>(i) / static_cast<float>(segments);
        data.vertices.push_back({normal * radius, normal, {u, 0.0f}});                         // 2i: base
        data.vertices.push_back({normal * radius + glm::vec3(0, height, 0), normal, {u, vMax}});  // 2i+1: topo
    }
    for (GLuint i = 0; i < static_cast<GLuint>(segments); ++i) {
        const GLuint bottom = 2 * i;
        const GLuint top = 2 * i + 1;
        const GLuint nextBottom = 2 * (i + 1);
        const GLuint nextTop = 2 * (i + 1) + 1;
        const GLuint quad[6] = {bottom, nextBottom, nextTop, nextTop, top, bottom};
        data.indices.insert(data.indices.end(), quad, quad + 6);
    }

    // --- Tampas --------------------------------------------------------------
    // Vértices próprios (não os da lateral), porque a normal aqui é +Y/-Y, não radial.
    // UV planar: a tampa é "carimbada" vista de cima, como o plane().
    auto addCap = [&](float y, float normalY) {
        const glm::vec3 normal(0.0f, normalY, 0.0f);
        auto capUv = [&](float px, float pz) {
            if (textureSize > 0.0f) {
                return glm::vec2(px / textureSize, -pz * normalY / textureSize);
            }
            return glm::vec2(0.5f + px / (2.0f * radius), 0.5f - pz * normalY / (2.0f * radius));
        };

        const GLuint center = static_cast<GLuint>(data.vertices.size());
        data.vertices.push_back({{0.0f, y, 0.0f}, normal, capUv(0.0f, 0.0f)});
        for (int i = 0; i <= segments; ++i) {
            const float angle = step * static_cast<float>(i);
            const float px = std::sin(angle) * radius;
            const float pz = std::cos(angle) * radius;
            data.vertices.push_back({{px, y, pz}, normal, capUv(px, pz)});
        }
        // Visto de cima, o anel vai de +Z para +X: sentido anti-horário. Para a tampa de
        // baixo (vista de baixo) a ordem do triângulo é invertida.
        for (GLuint i = 0; i < static_cast<GLuint>(segments); ++i) {
            const GLuint current = center + 1 + i;
            const GLuint next = current + 1;
            if (normalY > 0.0f) {
                data.indices.insert(data.indices.end(), {center, current, next});
            } else {
                data.indices.insert(data.indices.end(), {center, next, current});
            }
        }
    };
    addCap(height, 1.0f);
    addCap(0.0f, -1.0f);

    return data;
}

MeshData plane(float width, float depth, float textureSize) {
    const float x = width * 0.5f;
    const float z = depth * 0.5f;

    MeshData data;
    // Visto de cima, com +Z vindo na sua direção: "baixo" da textura em +Z, "cima" em -Z.
    addQuad(data, {-x, 0, z}, {x, 0, z}, {x, 0, -z}, {-x, 0, -z}, textureSize);
    return data;
}

}  // namespace primitives
