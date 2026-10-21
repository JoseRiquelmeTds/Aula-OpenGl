#include "gfx/texture.hpp"

#include <stb_image.h>

#include <cstdio>
#include <memory>

namespace {

// glGetError consome os flags: todo erro lido é registrado, inclusive os preexistentes.
// Chamar entre etapas evita atribuir ao upload um erro de outra operação.
bool reportGlErrors(const std::string& path, const char* operation) {
    bool ok = true;
    for (GLenum error = glGetError(); error != GL_NO_ERROR; error = glGetError()) {
        std::fprintf(stderr, "[Texture] '%s': erro OpenGL 0x%04X %s.\n", path.c_str(),
                     static_cast<unsigned int>(error), operation);
        ok = false;
    }
    return ok;
}

}  // namespace

Texture::Texture(const std::string& path, Wrap wrap) {
    // A imagem é guardada de cima para baixo (a 1ª linha do arquivo é a de cima), mas no
    // OpenGL v = 0 é a parte de BAIXO da textura. Inverter na carga deixa v = 1 em cima.
    stbi_set_flip_vertically_on_load(true);

    // Pede sempre 4 canais (RGBA), seja a imagem RGB, cinza ou com transparência: um formato
    // só para enviar à GPU, e cada pixel ocupa 4 bytes (as linhas ficam alinhadas a 4 bytes,
    // como o OpenGL espera por padrão).
    int channelsInFile = 0;
    int width = 0;
    int height = 0;
    const std::unique_ptr<unsigned char, decltype(&stbi_image_free)> pixels(
        stbi_load(path.c_str(), &width, &height, &channelsInFile, 4), stbi_image_free);
    if (pixels == nullptr) {
        std::fprintf(stderr,
                     "[Texture] Não foi possível carregar '%s': %s\n"
                     "          Rode o programa a partir da raiz do repositório (cmake --build ... --target run).\n",
                     path.c_str(), stbi_failure_reason());
        return;
    }

    reportGlErrors(path, "preexistente ao carregamento (flag consumido, origem externa)");
    GLint maxSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxSize);
    if (!reportGlErrors(path, "ao consultar GL_MAX_TEXTURE_SIZE")) {
        return;
    }
    if (width <= 0 || height <= 0 || width > maxSize || height > maxSize) {
        std::fprintf(stderr, "[Texture] '%s': dimensões %dx%d inválidas; limite do contexto: %d por eixo.\n",
                     path.c_str(), width, height, maxSize);
        return;
    }

    // Até o fim do construtor, dimensões públicas permanecem zeradas. Toda falha após
    // glGenTextures apaga o recurso parcial; apagar a textura também desfaz seu binding.
    const auto discardTexture = [&]() {
        glDeleteTextures(1, &id_);
        id_ = 0;
        reportGlErrors(path, "ao liberar textura parcial");
    };
    glGenTextures(1, &id_);
    const bool created = reportGlErrors(path, "ao gerar textura");
    if (!created || id_ == 0) {
        if (created) {
            std::fprintf(stderr, "[Texture] '%s': OpenGL não gerou um identificador de textura.\n", path.c_str());
        }
        discardTexture();
        return;
    }
    glBindTexture(GL_TEXTURE_2D, id_);
    if (!reportGlErrors(path, "ao ligar textura")) {
        discardTexture();
        return;
    }

    const GLint wrapMode = (wrap == Wrap::Repeat) ? GL_REPEAT : GL_CLAMP_TO_EDGE;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);  // S = eixo u
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);  // T = eixo v
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (!reportGlErrors(path, "ao ligar/configurar textura")) {
        discardTexture();
        return;
    }

    // Copia os pixels para a GPU (nível 0 = imagem original) e gera os níveis reduzidos
    // (metade, um quarto... até 1x1) a partir dele.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
    if (!reportGlErrors(path, "ao enviar pixels para a GPU")) {
        discardTexture();
        return;
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    if (!reportGlErrors(path, "ao gerar mipmaps")) {
        discardTexture();
        return;
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    if (!reportGlErrors(path, "ao desligar textura")) {
        discardTexture();
        return;
    }

    // A GPU já tem a cópia; o unique_ptr libera os pixels da CPU em qualquer saída.
    width_ = width;
    height_ = height;
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
    GLint maxUnits = 0;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnits);
    if (unit < 0 || unit >= maxUnits) {
        if (!warnedInvalidUnit_) {
            std::fprintf(stderr, "[Texture] Unidade %d inválida; o contexto oferece %d unidades (a partir de 0).\n",
                         unit, maxUnits);
            warnedInvalidUnit_ = true;
        }
        return;
    }
    // Seleciona a unidade e liga a textura nela. Com várias texturas no mesmo shader
    // (ex.: cor + mapa de brilho), cada uma vai numa unidade diferente.
    glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(unit));
    glBindTexture(GL_TEXTURE_2D, id_);
}
