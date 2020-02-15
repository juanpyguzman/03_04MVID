#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "engine/texture.hpp"
#include "engine/shader.hpp"

class Material {

	public:
		Material();
		Material(const Texture& t_albedo, const Texture& t_specular, const Texture& t_normal, int shininess, const Shader& shader);
		void setMaterial() const;



	private:
		Texture _t_albedo;
		Texture _t_specular;
		Texture _t_normal;
		int _shininess;
		Shader _shader;








};

#endif