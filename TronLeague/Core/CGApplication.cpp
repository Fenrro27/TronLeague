#include "CGApplication.h"
#include <iostream>

//
// FUNCIÓN: CGApplication::run()
//
// PROPÓSITO: Ejecuta la aplicación
//
void CGApplication::run()
{
    initWindow();
    initOpenGL();
    initModel();
    mainLoop();
    cleanup();
}

//
// FUNCIÓN: CGApplication::initWindow()
//
// PROPÓSITO: Inicializa la ventana con GLFW
//
void CGApplication::initWindow()
{
    if (!glfwInit())
    {
        std::cerr << "Error: No se pudo inicializar GLFW" << std::endl;
        return;
    }

    // Configuración del contexto OpenGL (Core Profile 4.0 con compatibilidad)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    windowWidth = 800;
    windowHeight = 600;
    fullScreen = false;

    window = glfwCreateWindow(windowWidth, windowHeight, "TronLeague (OpenGL)", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Error: No se pudo crear la ventana GLFW" << std::endl;
        glfwTerminate();
        return;
    }

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwMakeContextCurrent(window);

    // Configurar el cursor y capturar el ratón dentro de la ventana
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

//
// FUNCIÓN: CGApplication::initOpenGL()
//
// PROPÓSITO: Inicializa el entorno gráfico con GLEW
//
void CGApplication::initOpenGL()
{
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        std::cerr << "Error al inicializar GLEW: " << glewGetErrorString(err) << std::endl;
    }
}

//
// FUNCIÓN: CGApplication::initModel()
//
// PROPÓSITO: Inicializa el modelo
//
void CGApplication::initModel()
{
    limitFPS = 1.0 / 60.0;
    lastTime = glfwGetTime();
    deltaTime = 0;

    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    model.initialize(width, height);
}

//
// FUNCIÓN: CGApplication::mainLoop()
//
// PROPÓSITO: Bucle principal que procesa los eventos de la aplicación
//
void CGApplication::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        timing();
        glfwSwapBuffers(window);
    }
}

//
// FUNCIÓN: CGApplication::timing()
//
// PROPÓSITO: Control de tasa de cuadros y renderizado
//
void CGApplication::timing()
{
    double nowTime = glfwGetTime();
    deltaTime += (nowTime - lastTime) / limitFPS;
    lastTime = nowTime;

    while (deltaTime >= 1.0)
    {
        model.update();
        deltaTime--;
    }
    model.render();
}

//
// FUNCIÓN: CGApplication::cleanup()
//
// PROPÓSITO: Libera los recursos y finaliza la aplicación
//
void CGApplication::cleanup()
{
    model.finalize();
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

//
// FUNCIÓN: CGApplication::swapFullScreen()
//
// PROPÓSITO: Alterna la ventana entre pantalla completa o ventana normal
//
void CGApplication::swapFullScreen()
{
    if (!fullScreen)
    {
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        glfwGetWindowPos(window, &windowXpos, &windowYpos);
        fullScreen = true;
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    else
    {
        fullScreen = false;
        glfwSetWindowMonitor(window, nullptr, windowXpos, windowYpos, windowWidth, windowHeight, 0);
    }
}

//
// FUNCIÓN: CGApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
//
// PROPÓSITO: Respuesta a un evento de teclado sobre la aplicación
//
void CGApplication::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    CGApplication* app = (CGApplication*)glfwGetWindowUserPointer(window);
    if (app && (action == GLFW_PRESS || action == GLFW_REPEAT))
    {
        if (key == GLFW_KEY_F12) app->swapFullScreen();
        else app->model.key_pressed(key);
    }
}

//
// FUNCIÓN: CGApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
//
// PROPÓSITO: Respuesta a un evento de ratón sobre la aplicación
//
void CGApplication::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    CGApplication* app = (CGApplication*)glfwGetWindowUserPointer(window);
    if (app) app->model.mouse_button(button, action);
}

//
// FUNCIÓN: CGApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
//
// PROPÓSITO: Respuesta a un evento de movimiento del cursor sobre la aplicación
//
void CGApplication::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos)
{
    CGApplication* app = (CGApplication*)glfwGetWindowUserPointer(window);
    if (app) app->model.mouse_move(xpos, ypos);
}

//
// FUNCIÓN: CGApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
//
// PROPÓSITO: Respuesta a un evento de redimensionamiento de la ventana principal
//
void CGApplication::framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
    CGApplication* app = (CGApplication*)glfwGetWindowUserPointer(window);
    if (app && height != 0) app->model.resize(width, height);
}
