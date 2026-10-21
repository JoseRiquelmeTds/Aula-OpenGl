#ifndef AULA_OPENGL_CORE_INPUT_HPP
#define AULA_OPENGL_CORE_INPUT_HPP

#include <GLFW/glfw3.h>

#include <array>

// Input de uma janela GLFW. Usa o user pointer e os callbacks de teclado, botão,
// posição do cursor e foco dessa janela; não os substitua enquanto o Input existir.
// Criar depois da janela e destruir antes dela. Usar na thread do loop principal.
class Input {
public:
    struct MouseDelta {
        double x = 0.0;
        double y = 0.0;
    };

    explicit Input(GLFWwindow* window);
    ~Input();

    Input(const Input&) = delete;
    Input& operator=(const Input&) = delete;

    // Uma vez por frame, ANTES de glfwPollEvents(). Depois do poll, todos os
    // módulos consultam o mesmo estado; as consultas não consomem os eventos.
    void beginFrame();
    bool keyDown(int key) const;
    bool keyPressed(int key) const;  // GLFW_PRESS neste frame, sem GLFW_REPEAT
    bool mouseDown(int button) const;
    bool mousePressed(int button) const;

    // Soma dos movimentos neste frame, somente com cursor capturado.
    // Coordenadas GLFW: x cresce para a direita, y para baixo. Não multiplicar por dt.
    MouseDelta mouseDelta() const { return mouseDelta_; }
    void setCursorCaptured(bool captured);
    bool isCursorCaptured() const { return cursorCaptured_; }
    bool isFocused() const { return focused_; }
    bool focusChanged() const { return focusChanged_; }

private:
    static void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void onMouseButton(GLFWwindow* window, int button, int action, int mods);
    static void onCursorPosition(GLFWwindow* window, double x, double y);
    static void onFocus(GLFWwindow* window, int focused);
    void clearState();

    GLFWwindow* window_;
    std::array<bool, GLFW_KEY_LAST + 1> keysDown_{};
    std::array<bool, GLFW_KEY_LAST + 1> keysPressed_{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mouseDown_{};
    std::array<bool, GLFW_MOUSE_BUTTON_LAST + 1> mousePressed_{};
    MouseDelta mouseDelta_;
    double lastMouseX_ = 0.0;
    double lastMouseY_ = 0.0;
    bool hasMousePosition_ = false;
    bool cursorCaptured_ = false;
    bool focused_;
    bool focusChanged_ = false;
};

#endif  // AULA_OPENGL_CORE_INPUT_HPP
