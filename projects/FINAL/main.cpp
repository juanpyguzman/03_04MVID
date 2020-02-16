/* // ARKANOID //
    Actividad final asignatura "Programación avanzada I
    Máster en Diseño y Desarrollo de Videojuegos
    Profesor: Iván Fuertes Torrecilla

    Autor: Juan Pablo Guzmán Fernández

    Instrucciones: 
        - Pulse la barra espaciadora para comenzar. 
        - Use las flechas derecha e izquierda para mover la barra.
        - Dispone de 3 vidas.
*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <ctime>

#include "engine/camera.hpp"
#include "engine/geometry/cube.hpp"
#include "engine/input.hpp"
#include "engine/shader.hpp"
#include "engine/texture.hpp"
#include "engine/window.hpp"
#include "engine/geometry/sphere.hpp"
#include "engine/geometry/quad.hpp"
#include "engine/model.hpp"
#include "engine/light.hpp"
#include "engine/fbo.hpp"
#include "engine/material.hpp"
#include "engine/drawable.hpp"
#include <iostream>

Camera camera(glm::vec3(0.0f, -3.0f, 10.0f));

//Directional Light
DirectionalLight dirLight(
    glm::vec3(-1.0f, -0.5f, -3.0f), //Direction
    glm::vec3(1.0f, 0.5f, 3.0f),    //Position (for shadow mapping)
    glm::vec3(0.1f, 0.1f, 0.1f),    //Ambient
    glm::vec3(0.1f, 0.1f, 0.1f),    //Diffuse
    glm::vec3(0.2f, 0.2f, 0.2f)     //Specular
);

//Background
glm::vec3 backGroundSize(10.0f, 10.0f, 10.0f);

//Block positions
glm::vec3 blockPos(0.0f, 2.5f, 0.0f);
glm::vec3 blockSize(0.5f, 0.2f, 0.2f);
const uint32_t numberOfBlocks = 17;
const uint32_t numberOfRows = 4;
glm::vec3 blockPositions[4][17];
bool destroyBlock[4][17];

//PowerUps
glm::vec3 powerUpPositions[4][17];
glm::vec3 powerUpSize(0.4f, 0.15f, 0.15f);
uint16_t random;
uint16_t frequency = 10;
bool createPowerUp[4][17];
const float powerUpSpeed = 2.0f;
float powerUpTime = float(clock());
float maxPowerUpTime = 7.0f;

//Bar
glm::vec3 barPos(0.0f, -3.8f, 0.0f);
glm::vec3 barSize(1.0f, 0.2f, 0.2f);
const float k_barSpeed = 4.5f;

//Lifes
uint16_t lifes = 3;

//Ball
glm::vec3 ballPos(barPos.x, barPos.y + 0.3f, barPos.z);
float ballRadius = 0.1f;
const float ballSpeedX = 3.0f;
const float ballSpeedY = 4.0f;
glm::vec2 k_ballSpeed(ballSpeedX, ballSpeedY);

//PointLights
uint16_t numberPointLights = 2;
PointLight* pointLight = new PointLight[numberPointLights]
{  
    //PointLight
    {glm::vec3(1.0f, 0.5f, 3.0f),   //Position
    glm::vec3(0.1f, 0.1f, 0.1f),    //Ambient
    glm::vec3(0.6f, 0.6f, 0.6f),    //Diffuse
    glm::vec3(1.0f, 1.0f, 1.0f),    //Specular
    1.0f,                           //Constant
    0.0f,                           //Linear
    0.0f},                          //Quadratic

    //Light Ball
    {glm::vec3(ballPos),            //Position
    glm::vec3(0.5f, 0.5f, 0.3f),    //Ambient
    glm::vec3(0.9f, 0.9f, 0.2f),    //Diffuse
    glm::vec3(0.9f, 0.9f, 0.2f),    //Specular
    1.0f,                           //Constant
    1.7f,                           //Linear
    2.8f}                           //Quadratic
};

//Other constants
const uint32_t k_shadow_height = 1024;
const uint32_t k_shadow_width = 1024;
FBO fboShadow(k_shadow_width, k_shadow_height);
const float k_shadow_near = 1.0f;
const float k_shadow_far = 10.5f;

float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

bool startGame = false;

FBO postProcessFBO(Window::instance()->getWidth(), Window::instance()->getHeight());

void handleInput(float dt) {
    Input* input = Input::instance();

    if (input->isKeyPressed(GLFW_KEY_W)) {
        camera.handleKeyboard(Camera::Movement::Forward, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_S)) {
        camera.handleKeyboard(Camera::Movement::Backward, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_A)) {
        camera.handleKeyboard(Camera::Movement::Left, dt);
    }
    if (input->isKeyPressed(GLFW_KEY_D)) {
        camera.handleKeyboard(Camera::Movement::Right, dt);
    }
    //Controller
    if (input->isKeyPressed(GLFW_KEY_LEFT)) {
        barPos -= dt * k_barSpeed * glm::vec3(1.0f, 0.0f, 0.0f);
    }
    if (input->isKeyPressed(GLFW_KEY_RIGHT)) {
        barPos += dt * k_barSpeed * glm::vec3(1.0f, 0.0f, 0.0f);
    }
}

void onKeyPress(int key, int action) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(true);
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(false);
    }

    //Starting game
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (lifes > 0) {
            startGame = true;
        }
    }
}

void onMouseMoved(float x, float y) {
    if (firstMouse) {
        firstMouse = false;
        lastX = x;
        lastY = y;
    }

    const float xoffset = x - lastX;
    const float yoffset = lastY - y;
    lastX = x;
    lastY = y;

    camera.handleMouseMovement(xoffset, yoffset);
}

void onScrollMoved(float x, float y) {
    camera.handleMouseScroll(y);
}

void renderScene(const Shader& shader,
    const Drawable& d_background, const Drawable& d_bar, const Drawable& d_block, const Drawable& d_walls, const Drawable& d_ball, const Drawable& d_powerUp) {

    //Fondo
    d_background.setDrawable(shader, glm::vec3(0.0f, 0.0f, -0.5f), 0.0f, glm::vec3(0.0f), glm::vec3(backGroundSize));

    //Walls
    d_walls.setDrawable(shader, glm::vec3(5.25f, 0.0f, -0.5f), 0.0f, glm::vec3(0.0f),glm::vec3(0.5f, 10.0f, 0.75f));
    d_walls.setDrawable(shader, glm::vec3(-5.25f, 0.0f, -0.5f), 0.0f, glm::vec3(0.0f), glm::vec3(0.5f, 10.0f, 0.75f));
    d_walls.setDrawable(shader, glm::vec3(0.0f, 4.5f, -0.5f), glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.5f, 10.0f, 0.75f));

    //Bar
    d_bar.setDrawable(shader, glm::vec3(barPos), 0.0f, glm::vec3(0.0f), glm::vec3(barSize));

    //Blocks
    for (int j = 0; j < numberOfRows; j++) {
        for (int i = 0; i < numberOfBlocks; i++) {
            if (!destroyBlock[j][i]) {
                d_block.setDrawable(shader, glm::vec3(blockPositions[j][i]), 0.0f, glm::vec3(0.0f), glm::vec3(blockSize));
            }
            else {
                //PowerUp
                if (createPowerUp[j][i]) {
                    d_powerUp.setDrawable(shader, glm::vec3(powerUpPositions[j][i]), glm::radians(90.0f)*static_cast<float>(glfwGetTime()), glm::vec3(powerUpSpeed, 0.0f, 0.0f), glm::vec3(powerUpSize));
                }
            }
        };
    };

    //Model ball
    glm::vec3 rotation;
    if (startGame) {
        rotation = glm::vec3(-k_ballSpeed.y * 90.0f, 0.0f, 0.0f);
    }
    else {
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    }
    d_ball.setDrawable(shader, ballPos, static_cast<float>(glfwGetTime()), rotation, glm::vec3(ballRadius));
}

void render(const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_post,
    const Drawable& d_background, const Drawable& d_bar, const Drawable& d_block, const Drawable& d_walls, const Drawable& d_ball, const Drawable& d_powerUp,
    const Geometry& quadPostProcess,
    const uint32_t fbo, const uint32_t fbo_texture,
    const uint32_t fbo_post, const uint32_t fbo_post_texture) {

    //FIRST PASS: Shadow mapping
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, k_shadow_width, k_shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, k_shadow_near, k_shadow_far);
    const glm::mat4 lightView = glm::lookAt(dirLight.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    s_depth.use();
    s_depth.set("lightSpaceMatrix", lightSpaceMatrix);
    //glCullFace(GL_FRONT);
    renderScene(s_depth, d_background, d_bar, d_block, d_walls, d_ball, d_powerUp);
    //glCullFace(GL_BACK);

    if (lifes > 0) {
        //SECOND PASS: SCREEN
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.setFront(-camera.getPosition());
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

        s_phong.use();
        s_phong.set("view", view);
        s_phong.set("proj", proj);
        s_phong.set("viewPos", camera.getPosition());


        //Luz direccional
        dirLight.setShader(s_phong);

        //Lightball
        pointLight[0].setShader(s_phong, 0);
        pointLight[1].setShader(s_phong, 1);
        pointLight[1].setPosition(ballPos);

        //Shadows
        s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        s_phong.set("depthMap", 3);

        renderScene(s_phong, d_background, d_bar, d_block, d_walls, d_ball, d_powerUp);

        /*glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
        glClear(GL_COLOR_BUFFER_BIT);
        glDisable(GL_DEPTH_TEST);

        s_debug.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        s_debug.set("depthMap", 0);

        quad.render();*/
    }

    else if (lifes == 0)
    {
        //SECOND PASS: PostProcess
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_post);
        glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        camera.setFront(-camera.getPosition());
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

        s_phong.use();
        s_phong.set("view", view);
        s_phong.set("proj", proj);
        s_phong.set("viewPos", camera.getPosition());


        //Luz direccional
        dirLight.setShader(s_phong);

        //Lightball
        pointLight[0].setShader(s_phong, 0);
        pointLight[1].setShader(s_phong, 1);
        pointLight[1].setPosition(ballPos);

        //Shadows
        s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
        s_phong.set("depthMap", 3);

        renderScene(s_phong, d_background, d_bar, d_block, d_walls, d_ball, d_powerUp);

        //THIRD PASS: SCREEN
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);

        glClear(GL_COLOR_BUFFER_BIT);

        s_post.use();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fbo_post_texture);
        s_post.set("screenTexture", 0);

        quadPostProcess.render();
    }
    
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const Shader s_phong("../projects/FINAL/phong.vs", "../projects/FINAL/blinn.fs");
    const Shader s_depth("../projects/FINAL/depth.vs", "../projects/FINAL/depth.fs");
    const Shader s_debug("../projects/FINAL/debug.vs", "../projects/FINAL/debug.fs");
    const Shader s_post("../projects/FINAL/fbo.vs", "../projects/FINAL/fbo.fs");

    //Geometries
    const Quad quad(1.0f);
    const Quad quadPostProcess(2.0f);
    const Cube cube(1.0f);

    //Background
    const Texture t_background_albedo("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_basecolor.jpg", Texture::Format::RGB);
    const Texture t_background_specular("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_metallic.jpg", Texture::Format::RGB);
    const Texture t_background_normal("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_normal.jpg", Texture::Format::RGB);
    Material material_background(t_background_albedo, t_background_specular, t_background_normal, 64, s_phong);
    Drawable d_background(quad, material_background);

    //Bar and blocks
    const Texture t_block_albedo("../assets/FINAL/textures/bar/Metal_Plate_041_basecolor.jpg", Texture::Format::RGB);
    const Texture t_bar_albedo("../assets/FINAL/textures/bar/Metal_Plate_041_basecolor_bar.jpg", Texture::Format::RGB);
    const Texture t_bar_specular("../assets/FINAL/textures/bar/Metal_Plate_041_metallic.jpg", Texture::Format::RGB);
    const Texture t_bar_normal("../assets/FINAL/textures/bar/Metal_Plate_041_normal.jpg", Texture::Format::RGB);
    Material material_bar(t_bar_albedo, t_bar_specular, t_bar_normal, 128, s_phong);
    Drawable d_bar(cube, material_bar);
    Material material_block(t_block_albedo, t_bar_specular, t_bar_normal, 128, s_phong);
    Drawable d_block(cube, material_block);

    //Walls
    const Texture t_walls_albedo("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_basecolor_WALLS.jpg", Texture::Format::RGB);
    const Texture t_walls_specular("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_metallic_WALLS.jpg", Texture::Format::RGB);
    const Texture t_walls_normal("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_normal_WALLS.jpg", Texture::Format::RGB);
    Material material_walls(t_walls_albedo, t_walls_specular, t_walls_normal, 64, s_phong);
    Drawable d_walls(cube, material_walls);

    //Ball model
    const Model model_ball("../assets/FINAL/models/ball/sci_fi_ball.obj");

    const Texture t_ball_albedo("../assets/FINAL/models/ball/textures/sci_fi_ball_diffuse.png", Texture::Format::RGB);
    const Texture t_ball_specular("../assets/FINAL/models/ball/textures/sci_fi_ball_specular.png", Texture::Format::RGB);
    const Texture t_ball_normal("../assets/FINAL/models/ball/textures/sci_fi_ball_normal.png", Texture::Format::RGB);
    Material material_ball(t_ball_albedo, t_bar_specular, t_bar_normal, 32, s_phong);
    Drawable d_ball(model_ball, material_ball);

    //PowerUp
    //const Model powerUp("../assets/FINAL/models/powerUps/airCapsula.fbx");
    const Texture t_powerUp_albedo("../assets/FINAL/models/powerUps/textures/TexturesCom_Slime_512_albedo.jpg", Texture::Format::RGB);
    const Texture t_powerUp_specular("../assets/FINAL/models/powerUps/textures/TexturesCom_Slime_512_height.jpg", Texture::Format::RGB);
    const Texture t_powerUp_normal("../assets/FINAL/models/powerUps/textures/TexturesCom_Slime_512_normal.jpg", Texture::Format::RGB);
    Material material_powerUp(t_powerUp_albedo, t_powerUp_specular, t_powerUp_normal, 32, s_phong);
    Drawable d_powerUp(cube, material_powerUp);

    //FBO creation
    auto fbo = fboShadow.createShadowFBO();
    auto fbo_post = postProcessFBO.createPostProcessFBO();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    //Block positions
    for (int j = 0; j < numberOfRows; j++) {
        for (int i = 0; i < numberOfBlocks; i++) {
            blockPositions[j][i] = glm::vec3(-backGroundSize.x / 2.0f + (i + 2) * blockSize.x, blockPos.y - j * blockSize.y, 0.0f);
            powerUpPositions[j][i] = blockPositions[j][i];
            destroyBlock[j][i] = false;
            createPowerUp[j][i] = false;
        };
    };

    //Console messages
    std::cout << "Arkanoid" << std::endl;
    std::cout << "Pulse space bar to start" << std::endl;
    std::cout << lifes << " lifes remaining" << std::endl;

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        srand(time(NULL));

        handleInput(deltaTime);
        render(s_phong, s_depth, s_debug, s_post,
            d_background,
            d_bar,
            d_block,
            d_walls,
            d_ball,
            d_powerUp,
            quadPostProcess,
            fbo.first, fbo.second,
            std::get<0>(fbo_post), std::get<1>(fbo_post));

        if (!startGame && lifes>0) {
            ballPos = glm::vec3(barPos.x, barPos.y + 0.3f, barPos.z);
        }

        if (!startGame && lifes == 0) {
            std::cout << "GAME OVER" << std::endl;
        }

        if (startGame) {
            //Caída de la pelota
            if (ballPos.y < barPos.y-0.5f) {
                startGame = false;
                k_ballSpeed.y = -1.0 * k_ballSpeed.y;
                lifes--;
                std::cout << lifes << " lifes remaining" << std::endl;
                //Destruimos los PowerUp
                for (int j = 0; j < numberOfRows; j++) {
                    for (int i = 0; i < numberOfBlocks; i++) {
                        createPowerUp[j][i] = false;
                    }
                }
            }

            //Ball movement
            ballPos += glm::vec3(deltaTime * k_ballSpeed.x, deltaTime * k_ballSpeed.y, 0.0f);

            //Colissions
            //Bar collisions
            float distance_BarBall = (ballPos.y - ballRadius) - (barPos.y + barSize.y / 2.0f);
            if (distance_BarBall < 0.1f) {
                if (ballPos.x <= barPos.x + barSize.x / 2.0f 
                    && ballPos.x >= barPos.x - barSize.x / 2.0f 
                    && ballPos.y > barPos.y) {
                    k_ballSpeed.y = ballSpeedY;
                }
            }
            
            //Block collisions
            for (int j = 0; j < numberOfRows; j++) {
                for (int i = 0; i < numberOfBlocks; i++) {
                    float bottomDistance_BlockBall = glm::abs((blockPositions[j][i].y - blockSize.y / 2.0f) - (ballPos.y + ballRadius));
                    float topDistance_BlockBall = glm::abs((ballPos.y - ballRadius) - (blockPositions[j][i].y + blockSize.y / 2.0f));
                    float rightDistance_BlockBall = glm::abs((ballPos.x - ballRadius) - (blockPositions[j][i].x + blockSize.x / 2.0f));
                    float leftDistance_BlockBall = glm::abs((ballPos.x + ballRadius) - (blockPositions[j][i].x - blockSize.x / 2.0f));
                   
                    if (bottomDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                        if (ballPos.x <= blockPositions[j][i].x + blockSize.x / 2.0f && ballPos.x >= blockPositions[j][i].x - blockSize.x / 2.0f) {
                            k_ballSpeed.y = -ballSpeedY;
                            destroyBlock[j][i] = true;
                            
                            random = rand() % (frequency);
                            if (random==0) {
                                createPowerUp[j][i] = true;
                            }
                        }

                        else if (rightDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                            k_ballSpeed.x = ballSpeedX;
                            destroyBlock[j][i] = true;
                            
                            random = rand() % (frequency);
                            if (random == 0) {
                                createPowerUp[j][i] = true;
                            }
                        }

                        else if (leftDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                            k_ballSpeed.x = -ballSpeedX;
                            destroyBlock[j][i] = true;

                            random = rand() % (frequency);
                            if (random == 0) {
                                createPowerUp[j][i] = true;
                            }
                        }
                    }
                    if (topDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                        if (ballPos.x <= blockPositions[j][i].x + blockSize.x / 2.0f && ballPos.x >= blockPositions[j][i].x - blockSize.x / 2.0f) {
                            k_ballSpeed.y = ballSpeedY;
                            destroyBlock[j][i] = true;

                            random = rand() % (frequency);
                            if (random == 0) {
                                createPowerUp[j][i] = true;
                            }
                        }

                        else if (rightDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                            k_ballSpeed.x = ballSpeedX;
                            destroyBlock[j][i] = true;

                            random = rand() % (frequency);
                            if (random == 0) {
                                createPowerUp[j][i] = true;
                            }
                        }

                        else if (leftDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                            k_ballSpeed.x = -ballSpeedX;
                            destroyBlock[j][i] = true;

                            random = rand() % (frequency);
                            if (random == 0) {
                                createPowerUp[j][i] = true;
                            }
                        }
                    }
                    if (createPowerUp[j][i]) {
                        powerUpPositions[j][i] += glm::vec3(0.0f, -deltaTime * powerUpSpeed, 0.0f);
                        float distance_BarPowerUp = (powerUpPositions[j][i].y - powerUpSize.y/2.0f) - (barPos.y + barSize.y / 2.0f);
                        if (distance_BarPowerUp < 0.01f) {
                            if (powerUpPositions[j][i].x - powerUpSize.x / 2.0f <= barPos.x + barSize.x / 2.0f
                                && powerUpPositions[j][i].x + powerUpSize.x / 2.0f >= barPos.x - barSize.x / 2.0f
                                && powerUpPositions[j][i].y > barPos.y - barSize.y / 2.0f) {
                                createPowerUp[j][i]=false;
                                barSize.x = 1.5f;
                                powerUpTime = float(clock());
                            }
                        }
                    }
                };
            };

            //Up wall collisions
            if (ballPos.y >= 4.0f)
            {
                k_ballSpeed.y = -ballSpeedY;
            }

            //Right and left wall collisions
            else if (ballPos.x <= -backGroundSize.x / 2.0f + ballRadius * 2.0f)
            {
                k_ballSpeed.x = ballSpeedX;
            }
            
            else if (ballPos.x >= backGroundSize.x / 2.0f - ballRadius * 2.0f)
            {
                k_ballSpeed.x = -ballSpeedX;
            }
            //Final del efecto del PowerUp
            if ((float(clock())-powerUpTime) / CLOCKS_PER_SEC >= maxPowerUpTime) {
                barSize.x = 1.0f;
            }
        }
     
        window->frame();
    }
    
    glDeleteFramebuffers(1, &fbo.first);
    glDeleteTextures(1, &fbo.second);

    glDeleteFramebuffers(1, &std::get<0>(fbo_post));
    glDeleteTextures(1, &std::get<1>(fbo_post));
    glDeleteRenderbuffers(1, &std::get<2>(fbo_post));

    return 0;
}