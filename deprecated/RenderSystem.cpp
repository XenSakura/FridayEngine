#include "RenderSystem.h"
/**
 * @brief GLFW setup commands.
 *
 */
void RenderSystem::GLFWSetup()
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    data.window = glfwCreateWindow(WIDTH, HEIGHT, "FridayEngine", nullptr, nullptr);
}
/**
 * @brief GLFW cleanup.
 *
 */
void RenderSystem::GLFWCleanup()
{
    glfwDestroyWindow(data.window);
    glfwTerminate();
}
