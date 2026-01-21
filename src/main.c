#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define null 0

#define INFO(msg, ...) fprintf(stdout, "INFO: "msg, ##__VA_ARGS__)
#define ERROR(msg, ...) fprintf(stderr, "ERROR: "msg, ##__VA_ARGS__)
#define ASSERT(expr, msg, ...) do { if(!(expr)) { fprintf(stderr, "ASSERT FAILED! "msg, \
                       ##__VA_ARGS__); exit(1); } } while(0)

typedef struct {
    GLFWwindow* window;
    int width, height;
} Ctx;

int main(void) {
    Ctx ctx = {
        .width = 1260,
        .height = 720
    };

    // Init 
    {
        // Window
        {
            ASSERT(glfwInit(), "Can't init glfw!\n");

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

            ctx.window = glfwCreateWindow(ctx.width, ctx.height, "Chess", null, null);
            ASSERT(ctx.window, "Can't create window!\n");

            glfwMakeContextCurrent(ctx.window);

            ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Can't init opengl!");
        }
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    while(!glfwWindowShouldClose(ctx.window)) {
        glClear(GL_COLOR_BUFFER_BIT);


        glfwPollEvents();
        glfwSwapBuffers(ctx.window);

        glfwGetWindowSize(ctx.window, &ctx.width, &ctx.height);
        glViewport(0, 0, ctx.width, ctx.height);
    }

    // Cleanup
    {
        glfwDestroyWindow(ctx.window);
        glfwTerminate();
    }

    return 0;
}
