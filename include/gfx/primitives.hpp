#ifndef AULA_OPENGL_GFX_PRIMITIVES_HPP
#define AULA_OPENGL_GFX_PRIMITIVES_HPP

#include <glm/glm.hpp>

#include "gfx/mesh.hpp"

// =============================================================================
//  Primitivas geradas no código, base da modelagem dos objetos da cena.
// =============================================================================
//
//  Todas devolvem um MeshData (CPU) com posição, normal e UV, já no tamanho real em metros
//  e com o pivô na BASE (y = 0), centrado em x e z. Para enviar à GPU: Mesh m(primitives::box(...)).
//
//  Por que gerar no tamanho real em vez de escalar um cubo de 1 m com glm::scale?
//   - A UV acompanha o tamanho da peça (veja textureSize abaixo): a textura não estica.
//   - As normais já saem certas, sem depender da normal matrix para corrigir a escala.
//  Escala com glm::scale continua valendo para o que é animado (a tela retrátil).
//
//  textureSize: quantos metros UMA repetição da textura cobre.
//   - 0 (padrão): a textura inteira é esticada em cada face (UV de 0 a 1). Bom para imagens
//     que devem aparecer inteiras: o quadro, o slide, uma etiqueta.
//   - > 0: a UV é medida em metros / textureSize e a textura se repete (tiling). Bom para
//     materiais contínuos: madeira, metal, piso. Ex.: piso de 8 m com textureSize = 0.5
//     repete a textura 16 vezes; uma perna de mesa de 5 cm mostra só um pedaço dela, sem esticar.
namespace primitives {

// Caixa (paralelepípedo) de size.x de largura, size.y de altura e size.z de profundidade.
// 24 vértices: 4 por face, cada um com a normal da sua face, para as arestas ficarem "duras".
MeshData box(const glm::vec3& size, float textureSize = 0.0f);

// Cilindro em pé (eixo Y), com tampas. segments = quantas faces na volta: mais = mais redondo.
// A lateral usa normais radiais (suaves), as tampas usam normal +Y / -Y.
// Para um tubo deitado (travessa da cadeira), gire 90° em X ou Z na transformação da peça.
MeshData cylinder(float radius, float height, int segments = 32, float textureSize = 0.0f);

// Plano horizontal (no chão, normal +Y) de width em X por depth em Z, centrado na origem.
// v cresce para -Z (para longe de quem olha de +Z). Para deixá-lo em pé virado para +Z
// (quadro, parede): glm::rotate(model, glm::radians(90.0f), {1, 0, 0}). A borda de v = 1
// vai para cima e o plano fica centrado na origem (de y = -depth/2 a +depth/2).
MeshData plane(float width, float depth, float textureSize = 0.0f);

}  // namespace primitives

#endif  // AULA_OPENGL_GFX_PRIMITIVES_HPP
