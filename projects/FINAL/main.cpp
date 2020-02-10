#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

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
#include <iostream>

Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));

//Directional Light
DirectionalLight dirLight(
    glm::vec3(-1.0f, -0.5f, -3.0f), //Direction
    glm::vec3(1.0f, 0.5f, 3.0f),    //Position (for shadow mapping)
    glm::vec3(0.2f, 0.2f, 0.2f),    //Ambient
    glm::vec3(0.8f, 0.8f, 0.8f),    //Diffuse
    glm::vec3(1.0f, 1.0f, 1.0f)     //Specular
);

glm::vec3 lightPos(1.0f, 0.5f, 3.0f);

//Background
glm::vec3 backGroundSize(10.0f, 10.0f, 10.0f);

//Block positions
glm::vec3 blockPos(0.0f, 2.5f, 0.0f);
glm::vec3 blockSize(0.5f, 0.2f, 0.2f);
const uint32_t numberOfBlocks = 17;
const uint32_t numberOfRows = 4;
glm::vec3 blockPositions[4][17];
bool destroyBlock[4][17];

//Bar
glm::vec3 barPos(0.0f, -3.8f, 0.0f);
glm::vec3 barSize(1.0f, 0.2f, 0.2f);
//const float k_barSpeed = 2.5f;
const float k_barSpeed = 4.5f;

//Ball
glm::vec3 ballPos(barPos.x, barPos.y + 0.3f, barPos.z);
float ballRadius = 0.1f;
glm::vec2 k_ballSpeed(3.0f, -4.0f);

//Other constants
const uint32_t k_shadow_height = 1024;
const uint32_t k_shadow_width = 1024;
const float k_shadow_near = 1.0f;
const float k_shadow_far = 10.5f;

float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

bool startGame = false;

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

    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        startGame = true;
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

std::pair<uint32_t, uint32_t> createFBO() {
    uint32_t fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    uint32_t depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, k_shadow_width, k_shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[]{ 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer Incomplete" << std::endl;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return std::make_pair(fbo, depthMap);
}

void renderScene(const Shader& shader, const Geometry& quad, const Geometry& cube, const Geometry& sphere, const Model& ball,
    const Texture& t_albedo, const Texture& t_specular, const Texture& t_normal,
    const Texture& t_bar_albedo, const Texture& t_bar_specular, const Texture& t_bar_normal,
    const Texture& t_ball_albedo, const Texture& t_ball_specular, const Texture& t_ball_normal) {

    bool isModel;

    //Fondo
    isModel = false;
    t_albedo.use(shader, "material.diffuse", 0);
    t_specular.use(shader, "material.specular", 1);
    t_normal.use(shader, "material.normal", 2);
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.5f));
    model = glm::scale(model, glm::vec3(backGroundSize));
    shader.set("model", model);
    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    shader.set("isModel", isModel);
    quad.render();

    //Bar
    t_bar_albedo.use(shader, "material.diffuse", 0);
    t_bar_specular.use(shader, "material.specular", 1);
    t_bar_normal.use(shader, "material.normal", 2);
    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(barPos));
    model = glm::scale(model, glm::vec3(barSize));
    shader.set("model", model);
    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    shader.set("isModel", isModel);
    cube.render();

    //Blocks
    for (int j = 0; j < numberOfRows; j++) {
        for (int i = 0; i < numberOfBlocks; i++) {
            if (!destroyBlock[j][i]) {
                t_bar_albedo.use(shader, "material.diffuse", 0);
                t_bar_specular.use(shader, "material.specular", 1);
                t_bar_normal.use(shader, "material.normal", 2);
                model = glm::mat4(1.0);
                model = glm::translate(model, glm::vec3(blockPositions[j][i]));
                model = glm::scale(model, glm::vec3(blockSize));
                shader.set("model", model);
                normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
                shader.set("normalMat", normalMat);
                shader.set("isModel", isModel);
                cube.render();
            }
        };
    };

    //Model ball
    isModel = true;
    t_ball_albedo.use(shader, "material.diffuse", 0);
    t_ball_specular.use(shader, "material.specular", 1);
    t_ball_normal.use(shader, "material.normal", 2);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(ballPos));
    if (startGame) {
        model = glm::rotate(model, static_cast<float>(glfwGetTime()), glm::vec3(-k_ballSpeed.y * 90.0f, 0.0f, 0.0f));
    }
    model = glm::scale(model, glm::vec3(ballRadius));
    shader.set("model", model);

    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader.set("normalMat", normalMat);
    shader.set("material.shininess", 32);
    shader.set("isModel", isModel);
    ball.render(shader);
}

