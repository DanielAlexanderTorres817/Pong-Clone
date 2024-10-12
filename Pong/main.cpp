/**
* Author: Daniel Torres
* Assignment: Pong Clone
* Date due: 2024-10-12, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/


#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION
#define LOG(argument) std::cout << argument << '\n'

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"
#include "cmath"
#include <ctime>
#include <vector>
#include <string>


enum AppStatus { RUNNING, TERMINATED };
enum GameStatus { PAUSE, START };
enum OpponentStatus { PLAYER, AI };


constexpr float WINDOW_SIZE_MULT = 1.5f;

constexpr int WINDOW_WIDTH = 640 * WINDOW_SIZE_MULT,
WINDOW_HEIGHT = 480 * WINDOW_SIZE_MULT;

constexpr float BG_RED = 0.9765625f,
BG_GREEN = 0.97265625f,
BG_BLUE = 0.9609375f,
BG_OPACITY = 1.0f;

constexpr int VIEWPORT_X = 0,
VIEWPORT_Y = 0,
VIEWPORT_WIDTH = WINDOW_WIDTH,
VIEWPORT_HEIGHT = WINDOW_HEIGHT;

constexpr char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

constexpr GLint NUMBER_OF_TEXTURES = 1;
constexpr GLint LEVEL_OF_DETAIL = 0;
constexpr GLint TEXTURE_BORDER = 0;

constexpr float MILLISECONDS_IN_SECOND = 1000.0;

constexpr char P1_FILEPATH[] = "Magic_sphere.png",
P2_FILEPATH[] = "Attack_2.png",
FIREBALL_FILEPATH[] = "fireball_transparent.png",
BG_FILEPATH[] = "pixelArena.png",
PADDLE_PATH[] = "paddle_1.png",
FONT_PATH[] = "font1.png";



SDL_Window* g_display_window;

AppStatus g_app_status = RUNNING;
GameStatus game_status = PAUSE;
OpponentStatus opponent_status = PLAYER;
ShaderProgram g_shader_program = ShaderProgram();
glm::mat4 g_view_matrix, g_P1_matrix, g_projection_matrix, g_P2_matrix,
g_fireball1_matrix, g_fireball2_matrix, g_fireball3_matrix, g_BG_matrix,
g_paddle1_matrix, g_paddle2_matrix;

float g_previous_ticks = 0.0f;

GLuint g_P1_texture_id;
GLuint g_P2_texture_id;
GLuint g_fireball_texture_id;
GLuint g_BG_texture_id;
GLuint g_paddle_texture_id;
GLuint g_font_texture_id;

/*
* ------------------------------------------------------------------------------------ - ADD ANY OTHER CONSTANTS HERE--------------------------------------------------------------------

*/
constexpr int FONTBANK_SIZE = 16;


bool t_key_pressed = false;
float PLAYER_SPEED = 2.0f;
float PLAYER_HEIGHT = 1.75f;

float AI_SPEED = 3.5f;
bool IS_MOVING_UP = true;

float FIREBALL_SPEED = 1.5f;
constexpr float FIREBALL_WIDTH = 0.25f;
constexpr float FIREBALL_HEIGHT = 0.25f;

int player1_score = 0;
int player2_score = 0;

int CAP = 5;
std::string winner = "NONE";

//glm::vec3 fireball_positions[3];
//glm::vec3 fireball_movements[3];
bool fireball_active[3] = { false, false, false };


//for top and bottom screen collision detection
constexpr float WINDOW_TOP = 3.75f;
constexpr float WINDOW_BOTTOM = -3.75f;

//for box to box collision, the image is 35 x 124, the other numbers are the scaling factors I used
float scaled_paddle_width =  0.5f; 
float scaled_paddle_height = 0.75f;

glm::vec3 g_P1_position = glm::vec3(-4.5f, 0.0f, 0.0f);
glm::vec3 g_P1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_P2_position = glm::vec3(4.5f, 0.0f, 0.0f);
glm::vec3 g_P2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_paddle1_position = glm::vec3(-3.75f, -0.35f, 0.0f);
glm::vec3 g_paddle1_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_paddle2_position = glm::vec3(3.75f, -0.35f, 0.0f);
glm::vec3 g_paddle2_movement = glm::vec3(0.0f, 0.0f, 0.0f);

glm::vec3 g_fireball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 g_fireball1_movement = glm::vec3(-1.0f, 1.5f, 0.0f);


