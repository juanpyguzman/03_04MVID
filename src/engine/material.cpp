#include "engine/material.hpp"


Material::Material(const Texture& t_albedo, const Texture& t_specular, const Texture& t_normal, int shininess, const Shader& shader)
	: _t_albedo(t_albedo), _t_specular(t_specular), _t_normal(t_normal), _shininess(shininess), _shader(shader) {
}

void Material::setMaterial() const {

	_t_albedo.use(_shader, "material.diffuse", 0);
	_t_specular.use(_shader, "material.specular", 1);
	_t_normal.use(_shader, "material.normal", 2);
	_shader.set("material.shininess", _shininess);

}