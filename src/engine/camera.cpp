#include "engine/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera(const glm::vec3& position, const glm::vec3& up, float yaw, float pitch)
 : _position(position), _worldUp(up), _yaw(yaw), _pitch(pitch), _fov(k_FOV) {
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
    : _position(glm::vec3(posX, posY, posZ)), _worldUp(glm::vec3(upX, upY, upZ)), _yaw(yaw), _pitch(pitch), _fov(k_FOV) {
    updateCameraVectors();
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(_position, _position + _front, _up);
}

//Modificación del ejercicio EJ06_01 para usar cámara tipo FPS 
glm::mat4 Camera::getViewMatrix(bool FPS, float ground) const {
    glm::vec3 positionFPS = _position;
    
    //Si está habilitado el modo FPS se fija la altura (coordenada "y" de la posición) sobre el suelo
    if (FPS) {
        positionFPS[1] = ground;
    }

    return glm::lookAt(positionFPS, positionFPS + _front, _up);
}

//Modificación del ejercicio EJ06_02 para implementar la función LookAt sin usar la de GLM
glm::mat4 Camera::getViewMatrixNoGLM() const {
    glm::mat4 matrixRUD = glm::mat4(1.0f);
    glm::mat4 matrixP = glm::mat4(1.0f);

    matrixRUD[0] = glm::vec4(-_right,0);
    matrixRUD[1] = glm::vec4(_up, 0);
    matrixRUD[2] = glm::vec4(_front, 0);
    
    matrixP[3] = glm::vec4(_position, 1);

    return glm::mat4(glm::transpose(matrixRUD)) * matrixP;
}


float Camera::getFOV() const {
    return _fov;
}

glm::vec3 Camera::getPosition() const {
    return _position;
}

glm::vec3 Camera::getCameraFront() const {
    return _front;
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    front.y = sin(glm::radians(_pitch));
    front.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(front);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}

void Camera::handleKeyboard(Movement direction, float dt) {
    const float velocity = k_Speed * dt;

    switch (direction) {
        case Movement::Forward: _position += _front * velocity; break;
        case Movement::Backward: _position -= _front * velocity; break;
        case Movement::Left: _position -= _right * velocity; break;
        case Movement::Right: _position += _right * velocity; break;
        default:;
    }
}

void Camera::handleMouseMovement(float xoffset, float yoffset, bool constrainPitch) {
    const float xoff = xoffset * k_Sensitivity;
    const float yoff = yoffset * k_Sensitivity;

    _yaw += xoff;
    _pitch += yoff;

    if (constrainPitch) {
        if (_pitch > 89.0f) _pitch = 89.0f;
        if (_pitch < -89.0f) _pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::handleMouseScroll(float yoffset) {
    if (_fov >= 1.0f && _fov <= 45.0f) _fov -= yoffset;
    if (_fov <= 1.0f) _fov = 1.0f;
    if (_fov >= 45.0f) _fov = 45.0f;
}