glm::vec3 g_fireball2_position = glm::vec3(0.0f, 2.0f, 0.0f);
glm::vec3 g_fireball2_movement = glm::vec3(-1.0f, -1.0f, 0.0f);

glm::vec3 g_fireball3_position = glm::vec3(0.0f, -2.0f, 0.0f);
glm::vec3 g_fireball3_movement = glm::vec3(1.0f, 0.5f, 0.0f);


glm::vec3 g_fireball_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
//adding the above movements and positions to the appropriate arrays
glm::vec3 fireball_positions[] = { g_fireball1_position , g_fireball2_position , g_fireball3_position };
glm::vec3 fireball_movements[] = { g_fireball1_movement , g_fireball2_movement  ,g_fireball3_movement };
void initialise();
void process_input();
void update();
void render();
void shutdown();

//using this function to get the specific idx of the sprite I want from the two sprite maps: 
void draw_sprite_from_texture_atlas(ShaderProgram* shaderProgram, GLuint texture_id, int index, int rows, int cols)
{
    // Step 1: Calculate the UV location of the indexed frame
    float u_coord = (float)(index % cols) / (float)cols;
    float v_coord = (float)(index / cols) / (float)rows;

    // Step 2: Calculate its UV size
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;

    // Step 3: Just as we have done before, match the texture coordinates to the vertices
    float tex_coords[] =
    {
        u_coord, v_coord + height, u_coord + width, v_coord + height, u_coord + width, v_coord,
        u_coord, v_coord + height, u_coord + width, v_coord, u_coord, v_coord
    };

    float vertices[] =
    {
        -0.5, -0.5, 0.5, -0.5,  0.5, 0.5,
        -0.5, -0.5, 0.5,  0.5, -0.5, 0.5
    };
    glBindTexture(GL_TEXTURE_2D, texture_id);

    glVertexAttribPointer(shaderProgram->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices);
    glEnableVertexAttribArray(shaderProgram->get_position_attribute());

    glVertexAttribPointer(shaderProgram->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        tex_coords);
    glEnableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(shaderProgram->get_position_attribute());
    glDisableVertexAttribArray(shaderProgram->get_tex_coordinate_attribute());
}



GLuint load_texture(const char* filepath)
{
    int width, height, number_of_components;
    unsigned char* image = stbi_load(filepath, &width, &height, &number_of_components,
        STBI_rgb_alpha);

    if (image == NULL)
    {
        LOG("Unable to load image. Make sure the path is correct.");
        assert(false);
    }

    GLuint textureID;
    glGenTextures(NUMBER_OF_TEXTURES, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, LEVEL_OF_DETAIL, GL_RGBA, width, height, TEXTURE_BORDER,
        GL_RGBA, GL_UNSIGNED_BYTE, image);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    stbi_image_free(image);

    return textureID;
}



void draw_text(ShaderProgram* program, GLuint font_texture_id, std::string text,
    float font_size, float spacing, glm::vec3 position)
{
    // Scale the size of the fontbank in the UV-plane
    // We will use this for spacing and positioning
    float width = 1.0f / FONTBANK_SIZE;
    float height = 1.0f / FONTBANK_SIZE;

    // Instead of having a single pair of arrays, we'll have a series of pairs—one for
    // each character. Don't forget to include <vector>!
    std::vector<float> vertices;
    std::vector<float> texture_coordinates;

    // For every character...
    for (int i = 0; i < text.size(); i++) {
        // 1. Get their index in the spritesheet, as well as their offset (i.e. their
        //    position relative to the whole sentence)
        int spritesheet_index = (int)text[i];  // ascii value of character
        float offset = (font_size + spacing) * i;

        // 2. Using the spritesheet index, we can calculate our U- and V-coordinates
        float u_coordinate = (float)(spritesheet_index % FONTBANK_SIZE) / FONTBANK_SIZE;
        float v_coordinate = (float)(spritesheet_index / FONTBANK_SIZE) / FONTBANK_SIZE;

        // 3. Inset the current pair in both vectors
        vertices.insert(vertices.end(), {
            offset + (-0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (0.5f * font_size), -0.5f * font_size,
            offset + (0.5f * font_size), 0.5f * font_size,
            offset + (-0.5f * font_size), -0.5f * font_size,
            });

        texture_coordinates.insert(texture_coordinates.end(), {
            u_coordinate, v_coordinate,
            u_coordinate, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate + width, v_coordinate + height,
            u_coordinate + width, v_coordinate,
            u_coordinate, v_coordinate + height,
            });
    }

    // 4. And render all of them using the pairs
    glm::mat4 model_matrix = glm::mat4(1.0f);
    model_matrix = glm::translate(model_matrix, position);

    program->set_model_matrix(model_matrix);
    glUseProgram(program->get_program_id());

    glVertexAttribPointer(program->get_position_attribute(), 2, GL_FLOAT, false, 0,
        vertices.data());
    glEnableVertexAttribArray(program->get_position_attribute());
    glVertexAttribPointer(program->get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0,
        texture_coordinates.data());
    glEnableVertexAttribArray(program->get_tex_coordinate_attribute());

    glBindTexture(GL_TEXTURE_2D, font_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));

    glDisableVertexAttribArray(program->get_position_attribute());
    glDisableVertexAttribArray(program->get_tex_coordinate_attribute());
}


