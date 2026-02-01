#include <math.h>
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

#define FILES 8
#define RANKS 8

#define INFO(msg, ...) fprintf(stdout, "INFO: "msg, ##__VA_ARGS__)
#define ERROR(msg, ...) fprintf(stderr, "ERROR: "msg, ##__VA_ARGS__)
#define ASSERT(expr, msg, ...) do { if(!(expr)) { fprintf(stderr, "ASSERT FAILED! "msg, \
                       ##__VA_ARGS__); exit(1); } } while(0)

typedef enum {
    TEAM_WHITE,
    TEAM_BLACK
} PieceTeam;

typedef enum {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING
} PieceType;

typedef struct {
    uint32_t id;
    uint32_t width;
    uint32_t height;
} Texture;

typedef struct {
    uint32_t vao, vbo;
} Quad;

typedef struct {
    Texture tex;
    Quad quad;
    PieceTeam team;
    PieceType type;
    int file, rank;
    bool valid;
} Piece;

typedef struct {
    bool board[FILES][RANKS]; // to know if any slot on the board is occupied or not
    Piece pieces[8 * 4];
} PieceManager;

typedef struct {
    GLFWwindow* window;
    int width, height;

    uint32_t shader;

    Quad board;
    Texture boardTex;

    PieceManager manager;
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
    FILE* file;
    fopen_s(&file, path, "rt");
    ASSERT(file != null, "Can't read file! Path: %s\n", path);

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
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
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_DYNAMIC_DRAW);
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

