#ifndef __FBO_H__
#define __FBO_H__
#include <glm/glm.hpp>
#include <iostream>

class FBO {

	public:
		FBO(uint32_t k_height, uint32_t k_width);
		std::pair<uint32_t, uint32_t> createShadowFBO();
		std::tuple<uint32_t, uint32_t, uint32_t> createPostProcessFBO();
		uint32_t createFBO(uint32_t fbo);
		uint32_t createDepthMap(uint32_t depthMap);
		uint32_t createTextureColor(uint32_t textureColor);
		uint32_t createRBO(uint32_t rbo);

		
	private:
		uint32_t _k_width;
		uint32_t _k_height;


};







#endif