void initialise()
{
    SDL_Init(SDL_INIT_VIDEO);
    g_display_window = SDL_CreateWindow("User-Input and Collisions Exercise",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_OPENGL);

    SDL_GLContext context = SDL_GL_CreateContext(g_display_window);
    SDL_GL_MakeCurrent(g_display_window, context);


    if (g_display_window == nullptr) shutdown();

#ifdef _WINDOWS
    glewInit();
#endif

    glViewport(VIEWPORT_X, VIEWPORT_Y, VIEWPORT_WIDTH, VIEWPORT_HEIGHT);

    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);

    g_P1_matrix = glm::mat4(1.0f);
    g_P2_matrix = glm::mat4(1.0f);
    g_fireball1_matrix = glm::mat4(1.0f);
    g_fireball2_matrix = glm::mat4(1.0f);
    g_fireball3_matrix = glm::mat4(1.0f);
    g_BG_matrix = glm::mat4(1.0f);
    g_paddle1_matrix = glm::mat4(1.0f);
    g_paddle2_matrix = glm::mat4(1.0f);




    //set up for the initial positions here
    g_BG_matrix = glm::scale(g_BG_matrix, glm::vec3(10.0f, 7.5f, 1.0f));

    g_view_matrix = glm::mat4(1.0f);
    g_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

    g_shader_program.set_projection_matrix(g_projection_matrix);
    g_shader_program.set_view_matrix(g_view_matrix);

    glUseProgram(g_shader_program.get_program_id());

    glClearColor(BG_RED, BG_BLUE, BG_GREEN, BG_OPACITY);

    g_P1_texture_id = load_texture(P1_FILEPATH);
    g_P2_texture_id = load_texture(P2_FILEPATH);
    g_fireball_texture_id = load_texture(FIREBALL_FILEPATH);
    g_BG_texture_id = load_texture(BG_FILEPATH);
    g_paddle_texture_id = load_texture(PADDLE_PATH);
    g_font_texture_id = load_texture(FONT_PATH);


    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void process_input()
{
    g_P1_movement = glm::vec3(0.0f);
    g_P2_movement = glm::vec3(0.0f);

    g_paddle1_movement = glm::vec3(0.0f);
    g_paddle2_movement = glm::vec3(0.0f);

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            // End game
        case SDL_QUIT:
        case SDL_WINDOWEVENT_CLOSE:
            g_app_status = TERMINATED;
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_q: g_app_status = TERMINATED; break;
            default: break;
            }

        default:
            break;
        }
    }


    const Uint8* key_state = SDL_GetKeyboardState(NULL);
    if (key_state[SDL_SCANCODE_W])
    {
        g_P1_movement.y = 1.0;
        g_paddle1_movement.y = 1.0f;
    }
    if (key_state[SDL_SCANCODE_S])
    {
        g_P1_movement.y = -1.0;
        g_paddle1_movement.y = -1.0f;
    }
    if (key_state[SDL_SCANCODE_UP] && opponent_status == PLAYER)
    {
        g_P2_movement.y = 1.0;
        g_paddle2_movement.y = 1.0f;
    }
    if (key_state[SDL_SCANCODE_DOWN] && opponent_status == PLAYER) {

        g_P2_movement.y = -1.0;
        g_paddle2_movement.y = -1.0f;
    }
    if (key_state[SDL_SCANCODE_SPACE]) {
        game_status = START;
    }
    


    if (key_state[SDL_SCANCODE_T])
    {
        if (!t_key_pressed)
        {
            if (opponent_status == PLAYER)
            {
                opponent_status = AI;
            }
            else
            {
                opponent_status = PLAYER;
            }
            t_key_pressed = true;
        }
    }
    else
    {
        t_key_pressed = false;  // Reset when 'T' is released
    }
    if (key_state[SDL_SCANCODE_1])
    {
        fireball_active[0] = true;
        fireball_active[1] = false;
        fireball_active[2] = false;

    }
    else if (key_state[SDL_SCANCODE_2])
    {
        fireball_active[0] = true;
        fireball_active[1] = true;
        fireball_active[2] = false;
    }
    else if (key_state[SDL_SCANCODE_3])
    {
        fireball_active[0] = true;
        fireball_active[1] = true;
        fireball_active[2] = true;
    }

}

