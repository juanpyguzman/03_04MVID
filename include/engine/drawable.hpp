#ifndef __DRAWABLE_H__
#define __DRAWABLE_H__

#include "engine/material.hpp"
#include "engine/shader.hpp"
#include "engine/geometry/geometry.hpp"
#include "engine/model.hpp"
#include <glm/gtc/matrix_transform.hpp>


class Drawable {

public:
	Drawable(const Geometry& object, const Material& material);
	Drawable(const Model& object, const Material& material);
	Drawable(const Model& object);
	void setDrawable(const Shader& shader, glm::vec3 position, float angle, glm::vec3 rotation, glm::vec3 scale) const;



private:
	Material _material;
	Geometry _object;
	Model _model;
	bool _isGeometry;
	bool _isModel;
	bool _textureInside;


};

#endif