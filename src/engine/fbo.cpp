#include "engine/fbo.hpp"
#include <glad/glad.h>
#include <tuple>

FBO::FBO(uint32_t k_width, uint32_t k_height)
: _k_width(k_width), _k_height(k_height) {

}

std::pair<uint32_t, uint32_t> FBO::createShadowFBO() {
    uint32_t fbo = 0;
    uint32_t depthmap = 0;
    
    fbo = FBO::createFBO(fbo);
    depthmap = FBO::createDepthMap(depthmap);
    
    return std::make_pair(fbo, depthmap);
}

std::tuple<uint32_t, uint32_t, uint32_t> FBO::createPostProcessFBO() {
    uint32_t fbo = 0;
    uint32_t textureColor = 0;
    uint32_t rbo=0;

    fbo = FBO::createFBO(fbo);
    textureColor = FBO::createTextureColor(textureColor);
    rbo = FBO::createRBO(rbo);

    return std::make_tuple(fbo, textureColor, rbo);
}

uint32_t FBO::createFBO(uint32_t fbo) {
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    return fbo;
}

uint32_t FBO::createDepthMap(uint32_t depthMap) {

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _k_width, _k_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
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

    return depthMap;

}

uint32_t FBO::createTextureColor(uint32_t textureColor) {
    glGenTextures(1, &textureColor);
    glBindTexture(GL_TEXTURE_2D, textureColor);
   
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Window::instance()->getWidth(), Window::instance()->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _k_width, _k_height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColor, 0);

    return textureColor;
}


uint32_t FBO::createRBO(uint32_t rbo) {
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, Window::instance()->getWidth(), Window::instance()->getHeight());
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, _k_width, _k_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    return rbo;
}