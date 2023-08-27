#pragma once

#include <glm/glm.hpp>
#include <stdint.h>

#include "../../engine/renderer/mesh.h"

namespace Engine
{
	struct Ray;
}

namespace GameModule
{
	struct Block;

	struct Chunk
	{
		glm::vec3 pos;
		Block* blocks;
		Engine::Renderer::Mesh mesh;
	};

	enum class RayType
	{
		REMOVE,
		PLACE
	};

	Chunk generateChunk(glm::vec3 pos);
	void loadChunkMesh(Chunk& chunk);
	bool processRayInChunk(Chunk& chunk, const Engine::Ray& ray, RayType type = RayType::REMOVE);
	bool rayStartInChunk(const Chunk& chunk, const Engine::Ray& ray);
	void checkNeighbourChunk(Chunk& chunk1, Chunk& chunk2);
	void generateMesh(Chunk& chunk);
	void drawChunk(const Chunk& chunk);
}
 