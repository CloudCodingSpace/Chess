#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include <stb/stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define null 0
#define QUAD_VERTICES 6

#define INFO(msg, ...) fprintf(stdout, "INFO: "msg, ##__VA_ARGS__)
#define ERROR(msg, ...) fprintf(stderr, "ERROR: "msg, ##__VA_ARGS__)
#define ASSERT(expr, msg, ...) do { if(!(expr)) { fprintf(stderr, "ASSERT FAILED! "msg, \
                       ##__VA_ARGS__); exit(1); } } while(0)

typedef struct {
    uint32_t id;
    uint32_t width;
    uint32_t height;
} Texture;

typedef struct {
    uint32_t vao, vbo;
} Quad;

typedef struct {
    GLFWwindow* window;
    int width, height;

    uint32_t shader;

    Quad board;
    Texture boardTex;
} Ctx;


static float BOARD_VERTICES[] = {
    -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f,     0.0f, 0.0f,
     1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f,     0.0f, 1.0f,
     1.0f, -1.0f, 0.0f,     1.0f, 0.0f,
     1.0f,  1.0f, 0.0f,     1.0f, 1.0f
};


char* readFile(const char* path) {
    ASSERT(path != null, "The path shouldn't be null!\n");
    FILE* file = fopen(path, "rt");
    if(!file)
        ERROR("Can't read file! Path: %s\n", path);

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* str = malloc(sizeof(char) * (size + 1));
    memset(str, 0, sizeof(char) * (size + 1));
    
    fread(str, size, 1, file);
    str[size] = '\0';

    fclose(file);
    
    return str;
}

bool createShader(uint32_t* id) {
    char log[512];
    int success = false;
    const char* vertStr = readFile("assets/shaders/default.vert");
    const char* fragStr = readFile("assets/shaders/default.frag");

    uint32_t vID, fID;
    vID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vID, 1, &vertStr, 0);
    glCompileShader(vID);
    glGetShaderiv(vID, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vID, 512, 0, log);
        fprintf(stderr, "ERROR: %s\n", log);
        return false;
    }
    fID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fID, 1, &fragStr, 0);
    glCompileShader(fID);
    glGetShaderiv(fID, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fID, 512, 0, log); 
        fprintf(stderr, "ERROR: %s\n", log);
        return false;
    }

    *id = glCreateProgram();
    glAttachShader(*id, vID);
    glAttachShader(*id, fID);
    glLinkProgram(*id);
    glValidateProgram(*id);
    glGetProgramiv(*id, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(*id, 1024, 0, log);
        fprintf(stderr, "ERROR: %s\n", log);
        return false;
    }
    glGetProgramiv(*id, GL_VALIDATE_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(*id, 1024, 0, log);
        fprintf(stderr, "ERROR: %s\n", log);
        return false;
    }

    glDeleteShader(vID);
    glDeleteShader(fID);

    free((void*)vertStr);
    free((void*)fragStr);

    return true;
}

void createTexture(Texture* texture, uint32_t width, uint32_t height, uint8_t* pixels) {
    ASSERT(texture != null, "The texture shouldn't be null!\n");

    texture->width = width;
    texture->height = height;

    glGenTextures(1, &texture->id);
    glBindTexture(GL_TEXTURE_2D, texture->id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void bindTexture(Texture* tex) {
    glBindTexture(GL_TEXTURE_2D, tex->id);
}

void deleteTexture(Texture* texture) {
    ASSERT(texture != null, "The texture shouldn't be null!");
    ASSERT(texture->id != null, "The texture handle shouldn't be 0!");

    glDeleteTextures(1, &texture->id);
    memset(texture, 0, sizeof(Texture));
}

Quad createQuad(float* data, size_t dataSize) {
    Quad q; 

    glGenVertexArrays(1, &q.vao);
    glGenBuffers(1, &q.vbo);

    glBindVertexArray(q.vao);

    glBindBuffer(GL_ARRAY_BUFFER, q.vbo);
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    return q;
}

void deleteQuad(Quad* quad) {
    ASSERT(quad != null, "The quad shouldn't be null!\n");

    glDeleteBuffers(1, &quad->vbo);
    glDeleteVertexArrays(1, &quad->vao);
}

void renderQuad(Quad* quad, Texture* tex, uint32_t shader) {
    ASSERT(quad != null, "The quad shouldn't be null!");
    ASSERT(tex != null, "The tex shouldn't be null!");
    ASSERT(shader != null, "The shader shouldn't be 0!");
 
    bindTexture(tex);
    glActiveTexture(GL_TEXTURE0);
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "u_Tex"), 0);

    glBindVertexArray(quad->vao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES);
}

int main(void) {
    Ctx ctx = {
        .width = 800,
        .height = 800
    };

    // Init 
    {
        // Window
        {
            ASSERT(glfwInit(), "Can't init glfw!\n");

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);

            ctx.window = glfwCreateWindow(ctx.width, ctx.height, "Chess", null, null);
            ASSERT(ctx.window, "Can't create window!\n");

            glfwMakeContextCurrent(ctx.window);

            ASSERT(gladLoadGLLoader((GLADloadproc)glfwGetProcAddress), "Can't init opengl!");
        }

        createShader(&ctx.shader);

        //Board 
        {
            stbi_set_flip_vertically_on_load(false);

            int width, height, channels;
            uint8_t* data = stbi_load("assets/textures/board.jpg", &width, &height, &channels, 4);
            ASSERT(data != null, "Failed to load the board image! Reason by stb_image: %s\n", stbi_failure_reason());

            ctx.board = createQuad((float*)BOARD_VERTICES, sizeof(BOARD_VERTICES));
            createTexture(&ctx.boardTex, width, height, data);

            stbi_image_free(data);
        }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwShowWindow(ctx.window);
    while(!glfwWindowShouldClose(ctx.window)) {
        glClear(GL_COLOR_BUFFER_BIT);

        renderQuad(&ctx.board, &ctx.boardTex, ctx.shader);

        glfwPollEvents();
        glfwSwapBuffers(ctx.window);

        glfwGetWindowSize(ctx.window, &ctx.width, &ctx.height);
        glViewport(0, 0, ctx.width, ctx.height);
    }

    // Cleanup
    {
        deleteQuad(&ctx.board);
        deleteTexture(&ctx.boardTex);

        glDeleteProgram(ctx.shader);

        glfwDestroyWindow(ctx.window);
        glfwTerminate();
    }

    return 0;
}
