#version 330 core

in vec3 vWorldPos;
in vec3 vNormal;
in vec2 vUV;

out vec4 FragColor;

// Modo de cor:
//   0 = cor sólida (uColor)
//   1 = normal como cor: cada eixo vira um canal (x -> R, y -> G, z -> B)
//   2 = UV como cor: u -> R, v -> G
//   3 = textura (uTexture amostrada pela UV)
uniform int uDebugMode;
uniform vec3 uColor;

// Textura ligada na unidade indicada por setInt("uTexture", unidade) (veja include/gfx/texture.hpp).
uniform sampler2D uTexture;

void main() {
    if (uDebugMode == 1) {
        // A normal vai de -1 a 1 e a cor de 0 a 1: n * 0.5 + 0.5 faz essa conversão.
        // Face para +X fica vermelha, +Y verde, +Z azul.
        FragColor = vec4(normalize(vNormal) * 0.5 + 0.5, 1.0);
    } else if (uDebugMode == 2) {
        FragColor = vec4(vUV, 0.0, 1.0);
    } else if (uDebugMode == 3) {
        // texture() busca a cor na posição (u, v) da imagem, já com wrap e filtro aplicados:
        // UV fora de 0..1 repete (GL_REPEAT) e entre dois texels a cor é misturada (GL_LINEAR).
        FragColor = texture(uTexture, vUV);
    } else {
        FragColor = vec4(uColor, 1.0);
    }
}
