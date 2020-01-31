/*
EJ14.02 - Usando la misma escena, implementar un sistema con dos luces, donde ambas generan sombras
(Pintar las fuentes de luces con sendas esferas blancas para localizarlas en la escena 3D).
*/

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
#include <iostream>


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
glm::vec3 light1Pos(0.5f, 4.0f, 1.0f);
glm::vec3 light1Dir(0.0f, -1.0f, 0.0f);
glm::vec3 light2Pos(-0.5f, 5.0f, -1.5f);
glm::vec3 light2Dir(0.0f, -1.0f, 0.0f);

const uint32_t k_shadow_height = 1024;
const uint32_t k_shadow_width = 1024;
const float k_shadow_near = 1.0f;
const float k_shadow_far = 7.5f;

float lastFrame = 0.0f;
float lastX, lastY;
bool firstMouse = true;

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
}

void onKeyPress(int key, int action) {
    if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(true);
    }

    if (key == GLFW_KEY_E && action == GLFW_PRESS) {
        Window::instance()->setCaptureMode(false);
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

void renderScene(const Shader& shader1, const Geometry& quad, const Geometry& cube, const Geometry& sphere,
    const Texture& t_albedo, const Texture& t_specular,
    glm::mat4 view, glm::mat4 proj) {
    t_albedo.use(shader1, "material.diffuse", 0);
    t_specular.use(shader1, "material.specular", 1);

    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    shader1.set("model", model);
    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader1.set("normalMat", normalMat);
    quad.render();

    model = glm::mat4(1.0);
    shader1.set("model", model);
    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader1.set("normalMat", normalMat);
    cube.render();

    model = glm::mat4(1.0);
    model = glm::translate(model, glm::vec3(0.6f, 2.0f, 0.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    shader1.set("model", model);
    normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    shader1.set("normalMat", normalMat);
    sphere.render();

}

void render(const Geometry& quad, const Geometry& cube, const Geometry& sphere,
    const Shader& s_phong, const Shader& s_depth, const Shader& s_debug, const Shader& s_light,
    const Texture& t_albedo, const Texture& t_specular,
    const uint32_t fbo1, const uint32_t fbo1_texture, const uint32_t fbo2, const uint32_t fbo2_texture) {

    //FIRST PASS
    //Light1
    glBindFramebuffer(GL_FRAMEBUFFER, fbo1);
    glViewport(0, 0, k_shadow_width, k_shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const glm::mat4 lightProjection = glm::perspective(glm::radians(45.0f), 1.0f, k_shadow_near, k_shadow_far);
    const glm::mat4 light1View = glm::lookAt(light1Pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 light1SpaceMatrix = lightProjection * light1View;

    s_depth.use();
    s_depth.set("lightSpaceMatrix", light1SpaceMatrix);
    //glCullFace(GL_FRONT);
    renderScene(s_depth, quad, cube, sphere, t_albedo, t_specular, light1View, lightProjection);
    //glCullFace(GL_BACK);

    //Light2
    glBindFramebuffer(GL_FRAMEBUFFER, fbo2);
    glViewport(0, 0, k_shadow_width, k_shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    const glm::mat4 light2View = glm::lookAt(light2Pos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::mat4 light2SpaceMatrix = lightProjection * light2View;

    s_depth.set("lightSpaceMatrix", light2SpaceMatrix);
    //glCullFace(GL_FRONT);
    renderScene(s_depth, quad, cube, sphere, t_albedo, t_specular, light2View, lightProjection);
    //glCullFace(GL_BACK);

//SECOND PASS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / Window::instance()->getHeight(), 0.1f, 100.0f);


    //Lights
    s_light.use();
    //Light1
    glm::mat4 model = glm::mat4(1.0);
    model = glm::translate(model, light1Pos);
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", 1.0f, 0.0f, 0.0f);
    sphere.render();

    //Light2
    model = glm::mat4(1.0);
    model = glm::translate(model, light2Pos);
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", 1.0f, 1.0f, 1.0f);
    sphere.render();

    s_phong.use();

    s_phong.set("view", view);
    s_phong.set("proj", proj);

    s_phong.set("viewPos", camera.getPosition());

    s_phong.set("spotLight[0].position", light1Pos);
    s_phong.set("spotLight[0].direction", light1Dir);
    s_phong.set("spotLight[0].ambient", 0.1f, 0.1f, 0.1f);
    s_phong.set("spotLight[0].diffuse", 0.5f, 0.5f, 0.5f);
    s_phong.set("spotLight[0].specular", 1.0f, 1.0f, 1.0f);
    s_phong.set("spotLight[0].constant", 1.0f);
    s_phong.set("spotLight[0].linear", 0.2f);
    s_phong.set("spotLight[0].quadratic", 0.06f);
    s_phong.set("spotLight[0].cutOff", glm::cos(glm::radians(30.0f)));
    s_phong.set("spotLight[0].outerCutOff", glm::cos(glm::radians(35.0f)));

    s_phong.set("spotLight[1].position", light2Pos);
    s_phong.set("spotLight[1].direction", light2Dir);
    s_phong.set("spotLight[1].ambient", 0.1f, 0.1f, 0.1f);
    s_phong.set("spotLight[1].diffuse", 0.5f, 0.5f, 0.5f);
    s_phong.set("spotLight[1].specular", 1.0f, 1.0f, 1.0f);
    s_phong.set("spotLight[1].constant", 1.0f);
    s_phong.set("spotLight[1].linear", 0.2f);
    s_phong.set("spotLight[1].quadratic", 0.06f);
    s_phong.set("spotLight[1].cutOff", glm::cos(glm::radians(30.0f)));
    s_phong.set("spotLight[1].outerCutOff", glm::cos(glm::radians(35.0f)));

    s_phong.set("material.shininess", 32);

    s_phong.set("light1SpaceMatrix", light1SpaceMatrix);
    s_phong.set("light2SpaceMatrix", light2SpaceMatrix);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, fbo1_texture);
    s_phong.set("depthMap[0]", 2);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, fbo2_texture);
    s_phong.set("depthMap[1]", 3);

    renderScene(s_phong, quad, cube, sphere, t_albedo, t_specular, view, proj);


    //Debug
    /*glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Window::instance()->getWidth(), Window::instance()->getHeight());
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    s_debug.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fbo1_texture);
    s_debug.set("depthMap", 0);
    s_debug.set("near_plane", k_shadow_near);
    s_debug.set("far_plane", k_shadow_far);

    quad.render();*/
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_phong("../projects/EJ14_02/phong.vs", "../projects/EJ14_02/blinn.fs");
    const Shader s_depth("../projects/EJ14_02/depth.vs", "../projects/EJ14_02/depth.fs");
    const Shader s_debug("../projects/EJ14_02/debug.vs", "../projects/EJ14_02/debug.fs");
    const Shader s_light("../projects/EJ14_02/light.vs", "../projects/EJ14_02/light.fs");
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Quad quad(2.0f);
    const Cube cube(1.0f);
    const Sphere sphere(1.0f, 25, 25);

    auto fbo1 = createFBO();
    auto fbo2 = createFBO();

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, cube, sphere, s_phong, s_depth, s_debug, s_light, t_albedo, t_specular, fbo1.first, fbo1.second, fbo2.first, fbo2.second);
        window->frame();
    }

    glDeleteFramebuffers(1, &fbo1.first);
    glDeleteFramebuffers(1, &fbo2.first);
    glDeleteTextures(1, &fbo1.second);
    glDeleteTextures(1, &fbo2.second);

    return 0;
}