void updateQuadVertices(Quad* quad, size_t size, float* data) {
    ASSERT(quad != null, "The quad shouldn't be null!\n");
    ASSERT(data != null, "The vertex data provided shouldn't be null!\n");

    glBindVertexArray(quad->vao);
    glBindBuffer(GL_ARRAY_BUFFER, quad->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void renderQuad(Quad* quad, Texture* tex, uint32_t shader) {
    ASSERT(quad != null, "The quad shouldn't be null!");
    ASSERT(tex != null, "The tex shouldn't be null!");
    ASSERT(shader != null, "The shader shouldn't be 0!");
 
    glActiveTexture(GL_TEXTURE0);
    bindTexture(tex);
    glUseProgram(shader);
    glUniform1i(glGetUniformLocation(shader, "u_Tex"), 0);

    glBindVertexArray(quad->vao);
    glDrawArrays(GL_TRIANGLES, 0, QUAD_VERTICES);
}

// @note Make sure the 'out' is of length QUAD_VERTICES * 5
void getPieceVertices(float* out, int file, int rank) {
    float size = 2.0f/8.0f;
    float x0 = -1.0f + (file-1) * size;
    float y0 = -1.0f + (rank-1) * size;
    float x1 = x0 + size;
    float y1 = y0 + size;

    float cpy[] = {
        x0, y1, 0.0f,     0.0f, 1.0f,
        x0, y0, 0.0f,     0.0f, 0.0f,
        x1, y0, 0.0f,     1.0f, 0.0f,
        x0, y1, 0.0f,     0.0f, 1.0f,
        x1, y0, 0.0f,     1.0f, 0.0f,
        x1, y1, 0.0f,     1.0f, 1.0f
    };

    memcpy(out, cpy, sizeof(cpy));
}

void createPiece(Piece* piece, PieceType type, PieceTeam team, int file, int rank) {
    ASSERT(piece != null, "The piece ptr provided shouldn't be null!");

    piece->valid = true;
    piece->file = file;
    piece->rank = rank;
    piece->type = type;
    piece->team = team;

    float vertices[QUAD_VERTICES * 5];
    getPieceVertices(vertices, file, rank);

    piece->quad = createQuad((float*)vertices, sizeof(vertices));

    const char* white[] = {
        "assets/textures/white_pawn.png",
        "assets/textures/white_rook.png",
        "assets/textures/white_knight.png",
        "assets/textures/white_bishop.png",
        "assets/textures/white_queen.png",
        "assets/textures/white_king.png"
    };
    const char* black[] = {
        "assets/textures/black_pawn.png",
        "assets/textures/black_rook.png",
        "assets/textures/black_knight.png",
        "assets/textures/black_bishop.png",
        "assets/textures/black_queen.png",
        "assets/textures/black_king.png"
    };
    const char* path;
    if(team == TEAM_WHITE)
        path = white[(int)type];
    else
        path = black[(int)type];

    // texture
    {
        stbi_set_flip_vertically_on_load(true);
        int w, h, ch;
        uint8_t* pixels = stbi_load(path, &w, &h, &ch, 4);
        ASSERT(pixels != null, "Failed to load the texture! Path: %s", path);
        createTexture(&piece->tex, w, h, pixels);
        free(pixels);
    }
}

void deletePiece(Piece* piece) {
    ASSERT(piece != null, "The piece ptr provided shouldn't be null!");
    
    deleteTexture(&piece->tex);
    deleteQuad(&piece->quad);
    piece->valid = false;
}

void updatePiecePosition(Piece* piece, int file, int rank) {
    ASSERT(piece != null, "The piece ptr provided shouldn't be null!");
    piece->file = file;
    piece->rank = rank;

    float vertices[QUAD_VERTICES * 5];
    getPieceVertices(vertices, file, rank);
    updateQuadVertices(&piece->quad, sizeof(vertices), vertices);
}

void renderPiece(Piece* piece, uint32_t shader) {
    ASSERT(piece != null, "The piece ptr provided shouldn't be null!");

    renderQuad(&piece->quad, &piece->tex, shader);
}

void initPieceManager(PieceManager* manager) {
    ASSERT(manager != null, "The manager ptr provided shouldn't be null!");

    int idx = 0;
    for(int i = 0; i < FILES; i++) {
        createPiece(&manager->pieces[idx++], PAWN, TEAM_WHITE, i+1, 2);
    }
    for(int i = 0; i < FILES; i++) {
        createPiece(&manager->pieces[idx++], PAWN, TEAM_BLACK, i+1, 7);
    }
    createPiece(&manager->pieces[idx++], ROOK, TEAM_WHITE, 1, 1);
    createPiece(&manager->pieces[idx++], ROOK, TEAM_WHITE, 8, 1);
    createPiece(&manager->pieces[idx++], KNIGHT, TEAM_WHITE, 2, 1);
    createPiece(&manager->pieces[idx++], KNIGHT, TEAM_WHITE, 7, 1);
    createPiece(&manager->pieces[idx++], BISHOP, TEAM_WHITE, 3, 1);
    createPiece(&manager->pieces[idx++], BISHOP, TEAM_WHITE, 6, 1);
    createPiece(&manager->pieces[idx++], QUEEN, TEAM_WHITE, 4, 1);
    createPiece(&manager->pieces[idx++], KING, TEAM_WHITE, 5, 1);

    createPiece(&manager->pieces[idx++], ROOK, TEAM_BLACK, 1, 8);
    createPiece(&manager->pieces[idx++], ROOK, TEAM_BLACK, 8, 8);
    createPiece(&manager->pieces[idx++], KNIGHT, TEAM_BLACK, 2, 8);
    createPiece(&manager->pieces[idx++], KNIGHT, TEAM_BLACK, 7, 8);
    createPiece(&manager->pieces[idx++], BISHOP, TEAM_BLACK, 3, 8);
    createPiece(&manager->pieces[idx++], BISHOP, TEAM_BLACK, 6, 8);
    createPiece(&manager->pieces[idx++], QUEEN, TEAM_BLACK, 4, 8);
    createPiece(&manager->pieces[idx++], KING, TEAM_BLACK, 5, 8);

    for(int i = 0; i < FILES; i++) {
        manager->board[i][0] = true;
        manager->board[i][1] = true;
        manager->board[i][6] = true;
        manager->board[i][7] = true;
    }
}

void deinitPieceManager(PieceManager* manager) {
    ASSERT(manager != null, "The manager ptr provided shouldn't be null!");

    for(int i = 0; i < 8 * 4; i++) {
        if(manager->pieces[i].valid) {
            deletePiece(&manager->pieces[i]);
        }
    }

    memset(manager, 0, sizeof(PieceManager));
}

void renderPieces(PieceManager* manager, uint32_t shader) {
    ASSERT(manager != null, "The manager ptr provided shouldn't be null!");

    for(int i = 0; i < 8 * 4; i++) {
        if(manager->pieces[i].valid) {
            renderPiece(&manager->pieces[i], shader);
        }
    }
}

void updatePieces(PieceManager* manager, GLFWwindow* window, int width, int height) {
    ASSERT(manager != null, "The manager ptr provided shouldn't be null!");
    ASSERT(window != null, "The window ptr provided shouldn't be null!");
    static bool isClicked = false;
    Piece* piece = null;
    int file, rank;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        isClicked = true;
    } else if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
        if(isClicked) {
            double X, Y;
            glfwGetCursorPos(window, &X, &Y);
            X /= width;
            Y /= height;
            X *= 8;
            Y *= 8;
            if(X == 8)
                X = 7;
            if(Y == 8)
                Y = 7;
            file = floorl(X) + 1;
            rank = 8 - floorl(Y);
            for(int i = 0; i < 8 * 4; i++) {
                Piece* p = &manager->pieces[i];
                if(p->valid && (p->rank == rank) && (p->file == file))
                    piece = p;
            }
        }
        isClicked = false;
    }

    if(!manager->board[file-1][rank-1] || !piece)
        return;

    INFO("Piece selected! Team: %s, Pos: (%d, %d)\n", 
         (piece->team == TEAM_WHITE) ? "White" : "Black",
         file, rank);
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
            uint8_t* data = stbi_load("assets/textures/board.png", &width, &height, &channels, 4);
            ASSERT(data != null, "Failed to load the board image! Reason by stb_image: %s\n", stbi_failure_reason());

            ctx.board = createQuad((float*)BOARD_VERTICES, sizeof(BOARD_VERTICES));
            createTexture(&ctx.boardTex, width, height, data);

            stbi_image_free(data);
        }

        initPieceManager(&ctx.manager);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwShowWindow(ctx.window);
    while(!glfwWindowShouldClose(ctx.window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        // render
        renderQuad(&ctx.board, &ctx.boardTex, ctx.shader);
        renderPieces(&ctx.manager, ctx.shader);
        // update
        updatePieces(&ctx.manager, ctx.window, ctx.width, ctx.height);
        // viewport update        
        glfwGetWindowSize(ctx.window, &ctx.width, &ctx.height);
        glViewport(0, 0, ctx.width, ctx.height);
        // inputs
        if(glfwGetKey(ctx.window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            break;
        }
        // window event polling
        glfwPollEvents();
        glfwSwapBuffers(ctx.window);
    }

    // Cleanup
    {
        deinitPieceManager(&ctx.manager);

        deleteQuad(&ctx.board);
        deleteTexture(&ctx.boardTex);

        glDeleteProgram(ctx.shader);

        glfwDestroyWindow(ctx.window);
        glfwTerminate();
    }

    return 0;
}
