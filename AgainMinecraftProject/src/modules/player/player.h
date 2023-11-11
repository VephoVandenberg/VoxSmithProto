#pragma once

#include <glm/glm.hpp>

#include "../../engine/camera/camera.h"

namespace GameModule
{
	struct Player 
	{
		glm::vec3 pos;
		glm::vec3 velocity;
		uint32_t height;
		float speed;

		Engine::Camera camera;
	};
}