void render(const Geometry& quad, const Geometry& cube, const Geometry& sphere, const Model& ball,
    const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_light,
    const Texture& t_albedo, const Texture& t_specular, const Texture& t_normal,
    const Texture& t_bar_albedo, const Texture& t_bar_specular, const Texture& t_bar_normal,
    const Texture& t_ball_albedo, const Texture& t_ball_specular, const Texture& t_ball_normal,
    const uint32_t fbo, const uint32_t fbo_texture) {

    //FIRST PASS
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, k_shadow_width, k_shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const glm::mat4 lightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, k_shadow_near, k_shadow_far);
    const glm::mat4 lightView = glm::lookAt(dirLight.getPosition(), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 lightSpaceMatrix = lightProjection * lightView;

    s_depth.use();
    s_depth.set("lightSpaceMatrix", lightSpaceMatrix);
    //glCullFace(GL_FRONT);
    renderScene(s_depth, quad, cube, sphere, ball, t_albedo, t_specular, t_normal, t_bar_albedo, t_bar_specular, t_bar_normal, t_ball_albedo, t_ball_specular, t_ball_normal);
    //glCullFace(GL_BACK);

//SECOND PASS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);

    s_phong.use();

    s_phong.set("view", view);
    s_phong.set("proj", proj);

    s_phong.set("viewPos", camera.getPosition());


    //Luz direccional
    dirLight.setShader(s_phong);

    s_phong.set("material.shininess", 64);

    s_phong.set("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    s_phong.set("depthMap", 3);

    renderScene(s_phong, quad, cube, sphere, ball, t_albedo, t_specular, t_normal, t_bar_albedo, t_bar_specular, t_bar_normal, t_ball_albedo, t_ball_specular, t_ball_normal);

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

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    const Shader s_phong("../projects/FINAL/phong.vs", "../projects/FINAL/blinn.fs");
    const Shader s_depth("../projects/FINAL/depth.vs", "../projects/FINAL/depth.fs");
    const Shader s_debug("../projects/FINAL/debug.vs", "../projects/FINAL/debug.fs");
    const Shader s_light("../projects/FINAL/light.vs", "../projects/FINAL/light.fs");
    
    
    /*
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Texture t_normal("../assets/textures/bricks_normal.png", Texture::Format::RGB);
    */

    const Texture t_albedo("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_basecolor.jpg", Texture::Format::RGB);
    const Texture t_specular("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_metallic.jpg", Texture::Format::RGB);
    const Texture t_normal("../assets/FINAL/textures/bar/Sci-Fi_Wall_002_normal.jpg", Texture::Format::RGB);

    //Bar textures
    const Texture t_bar_albedo("../assets/FINAL/textures/bar/Metal_Plate_041_basecolor.jpg", Texture::Format::RGB);
    const Texture t_bar_specular("../assets/FINAL/textures/bar/Metal_Plate_041_metallic.jpg", Texture::Format::RGB);
    const Texture t_bar_normal("../assets/FINAL/textures/bar/Metal_Plate_041_normal.jpg", Texture::Format::RGB);
    
    //Ball model
    const Model ball("../assets/FINAL/models/ball/sci_fi_ball.obj");

    const Texture t_ball_albedo("../assets/FINAL/models/ball/textures/sci_fi_ball_diffuse.png", Texture::Format::RGB);
    const Texture t_ball_specular("../assets/FINAL/models/ball/textures/sci_fi_ball_specular.png", Texture::Format::RGB);
    const Texture t_ball_normal("../assets/FINAL/models/ball/textures/sci_fi_ball_normal.png", Texture::Format::RGB);

    const Quad quad(1.0f);
    const Cube cube(1.0f);
    const Cube sphere(1.0f);

    auto fbo = createFBO();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    //Block positions
    for (int j = 0; j < numberOfRows; j++) {
        for (int i = 0; i < numberOfBlocks; i++) {
            blockPositions[j][i] = glm::vec3(-backGroundSize.x / 2.0f + (i + 2) * blockSize.x, blockPos.y - j * blockSize.y, 0.0f);
            destroyBlock[j][i] = false;
        };
    };


    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, cube, sphere, ball, s_phong, s_depth, s_debug, s_light,
            t_albedo, t_specular, t_normal,
            t_bar_albedo, t_bar_specular, t_bar_normal,
            t_ball_albedo, t_ball_specular, t_ball_normal,
            fbo.first, fbo.second);

        if (startGame) {

            //Ball movement
            ballPos += glm::vec3(deltaTime * k_ballSpeed.x, deltaTime * k_ballSpeed.y, 0.0f);

            //Colissions
            //Bar collisions
            float distance_BarBall = (ballPos.y - ballRadius) - (barPos.y + barSize.y / 2.0f);
            if (distance_BarBall < 0.1f) {
                if (ballPos.x <= barPos.x + barSize.x / 2.0f 
                    && ballPos.x >= barPos.x - barSize.x / 2.0f 
                    && ballPos.y > barPos.y) {
                    k_ballSpeed.y = 4.0f;
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
                            k_ballSpeed.y = -4.0f;
                            destroyBlock[j][i] = true;
                        }

                        else if (rightDistance_BlockBall < 0.05f) {
                            k_ballSpeed.x = 3.0f;
                            destroyBlock[j][i] = true;
                        }

                        else if (leftDistance_BlockBall < 0.05f) {
                            k_ballSpeed.x = -3.0f;
                            destroyBlock[j][i] = true;
                        }
                    }
                    if (topDistance_BlockBall < 0.05f && destroyBlock[j][i] != true) {
                        if (ballPos.x <= blockPositions[j][i].x + blockSize.x / 2.0f && ballPos.x >= blockPositions[j][i].x - blockSize.x / 2.0f) {
                            k_ballSpeed.y = 4.0f;
                            destroyBlock[j][i] = true;
                        }

                        else if (rightDistance_BlockBall < 0.05f) {
                            k_ballSpeed.x = 3.0f;
                            destroyBlock[j][i] = true;
                        }

                        else if (leftDistance_BlockBall < 0.05f) {
                            k_ballSpeed.x = -3.0f;
                            destroyBlock[j][i] = true;
                        }
                    }
                };
            };

            //Up wall collisions
            if (ballPos.y >= 4.0f)
            {
                k_ballSpeed.y = -4.0f;
            }

            //Right and left wall collisions
            else if (ballPos.x <= -backGroundSize.x / 2.0f + ballRadius * 2.0f)
            {
                k_ballSpeed.x = 3.0f;
            }
            
            else if (ballPos.x >= backGroundSize.x / 2.0f - ballRadius * 2.0f)
            {
                k_ballSpeed.x = -3.0f;
            }
        }
     

        window->frame();
    }
    
    glDeleteFramebuffers(1, &fbo.first);
    glDeleteTextures(1, &fbo.second);

    return 0;
}