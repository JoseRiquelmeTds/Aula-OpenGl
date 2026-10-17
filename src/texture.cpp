#include "texture.hpp"

#include <stb_image.h>

#include <cstdio>

Texture::Texture(const std::string& path, Wrap wrap) {
    // A imagem é guardada de cima para baixo (a 1ª linha do arquivo é a de cima), mas no
    // OpenGL v = 0 é a parte de BAIXO da textura. Inverter na carga deixa v = 1 em cima.
    stbi_set_flip_vertically_on_load(true);

    // Pede sempre 4 canais (RGBA), seja a imagem RGB, cinza ou com transparência: um formato
    // só para enviar à GPU, e cada pixel ocupa 4 bytes (as linhas ficam alinhadas a 4 bytes,
    // como o OpenGL espera por padrão).
    int channelsInFile = 0;
    unsigned char* pixels = stbi_load(path.c_str(), &width_, &height_, &channelsInFile, 4);
    if (pixels == nullptr) {
        std::fprintf(stderr,
                     "[Texture] Não foi possível carregar '%s': %s\n"
                     "          Rode o programa a partir da raiz do repositório (cmake --build ... --target run).\n",
                     path.c_str(), stbi_failure_reason());
        width_ = 0;
        height_ = 0;
        return;
    }

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    const GLint wrapMode = (wrap == Wrap::Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);  // S = eixo u
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);  // T = eixo v
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Copia os pixels para a GPU (nível 0 = imagem original) e gera os níveis reduzidos
    // (metade, um quarto... até 1x1) a partir dele.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width_, height_, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    glGenerateMipmap(GL_TEXTURE_2D);

    // A GPU já tem a cópia: a imagem na memória da CPU pode ser liberada.
    stbi_image_free(pixels);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::printf("[Texture] '%s' carregada (%dx%d, %d canais no arquivo)\n", path.c_str(), width_, height_,
                channelsInFile);
}

Texture::~Texture() {
    glDeleteTextures(1, &id_);  // id 0 é ignorado pelo OpenGL
}

void Texture::bind(int unit) const {
    if (id_ == 0) {
        return;
    }
    // Seleciona a unidade e liga a textura nela. Com várias texturas no mesmo shader
    // (ex.: cor + mapa de brilho), cada uma vai numa unidade diferente.
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit));
    glBindTexture(GL_TEXTURE_2D, id_);
}
