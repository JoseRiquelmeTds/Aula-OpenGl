#include "input.hpp"

Input::Input(GLFWwindow* window)
    : window_(window), focused_(glfwGetWindowAttrib(window, GLFW_FOCUSED) == GLFW_TRUE) {
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, onKey);
    glfwSetMouseButtonCallback(window_, onMouseButton);
    glfwSetCursorPosCallback(window_, onCursorPosition);
    glfwSetWindowFocusCallback(window_, onFocus);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

Input::~Input() {
    setCursorCaptured(false);
    glfwSetKeyCallback(window_, nullptr);
    glfwSetMouseButtonCallback(window_, nullptr);
    glfwSetCursorPosCallback(window_, nullptr);
    glfwSetWindowFocusCallback(window_, nullptr);
    glfwSetWindowUserPointer(window_, nullptr);
}

void Input::beginFrame() {
    keysPressed_.fill(false);
    mousePressed_.fill(false);
    mouseDelta_ = {};
    focusChanged_ = false;
}

bool Input::keyDown(int key) const {
    return key >= 0 && key <= GLFW_KEY_LAST && keysDown_[key];
}

bool Input::keyPressed(int key) const {
    return key >= 0 && key <= GLFW_KEY_LAST && keysPressed_[key];
}

bool Input::mouseDown(int button) const {
    return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST && mouseDown_[button];
}

bool Input::mousePressed(int button) const {
    return button >= 0 && button <= GLFW_MOUSE_BUTTON_LAST && mousePressed_[button];
}

void Input::setCursorCaptured(bool captured) {
    captured = captured && focused_;
    if (captured == cursorCaptured_) {
        return;
    }
    cursorCaptured_ = captured;
    hasMousePosition_ = false;
    mouseDelta_ = {};
    glfwSetInputMode(window_, GLFW_CURSOR, captured ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

void Input::clearState() {
    keysDown_.fill(false);
    mouseDown_.fill(false);
    beginFrame();
    hasMousePosition_ = false;
}

void Input::onKey(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (!input.focused_ || key < 0 || key > GLFW_KEY_LAST) {
        return;
    }
    if (action == GLFW_PRESS) {
        input.keysDown_[key] = true;
        input.keysPressed_[key] = true;
    } else if (action == GLFW_RELEASE) {
        input.keysDown_[key] = false;
    }
}

void Input::onMouseButton(GLFWwindow* window, int button, int action, int /*mods*/) {
    auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (!input.focused_ || button < 0 || button > GLFW_MOUSE_BUTTON_LAST) {
        return;
    }
    if (action == GLFW_PRESS) {
        input.mouseDown_[button] = true;
        input.mousePressed_[button] = true;
    } else if (action == GLFW_RELEASE) {
        input.mouseDown_[button] = false;
    }
}

void Input::onCursorPosition(GLFWwindow* window, double x, double y) {
    auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
    if (!input.focused_ || !input.cursorCaptured_) {
        return;
    }
    if (input.hasMousePosition_) {
        input.mouseDelta_.x += x - input.lastMouseX_;
        input.mouseDelta_.y += y - input.lastMouseY_;
    }
    input.lastMouseX_ = x;
    input.lastMouseY_ = y;
    input.hasMousePosition_ = true;
}

void Input::onFocus(GLFWwindow* window, int focused) {
    auto& input = *static_cast<Input*>(glfwGetWindowUserPointer(window));
    input.focused_ = focused == GLFW_TRUE;
    input.clearState();
    input.focusChanged_ = true;
    // Voltar à janela deixa o cursor livre; um novo clique captura novamente.
    input.setCursorCaptured(false);
}