void update()
{
    // --- DELTA TIME CALCULATIONS --- //
    float ticks = (float)SDL_GetTicks() / MILLISECONDS_IN_SECOND;
    float delta_time = ticks - g_previous_ticks;
    g_previous_ticks = ticks;


    // --- PLAYER 1 & PADDLE 1 MOVEMENT / TRANSFORMATIONS --- //
    g_P1_position += g_P1_movement * PLAYER_SPEED * delta_time;
    g_P1_matrix = glm::mat4(1.0f);
    g_P1_matrix = glm::translate(g_P1_matrix, g_P1_position);
    g_P1_matrix = glm::scale(g_P1_matrix, glm::vec3(1.75f, 1.75f, 1.0f));

    g_paddle1_position += g_paddle1_movement * PLAYER_SPEED * delta_time;  
    g_paddle1_matrix = glm::mat4(1.0f);
    g_paddle1_matrix = glm::translate(g_paddle1_matrix, g_paddle1_position);
    g_paddle1_matrix = glm::scale(g_paddle1_matrix, glm::vec3(0.5f, 0.75f, 1.0f));



    // --- PLAYER 2 & PADDLE 2 MOVEMENT / TRANSFORMATIONS --- //
    if (opponent_status == PLAYER) {
        g_P2_position += g_P2_movement * PLAYER_SPEED * delta_time;
        g_P2_matrix = glm::mat4(1.0f);
        g_P2_matrix = glm::translate(g_P2_matrix, g_P2_position);
        g_P2_matrix = glm::rotate(g_P2_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        g_P2_matrix = glm::scale(g_P2_matrix, glm::vec3(1.5f, 1.75f, 1.0f));

        g_paddle2_position += g_paddle2_movement * PLAYER_SPEED * delta_time;
        g_paddle2_matrix = glm::mat4(1.0f);
        g_paddle2_matrix = glm::translate(g_paddle2_matrix, g_paddle2_position);
        g_paddle2_matrix = glm::scale(g_paddle2_matrix, glm::vec3(0.5f, 0.75f, 1.0f));
    }
    if (opponent_status == AI) {
        g_P2_movement.y = 1.0;
        g_paddle2_movement.y = 1.0f;
        g_P2_position += g_P2_movement * AI_SPEED * delta_time;
        g_P2_matrix = glm::mat4(1.0f);
        g_P2_matrix = glm::translate(g_P2_matrix, g_P2_position);
        g_P2_matrix = glm::rotate(g_P2_matrix, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        g_P2_matrix = glm::scale(g_P2_matrix, glm::vec3(1.5f, 1.75f, 1.0f));

        g_paddle2_position += g_paddle2_movement * AI_SPEED * delta_time;
        g_paddle2_matrix = glm::mat4(1.0f);
        g_paddle2_matrix = glm::translate(g_paddle2_matrix, g_paddle2_position);
        g_paddle2_matrix = glm::scale(g_paddle2_matrix, glm::vec3(0.5f, 0.75f, 1.0f));
    }


    // --- FIREBALL1 MOVEMENT / TRANSFORMATIONS --- //
    g_fireball_rotation.z += 20.0f * delta_time;
    if (game_status == START) {
        g_fireball1_position += g_fireball1_movement * FIREBALL_SPEED * delta_time;
    }
    else if (game_status == PAUSE) {

        g_fireball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
    }

    g_fireball1_matrix = glm::mat4(1.0f);
    g_fireball1_matrix = glm::translate(g_fireball1_matrix, g_fireball1_position);
    g_fireball1_matrix = glm::rotate(g_fireball1_matrix, g_fireball_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
    g_fireball1_matrix = glm::scale(g_fireball1_matrix, glm::vec3(0.5f, 0.35f, 1.0f));



    // --- FIREBALL2 MOVEMENT / TRANSFORMATIONS --- //
    if (game_status == START && fireball_active[1] == true) {
        g_fireball2_position += g_fireball2_movement * (FIREBALL_SPEED * 0.9f) * delta_time;
    }
    else if (game_status == PAUSE) {

        g_fireball2_position = glm::vec3(0.0f, 2.0f, 0.0f);
    }
    if (fireball_active[1] == true) {
        g_fireball2_matrix = glm::mat4(1.0f);
        g_fireball2_matrix = glm::translate(g_fireball2_matrix, g_fireball2_position);
        g_fireball2_matrix = glm::rotate(g_fireball2_matrix, g_fireball_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        g_fireball2_matrix = glm::scale(g_fireball2_matrix, glm::vec3(0.5f, 0.35f, 1.0f));
    }

    // --- FIREBALL3 MOVEMENT / TRANSFORMATIONS --- //
    if (game_status == START && fireball_active[2] == true) {
        g_fireball3_position += g_fireball3_movement * FIREBALL_SPEED * delta_time;
    }
    else if (game_status == PAUSE) {

        g_fireball3_position = glm::vec3(0.0f, -2.0f, 0.0f);
    }
    if (fireball_active[2] == true) {
        g_fireball3_matrix = glm::mat4(1.0f);
        g_fireball3_matrix = glm::translate(g_fireball3_matrix, g_fireball3_position);
        g_fireball3_matrix = glm::rotate(g_fireball3_matrix, g_fireball_rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        g_fireball3_matrix = glm::scale(g_fireball3_matrix, glm::vec3(0.5f, 0.35f, 1.0f));
    }

    // --- COLLISION DETECTION OH GOD (TOP and BOTTOM screen for the fireballs) --- //
    if (g_fireball1_position.y + 0.25f >= WINDOW_TOP) {
        g_fireball1_movement.y *= -1.0f;
    }


    if (g_fireball1_position.y - 0.25f <= WINDOW_BOTTOM) {
        g_fireball1_movement.y *= -1.0f;

    }
    if (g_fireball2_position.y + 0.25f >= WINDOW_TOP) {
        g_fireball2_movement.y *= -1.0f;
    }


    if (g_fireball2_position.y - 0.25f <= WINDOW_BOTTOM) {
        g_fireball2_movement.y *= -1.0f;

    }
    if (g_fireball3_position.y + 0.25f >= WINDOW_TOP) {
        g_fireball3_movement.y *= -1.0f;
    }


    if (g_fireball3_position.y - 0.25f <= WINDOW_BOTTOM) {
        g_fireball3_movement.y *= -1.0f;

    }
    // --- COLLISION DETECTION FOR PADDLE 1 --- //
    float x_diff_paddle1_fb1 = fabs(g_fireball1_position.x - g_paddle1_position.x);
    float y_diff_paddle1_fb1 = fabs(g_fireball1_position.y - g_paddle1_position.y);

    float x_distance_paddle1_fb1 = x_diff_paddle1_fb1 - ((scaled_paddle_width + FIREBALL_WIDTH) / 2.0f);
    float y_distance_paddle1_fb1 = y_diff_paddle1_fb1 - ((scaled_paddle_height + FIREBALL_HEIGHT) / 2.0f);

    if (x_distance_paddle1_fb1 < 0 && y_distance_paddle1_fb1 < 0) {
        g_fireball1_movement.x *= -1.0f;
        
    }

    float x_diff_paddle1_fb2 = fabs(g_fireball2_position.x - g_paddle1_position.x);
    float y_diff_paddle1_fb2 = fabs(g_fireball2_position.y - g_paddle1_position.y);

    float x_distance_paddle1_fb2 = x_diff_paddle1_fb2 - ((scaled_paddle_width + FIREBALL_WIDTH) / 2.0f);
    float y_distance_paddle1_fb2 = y_diff_paddle1_fb2 - ((scaled_paddle_height + FIREBALL_HEIGHT) / 2.0f);

    if (x_distance_paddle1_fb2 < 0 && y_distance_paddle1_fb2 < 0) {
        g_fireball2_movement.x *= -1.0f;

    }
    float x_diff_paddle1_fb3 = fabs(g_fireball3_position.x - g_paddle1_position.x);
    float y_diff_paddle1_fb3 = fabs(g_fireball3_position.y - g_paddle1_position.y);

    float x_distance_paddle1_fb3 = x_diff_paddle1_fb3 - ((scaled_paddle_width + FIREBALL_WIDTH) / 2.0f);
    float y_distance_paddle1_fb3 = y_diff_paddle1_fb3 - ((scaled_paddle_height + FIREBALL_HEIGHT) / 2.0f);

    if (x_distance_paddle1_fb3 < 0 && y_distance_paddle1_fb3 < 0) {
        g_fireball3_movement.x *= -1.0f;
       
    }

    // --- COLLISION DETECTION FOR PADDLE 2 --- //
    float x_diff_paddle2_fb1 = fabs(g_fireball1_position.x - g_paddle2_position.x);
    float y_diff_paddle2_fb1 = fabs(g_fireball1_position.y - g_paddle2_position.y);

    float x_distance_paddle2_fb1 = x_diff_paddle2_fb1 - ((scaled_paddle_width + FIREBALL_WIDTH) / 2.0f);
    float y_distance_paddle2_fb1 = y_diff_paddle2_fb1 - ((scaled_paddle_height + FIREBALL_HEIGHT) / 2.0f);

    if (x_distance_paddle2_fb1 < 0 && y_distance_paddle2_fb1 < 0) {
        g_fireball1_movement.x *= -1.0f;
        
    }

    float x_diff_paddle2_fb2 = fabs(g_fireball2_position.x - g_paddle2_position.x);
    float y_diff_paddle2_fb2 = fabs(g_fireball2_position.y - g_paddle2_position.y);

    float x_distance_paddle2_fb2 = x_diff_paddle2_fb2 - ((scaled_paddle_width + FIREBALL_WIDTH) / 2.0f);
    float y_distance_paddle2_fb2 = y_diff_paddle2_fb2 - ((scaled_paddle_height + FIREBALL_HEIGHT) / 2.0f);

    if (x_distance_paddle2_fb2 < 0 && y_distance_paddle2_fb2 < 0) {
        g_fireball2_movement.x *= -1.0f;
        
    }

    float x_diff_paddle2_fb3 = fabs(g_fireball3_position.x - g_paddle2_position.x);
    float y_diff_paddle2_fb3 = fabs(g_fireball3_position.y - g_paddle2_position.y);

    float x_distance_paddle2_fb3 = x_diff_paddle2_fb3 - ((scaled_paddle_width + FIREBALL_WIDTH) / 2.0f);
    float y_distance_paddle2_fb3 = y_diff_paddle2_fb3 - ((scaled_paddle_height + FIREBALL_HEIGHT) / 2.0f);

    if (x_distance_paddle2_fb3 < 0 && y_distance_paddle2_fb3 < 0) {
        g_fireball3_movement.x *= -1.0f;
        
    }


    // --- STOPPING P1 FROM EXCEEDING TOP AND BOTTOM EDGES --- //
    if (g_P1_position.y  > WINDOW_TOP) {
        g_P1_position.y = WINDOW_TOP ;
    }
    if (g_P1_position.y - (PLAYER_HEIGHT / 2.0f) < WINDOW_BOTTOM) {
        g_P1_position.y = WINDOW_BOTTOM + PLAYER_HEIGHT / 2.0f;
    }

    // --- STOPPING P2 FROM EXCEEDING TOP AND BOTTOM EDGES --- //
    if (g_P2_position.y > WINDOW_TOP) {
        g_P2_position.y = WINDOW_TOP;
        AI_SPEED = -AI_SPEED;
    }
    if (g_P2_position.y - (PLAYER_HEIGHT / 2.0f) < WINDOW_BOTTOM) {
        g_P2_position.y = WINDOW_BOTTOM + PLAYER_HEIGHT / 2.0f;
        AI_SPEED = -AI_SPEED;
    }

    // --- STOPPING PADDLE 1 FROM EXCEEDING TOP AND BOTTOM EDGES --- //
    if (g_paddle1_position.y + (scaled_paddle_height /2.0f) > WINDOW_TOP) {
        g_paddle1_position.y = WINDOW_TOP - (scaled_paddle_height / 2.0f);
    }
    if (g_paddle1_position.y - (scaled_paddle_height / 2.0f) < WINDOW_BOTTOM) {
        g_paddle1_position.y = WINDOW_BOTTOM + (scaled_paddle_height / 2.0f);
    }

    // --- STOPPING PADDLE 2 FROM EXCEEDING TOP AND BOTTOM EDGES --- //
    if (g_paddle2_position.y + (scaled_paddle_height / 2.0f) > WINDOW_TOP) {
        g_paddle2_position.y = WINDOW_TOP - (scaled_paddle_height / 2.0f);
    }
    if (g_paddle2_position.y - (scaled_paddle_height / 2.0f) < WINDOW_BOTTOM) {
        g_paddle2_position.y = WINDOW_BOTTOM + (scaled_paddle_height / 2.0f);
    }

    // --- GAME SCORE AND WINNER LOGIC --- //
    //player 1
    if (g_fireball1_position.x > 5.5f) {
        g_fireball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
        player1_score++;
    }
    if (g_fireball2_position.x > 5.5f) {
        g_fireball2_position = glm::vec3(0.0f, 2.0f, 0.0f);
        player1_score++;
    }
    if (g_fireball3_position.x > 5.5f) {
        g_fireball3_position = glm::vec3(0.0f, -2.0f, 0.0f);
        player1_score++;
    }
    //player 2
    if (g_fireball1_position.x < -5.5f) {
        g_fireball1_position = glm::vec3(0.0f, 0.0f, 0.0f);
        player2_score++;
    }
    if (g_fireball2_position.x < -5.5f) {
        g_fireball2_position = glm::vec3(0.0f, 2.0f, 0.0f);
        player2_score++;
    }
    if (g_fireball3_position.x < -5.5f) {
        g_fireball3_position = glm::vec3(0.0f, -2.0f, 0.0f);
        player2_score++;
    }

    //determining the winner
    if (player1_score == CAP) { winner = "Player 1"; }
    if (player2_score == CAP) { winner = "Player 2"; }


}


void draw_object(glm::mat4& object_model_matrix, GLuint& object_texture_id)
{
    g_shader_program.set_model_matrix(object_model_matrix);
    glBindTexture(GL_TEXTURE_2D, object_texture_id);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);

    

    g_shader_program.set_model_matrix(g_BG_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_BG_texture_id, 0, 1, 1);

    g_shader_program.set_model_matrix(g_P1_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_P1_texture_id, 9, 1, 16);

    g_shader_program.set_model_matrix(g_P2_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_P2_texture_id, 6, 1, 9);

    g_shader_program.set_model_matrix(g_fireball1_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_fireball_texture_id, 0, 1, 1);

    g_shader_program.set_model_matrix(g_paddle1_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_paddle_texture_id, 0, 1, 1);

    g_shader_program.set_model_matrix(g_paddle2_matrix);
    draw_sprite_from_texture_atlas(&g_shader_program, g_paddle_texture_id, 0, 1, 1);

    if (game_status == PAUSE) {
        draw_text(&g_shader_program, g_font_texture_id, "Press space to start!", 0.4f, 0.0f,
            glm::vec3(-4.1f, 2.0f, 0.0f));
    } 
    else if (game_status == START) {
        draw_text(&g_shader_program, g_font_texture_id, std::to_string(player1_score) +"          " + std::to_string(player2_score), 0.75f, 0.0f,
            glm::vec3(-4.1f, 3.0f, 0.0f));
    }
    if (winner != "NONE") {
        draw_text(&g_shader_program, g_font_texture_id,"The winner is: " + winner, 0.35f, 0.0f,
            glm::vec3(-4.1f, 2.0f, 0.0f));
        

    }

    //rendering for the fireballs 
    if (fireball_active[1] == true) {
        g_shader_program.set_model_matrix(g_fireball2_matrix);
        draw_sprite_from_texture_atlas(&g_shader_program, g_fireball_texture_id, 0, 1, 1);
    }

    if (fireball_active[2] == true) {
        g_shader_program.set_model_matrix(g_fireball3_matrix);
        draw_sprite_from_texture_atlas(&g_shader_program, g_fireball_texture_id, 0, 1, 1);
    }


    SDL_GL_SwapWindow(g_display_window);

    
}

void shutdown() { SDL_Quit(); }


int main(int argc, char* argv[])
{
    initialise();

    while (g_app_status == RUNNING)
    {
        process_input();
        update();
        render();
        if (winner != "NONE") {
            SDL_Delay(5000);
            g_app_status = TERMINATED;
            
        }
    }

    shutdown();
    return 0;
}