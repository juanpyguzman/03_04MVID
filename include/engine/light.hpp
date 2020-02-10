#ifndef __LIGHT_H__
#define __LIGHT_H__
#include <glm/glm.hpp>

class Shader;

class DirectionalLight {

	public:
		DirectionalLight(glm::vec3 direction, glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
		glm::vec3 getPosition();
		glm::vec3 getDirection();
		void setShader(const Shader& shader);

	private:
		glm::vec3 _direction;
		glm::vec3 _position;
		glm::vec3 _ambient;
		glm::vec3 _diffuse;
		glm::vec3 _specular;

};

class PointLight {

	public:
		PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic);
		PointLight();
		glm::vec3 getPosition();
		glm::vec3 getColor();
		void setShader(const Shader& shader, uint32_t unit);

	private:
		glm::vec3 _position;
		glm::vec3 _ambient;
		glm::vec3 _diffuse;
		glm::vec3 _specular;
		float _constant;
		float _linear;
		float _quadratic;
};

class SpotLight {

	public:
		SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant, float linear, float quadratic, float cutOff, float outerCutOff);
		SpotLight();
		glm::vec3 getPosition();
		glm::vec3 getColor();
		void setShader(const Shader& shader, uint32_t unit);

	private:
		glm::vec3 _position;
		glm::vec3 _direction;
		glm::vec3 _ambient;
		glm::vec3 _diffuse;
		glm::vec3 _specular;
		float _constant;
		float _linear;
		float _quadratic;
		float _cutOff;
		float _outerCutOff;
};

#endif