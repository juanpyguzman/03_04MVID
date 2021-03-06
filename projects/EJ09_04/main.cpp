/*
EJ09.04 - En la escena del punto 1, pintarla con una luz direccional, 2 point lights y 2 spotlights.
Pintar una esfera para cada luz en la posici�n de cada una de ellas. Y hacer que cada luz emita un color/propiedades distintas.
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
#include "engine/geometry/teapot.hpp"
#include "engine/light.hpp"
#include <iostream>

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

DirectionalLight dirLight(
    glm::vec3(-0.2f, -1.0f, -0.3f), //Direction
    glm::vec3(0.0f, 0.0f, 0.0f),    //Position (for shadow mapping)
    glm::vec3(0.02f, 0.02f, 0.02f), //Ambient
    glm::vec3(0.3f, 0.3f, 0.3f),    //Diffuse
    glm::vec3(0.2f, 0.2f, 0.2f)     //Specular
);

uint32_t numberPointLights = 2;
PointLight* pointLight = new PointLight[numberPointLights]
{
    {glm::vec3(4.0f, 2.0f, 0.0f),   //Position
    glm::vec3(0.02f, 0.02f, 0.02f), //Ambient
    glm::vec3(0.2f, 0.0f, 0.0f),    //Diffuse
    glm::vec3(0.5f, 0.0f, 0.0f),    //Specular
    0.7f,                           //Constant
    0.18f,                          //Linear
    0.024f },                       //Quadratic

    {glm::vec3(-4.0f, 2.0f, 0.0f),  //Position
    glm::vec3(0.02f, 0.02f, 0.02f), //Ambient
    glm::vec3(0.0f, 0.4f, 0.0f),    //Diffuse
    glm::vec3(0.0f, 0.5f, 0.0f),    //Specular
    1.0f,                           //Constant
    0.09f,                          //Linear
    0.032f }                        //Quadratic
};


uint32_t numberSpotLights = 2;
SpotLight* spotLight = new SpotLight[numberSpotLights]
{
    {glm::vec3(0.0f, 2.0f, 0.0f), //Position
    glm::vec3(0.0f, -1.0f, 0.0f), //Direction
    glm::vec3(0.1f, 0.1f, 0.1f),  //Ambient
    glm::vec3(0.5f, 0.5f, 0.5f),  //Diffuse
    glm::vec3(1.0f, 1.0f, 1.0f),  //Specular
    1.0f,                         //Constant 
    0.2f,                         //Linear
    0.06f,                        //Quadratic 
    glm::cos(glm::radians(25.0f)),//CutOff
    glm::cos(glm::radians(30.0f)) //OuterCutOff
    },

    {glm::vec3(0.0f, 1.0f, -3.0f), //Position
    glm::vec3(0.0f, -1.0f, -0.5f), //Direction
    glm::vec3(0.1f, 0.0f, 0.1f),   //Ambient
    glm::vec3(0.5f, 0.0f, 0.6f),   //Diffuse 
    glm::vec3(1.0f, 0.0f, 1.0f),   //Specular
    1.0f,                          //Constant
    0.3f,                          //Linear
    0.08f,                         //Quadratic
    glm::cos(glm::radians(20.0f)), //CutOff
    glm::cos(glm::radians(25.0f))  //OuterCutOff 
    }
};

glm::vec3 teapotPositions[] = {
    glm::vec3(4.0f, 0.0f, 0.0f),
    glm::vec3(-4.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 0.0f, 4.0f),
    glm::vec3(0.0f, 0.0f, -4.0f),
    glm::vec3(4.0f, 0.0f, 4.0f),
    glm::vec3(4.0f, 0.0f, -4.0f),
    glm::vec3(-4.0f, 0.0f, 4.0f),
    glm::vec3(-4.0f, 0.0f, -4.0f),
    glm::vec3(0.0f, 0.0f, 0.0f),
};

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

void render(const Geometry& floor, const Geometry& object, const Geometry& light, const Shader& s_phong, const Shader& s_light,
    const Texture& t_albedo, const Texture& t_specular) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 proj = glm::perspective(glm::radians(camera.getFOV()), static_cast<float>(Window::instance()->getWidth()) / static_cast<float>(Window::instance()->getHeight()), 0.1f, 100.0f);

    s_light.use();

    // Point Lights
    model = glm::mat4(1.0f);
    model = glm::translate(model, pointLight[0].getPosition());
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", 1.0f, 0.0f, 0.0f);

    light.render();
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, pointLight[1].getPosition());
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", 0.0f, 1.0f, 0.0f);

    light.render();


    // Spot Lights
    model = glm::mat4(1.0f);
    model = glm::translate(model, spotLight[0].getPosition());
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", spotLight[0].getColor());

    light.render();

    model = glm::mat4(1.0f);
    model = glm::translate(model, spotLight[1].getPosition());
    model = glm::scale(model, glm::vec3(0.25f));
    s_light.set("model", model);
    s_light.set("view", view);
    s_light.set("proj", proj);
    s_light.set("lightColor", spotLight[1].getColor());

    light.render();

    // Objetos y suelo
    s_phong.use();
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
    model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(10.0f, 10.0f, 10.0f));
    s_phong.set("model", model);
    s_phong.set("view", view);
    s_phong.set("proj", proj);

    glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
    s_phong.set("normalMat", normalMat);

    s_phong.set("viewPos", camera.getPosition());

    //Luz direccional
    dirLight.setShader(s_phong);

    // Point Lights
    pointLight[0].setShader(s_phong, 0);
    pointLight[1].setShader(s_phong, 1);

    // Spot Lights
    spotLight[0].setShader(s_phong, 0);
    spotLight[1].setShader(s_phong, 1);
    
    // Suelo
    t_albedo.use(s_phong, "material.diffuse", 0);
    t_specular.use(s_phong, "material.specular", 1);
    s_phong.set("material.shininess", 32);

    floor.render();

    for (const auto& teapotPos : teapotPositions) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, teapotPos);
        model = glm::translate(model, glm::vec3(0.0f, -0.5f, 0.0f));
        model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        s_phong.set("model", model);

        glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
        s_phong.set("normalMat", normalMat);

        object.render();
    }
}

int main(int, char* []) {
    Window* window = Window::instance();

    glClearColor(0.0f, 0.3f, 0.6f, 1.0f);

    const Shader s_phong("../projects/EJ09_04/phong.vs", "../projects/EJ09_04/blinn.fs");
    const Shader s_light("../projects/EJ09_04/light.vs", "../projects/EJ09_04/light.fs");
    const Texture t_albedo("../assets/textures/bricks_albedo.png", Texture::Format::RGB);
    const Texture t_specular("../assets/textures/bricks_specular.png", Texture::Format::RGB);
    const Sphere sphere(1.0f, 50, 50);
    const Teapot teapot(20);
    const Quad quad(1.0f);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Input::instance()->setKeyPressedCallback(onKeyPress);
    Input::instance()->setMouseMoveCallback(onMouseMoved);
    Input::instance()->setScrollMoveCallback(onScrollMoved);

    while (window->alive()) {
        const float currentFrame = glfwGetTime();
        const float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        handleInput(deltaTime);
        render(quad, teapot, sphere, s_phong, s_light, t_albedo, t_specular);
        window->frame();
    }

    return 0;
}