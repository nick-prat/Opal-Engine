#include <GL/gl3w.h>
#include "glcore.hpp"

#include <string>

#include <Scene/scene.hpp>
#include <Utilities/exceptions.hpp>
#include <Utilities/log.hpp>

// TODO Find a way to list all available scenes (./Resources/Scenes/[These folders are scenes])

// Creates a dummy GLCore that doesn't spawn a window
GLCore::GLCore() {
    m_deleter = nullptr;
    m_display = nullptr;
    m_currentScene = nullptr;
    m_window = nullptr;
    m_luaState = nullptr;
}

GLCore::GLCore(int width, int height, std::string title) : m_currentScene(nullptr) {
    m_currentScene = nullptr;

    glfwSetErrorCallback([](int error, const char* desc) {
        Log::getErrorLog() << "ERROR: " << "(" << error << ")" << " " << desc << '\n';
    });

    if(!glfwInit()) {
        Log::error("Couldn't initialize GLFW3\n");
        exit(-1);
    }

    m_deleter = [this]() {
        glfwTerminate();
    };

    constexpr int major = 3;
    constexpr int minor = 3;

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
    if(!m_window) {
        throw GenericException("Couldn't create window\n");
    }

    m_deleter = [this]() {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    };

    glfwMakeContextCurrent(m_window);

    if(gl3wInit() == -1) {
        throw GenericException("Couldn't initialize GL3W\n");
        glfwTerminate();
        exit(-1);
    }

    if(!gl3wIsSupported(major, minor)) {
        Log::getErrorLog() << "Open GL " << major << "." << minor << " is unsupported\n";
        glfwTerminate();
        exit(-1);
    }

    glfwSetWindowUserPointer(m_window, this);

    glfwSetKeyCallback(m_window, [](GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
        GLCore* glCore = reinterpret_cast<GLCore*>(glfwGetWindowUserPointer(window));
        if(action == GLFW_PRESS) {
            glCore->inputFunc(key, true);
        } else if(action == GLFW_RELEASE) {
            glCore->inputFunc(key, false);
        }
    });

    glfwSetMouseButtonCallback(m_window, [](GLFWwindow* window, int button, int action, int /*mods*/) {
        GLCore* glCore = reinterpret_cast<GLCore*>(glfwGetWindowUserPointer(window));
        if(action == GLFW_PRESS) {
            glCore->inputFunc(button, true);
        } else if(action == GLFW_RELEASE) {
            glCore->inputFunc(button, false);
        }
    });

    glfwSetCursorPosCallback(m_window, [](GLFWwindow* window, double xpos, double ypos) {
        GLCore* glCore = reinterpret_cast<GLCore*>(glfwGetWindowUserPointer(window));
        glCore->mouseFunc(xpos, ypos);
    });

    // NOTE You can set input mode with this
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    m_display = std::make_unique<const Display>(width, height);

    // Log information about current context
    Log::getLog() << "\nInformation: \n";
    Log::getLog() << "\tGL Version: " << glGetString(GL_VERSION) << '\n';
    Log::getLog() << "\tDisplay Address: " << m_display.get() << "\n\n";

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LESS);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    Log::getLog() << Log::OUT_LOG << "GL Context created\n";
    Log::getLog() << Log::OUT_LOG_CONS;
}

GLCore::~GLCore() {
    if(m_deleter != nullptr) {
        m_deleter();
    }
}

GLCore& GLCore::operator=(GLCore&& glCore) {
    m_deleter = glCore.m_deleter;
    glCore.m_deleter = nullptr;
    m_display.reset(glCore.m_display.release());
    m_currentScene = glCore.m_currentScene;
    if(m_window != nullptr) {
        glfwDestroyWindow(m_window);
    }
    m_window = glCore.m_window;
    m_luaState = glCore.m_luaState;
    glfwSetWindowUserPointer(m_window, this);
    return *this;
}

bool GLCore::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void GLCore::setClearColor(const glm::vec4& color) {
    glClearColor(color.x, color.y, color.z, color.w);
}

void GLCore::setVsync(bool enabled) {
    if(enabled) {
        glfwSwapInterval(1);
    } else {
        glfwSwapInterval(0);
    }
}

GLFWwindow* GLCore::getWindow() const {
    return m_window;
}

const Display* GLCore::getDisplay() const {
    return m_display.get();
}

Scene* GLCore::createScene(const std::string& scenename) {
    auto timer = glfwGetTime();
    auto scene = new Scene(m_display.get(), scenename);
    Log::getLog() << "Scene creation for " << scenename << " in " << glfwGetTime() - timer << " seconds\n";
    return scene;
}

void GLCore::startScene(Scene* scene) {
    m_currentScene = scene;
    m_currentScene->start();
}

void GLCore::displayFunc() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if(m_currentScene != nullptr) {
        m_currentScene->gameLoop();
    }
    m_display->getInputController()->callKeyLambdas();
    glfwSwapBuffers(m_window);
    glfwPollEvents();
}

void GLCore::inputFunc(int key, bool state) {
    m_display->getInputController()->updateKey(key, state);
}

void GLCore::mouseFunc(double xpos, double ypos) {
    m_display->getInputController()->updateMousePosition(xpos, ypos);
}
