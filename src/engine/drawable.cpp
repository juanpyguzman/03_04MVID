#include "engine/drawable.hpp"
#include <typeinfo>

Drawable::Drawable(const Geometry& object, const Material& material)
	: _object(object), _material(material), _isGeometry(true), _isModel(false), _textureInside(false) {
}

Drawable::Drawable(const Model& object, const Material& material)
    : _model(object), _material(material), _isModel(true), _isGeometry(false), _textureInside(false) {
}

Drawable::Drawable(const Model& object)
    : _model(object), _isModel(true), _isGeometry(false), _textureInside(true) {
}

void Drawable::setDrawable(const Shader& shader, glm::vec3 position, float angle, glm::vec3 rotation, glm::vec3 scale) const {

    if (_isGeometry) {
        _material.setMaterial();
        glm::mat4 model = glm::mat4(1.0);
        model = glm::translate(model, glm::vec3(position));
        if (rotation != glm::vec3(0.0f)) {
            model = glm::rotate(model, angle, glm::vec3(rotation));
        }
        model = glm::scale(model, glm::vec3(scale));
        shader.set("model", model);
        glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
        shader.set("normalMat", normalMat);
        shader.set("isModel", _isModel);
        _object.render();
    }

    if (_isModel) {
        if (!_textureInside) {
            _material.setMaterial();
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(position));
            if (rotation != glm::vec3(0.0f)) {
                model = glm::rotate(model, angle, glm::vec3(rotation));
            }
            model = glm::scale(model, glm::vec3(scale));
            shader.set("model", model);
            glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
            shader.set("normalMat", normalMat);
            shader.set("isModel", _isModel);
            _model.render(shader);
        }
        else {
            glm::mat4 model = glm::mat4(1.0);
            model = glm::translate(model, glm::vec3(position));
            if (rotation != glm::vec3(0.0f)) {
                model = glm::rotate(model, angle, glm::vec3(rotation));
            }
            model = glm::scale(model, glm::vec3(scale));
            shader.set("model", model);
            glm::mat3 normalMat = glm::inverse(glm::transpose(glm::mat3(model)));
            shader.set("normalMat", normalMat);
            shader.set("isModel", _isModel);
            _model.render(shader);
        }

    }

}