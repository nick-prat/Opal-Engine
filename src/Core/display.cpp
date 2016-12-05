#include "display.hpp"

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#include <Utilities/exceptions.hpp>

Display::Display()
        : m_inputController(nullptr), m_camera(nullptr), m_projMatrix(glm::mat4(1.0f)) {
}

Display::Display(uint width, uint height)
        : m_width(width), m_height(height), m_projMatrix(glm::mat4(1.0f)) {

    m_projMatrix = glm::perspective(glm::radians(60.0f), (float) width / (float) height, 0.1f, 100.0f);
    m_inputController = std::make_unique<InputController>();
    m_camera = std::make_unique<Camera>();
    glViewport(0, 0, width, height);
}

Display::~Display() {

}

InputController* Display::GetInputController() const {
    return m_inputController.get();
}

Camera* Display::GetCamera() const {
    return m_camera.get();
}

glm::mat4 Display::GetProjectionMatrix() const {
    return m_projMatrix;
}

uint Display::GetWidth() const {
    return m_width;
}

uint Display::GetHeight() const {
    return m_height;
}

void Display::SetCursorPosition(float x, float y) const {
    glm::clamp(x, 0.0f, 1.0f);
    glm::clamp(y, 0.0f, 1.0f);
}

void Display::SetCursorVisible(bool visible) const {
    if(visible) {
    } else {
    }
}
