#include <Models/line.hpp>

#include <vector>
#include <string>

#include <Core/glapi.hpp>
#include <Utilities/utilities.hpp>

using namespace gl;

Line::Line(std::shared_ptr<GlutDisplay> display, glm::vec3 tail, glm::vec3 head, glm::vec3 color)
        : m_indexCount(2), m_VAO(0), m_VBO(0), m_tail(tail), m_head(head), m_color(color), m_shader(nullptr), m_display(display) {

    if(display == nullptr) {
        throw new Utilities::Exception(1, "Null parameter passed to Line constructor");
    }

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    std::vector<glm::vec3> verts = {tail, head};
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * verts.size(), verts.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);

    std::vector<uint> indices = {0, 1};
    glGenBuffers(1, &m_IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices.size(), indices.data(), GL_STATIC_DRAW);
    m_indexCount = indices.size();

    std::vector<std::string> shaderFiles = {"Shaders/line.vs", "Shaders/line.fs"};
    std::vector<GLenum> shaderTypes = {GL_VERTEX_SHADER, GL_FRAGMENT_SHADER};
    m_shader = std::make_unique<Shader>();

    if(!m_shader->InitShader(shaderFiles, shaderTypes)) {
        throw new Utilities::Exception(1, "Line creation failed, couldn't load shader");
    }
}

Line::~Line() {
    glDeleteVertexArrays(1, &m_VAO);
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_IBO);
}

void Line::Render() {
    m_shader->UseShader();
}
