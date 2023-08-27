#include <algorithm>
#include <glm/glm.hpp>

#include "../../engine/renderer/mesh.h"
#include "../../engine/ray/ray.h"

#include "block.h"
#include "chunk.h"

using namespace GameModule;

constexpr uint32_t g_chunkSizeX = 16;
constexpr uint32_t g_chunkSizeY = 16;
constexpr uint32_t g_chunkSizeZ = 16;

constexpr uint32_t g_vertexPerCube = 36;
constexpr uint32_t g_vertexPerFace = 6;

constexpr float g_rayDeltaMag = 0.25f;

Chunk GameModule::generateChunk(glm::vec3 pos)
{
	Chunk chunk;
	chunk.pos = pos;
	chunk.blocks = new Block[g_chunkSizeX * g_chunkSizeY * g_chunkSizeZ];

	for (uint32_t x = 0; x < g_chunkSizeX; x++)
	{
		for (uint32_t y = 0; y < g_chunkSizeY; y++)
		{
			for (uint32_t z = 0; z < g_chunkSizeZ; z++)
			{
				auto index = y * g_chunkSizeX * g_chunkSizeZ + z * g_chunkSizeX + x;
				chunk.blocks[index].type = y < 5 ? BlockType::GRASS_DIRT : BlockType::AIR;

				chunk.blocks[index].pos = pos + glm::vec3(x, y, z);
			}
		}
	}

	return chunk;
}

void addBlcokkPosOffset(Engine::Renderer::Vertex* vertices, const glm::vec3& chunkPos, const size_t size)
{
	for (uint32_t vIndex = 0; vIndex < size; vIndex++)
	{
		vertices[vIndex].pos += chunkPos;
	}
}

bool checkAir(Block* blocks, uint32_t size, uint32_t index)
{
	if (index < size)
	{
		if (blocks[index].type == BlockType::AIR)
		{
			return true;
		}
	}
	return false;
}

void updateFacePos(Engine::Renderer::Vertex* vertices, const Block& block)
{
	for (uint32_t iVertex = 0; iVertex < g_vertexPerFace; iVertex++)
	{
		vertices[iVertex].pos += block.pos;
	}
}

void GameModule::generateMesh(Chunk& chunk)
{
	auto nBlocks = g_chunkSizeX * g_chunkSizeY * g_chunkSizeZ;

	size_t nVertices = nBlocks * g_vertexPerCube;

	if (!chunk.mesh.vertices)
	{
		chunk.mesh.vertices = new Engine::Renderer::Vertex[nVertices];
	}
	else
	{
		delete chunk.mesh.vertices;
		chunk.mesh.vertices = new Engine::Renderer::Vertex[nVertices];
	}
	
	uint32_t iFace = 0;
	for (uint32_t iBlock = 0; iBlock < nBlocks; iBlock++)
	{
		Engine::Renderer::Vertex back[] = {
			{glm::vec3(0, 0, 0), { 0, 0 }},
			{glm::vec3(0, 1, 0), { 0, 1 }},
			{glm::vec3(1, 0, 0), { 1, 0 }},

			{glm::vec3(1, 0, 0), { 1, 0 }},
			{glm::vec3(0, 1, 0), { 0, 1 }},
			{glm::vec3(1, 1, 0), { 1, 1 }},
		};
		Engine::Renderer::Vertex front[] = {
			{glm::vec3(0, 0, 1), { 0, 0 }},
			{glm::vec3(1, 0, 1), { 1, 0 }},
			{glm::vec3(0, 1, 1), { 0, 1 }},

			{glm::vec3(1, 0, 1), { 1, 0 }},
			{glm::vec3(1, 1, 1), { 1, 1 }},
			{glm::vec3(0, 1, 1), { 0, 1 }},
		};
		Engine::Renderer::Vertex top[] = {
			{glm::vec3(0, 1, 1), { 0, 0 }},
			{glm::vec3(1, 1, 1), { 1, 0 }},
			{glm::vec3(0, 1, 0), { 0, 1 }},

			{glm::vec3(1, 1, 1), { 1, 0 }},
			{glm::vec3(1, 1, 0), { 1, 1 }},
			{glm::vec3(0, 1, 0), { 0, 1 }},
		};
		Engine::Renderer::Vertex bottom[] = {
			{glm::vec3(0, 0, 1), { 0, 0 }},
			{glm::vec3(0, 0, 0), { 0, 1 }},
			{glm::vec3(1, 0, 1), { 1, 0 }},

			{glm::vec3(1, 0, 0), { 1, 1 }},
			{glm::vec3(1, 0, 1), { 1, 0 }},
			{glm::vec3(0, 0, 0), { 0, 1 }},
		};
		Engine::Renderer::Vertex left[] = {
			{glm::vec3(0, 0, 0), { 0, 0 }},
			{glm::vec3(0, 0, 1), { 1, 0 }},
			{glm::vec3(0, 1, 0), { 0, 1 }},

			{glm::vec3(0, 0, 1), { 1, 0 }},
			{glm::vec3(0, 1, 1), { 1, 1 }},
			{glm::vec3(0, 1, 0), { 0, 1 }},
		};
		Engine::Renderer::Vertex right[] = {
			{glm::vec3(1, 0, 1), { 0, 0 }},
			{glm::vec3(1, 0, 0), { 1, 0 }},
			{glm::vec3(1, 1, 1), { 0, 1 }},

			{glm::vec3(1, 0, 0), { 1, 0 }},
			{glm::vec3(1, 1, 0), { 1, 1 }},
			{glm::vec3(1, 1, 1), { 0, 1 }}
		};

		auto iPos = chunk.blocks[iBlock].pos - chunk.pos;
		uint32_t iTop = (iPos.y + 1) * g_chunkSizeX * g_chunkSizeZ + iPos.z * g_chunkSizeX + iPos.x;
		uint32_t iFront = iPos.y * g_chunkSizeX * g_chunkSizeZ + (iPos.z + 1) * g_chunkSizeX + iPos.x;
		uint32_t iRight = iPos.y * g_chunkSizeX * g_chunkSizeZ + iPos.z * g_chunkSizeX + (iPos.x + 1);

		if (chunk.blocks[iBlock].type == BlockType::AIR)
		{
			if (chunk.blocks[iTop].type != BlockType::AIR &&
				iPos.y < g_chunkSizeY - 1)
			{
				updateFacePos(bottom, chunk.blocks[iTop]);
				std::copy(
					bottom, bottom + g_vertexPerFace,
					chunk.mesh.vertices + iFace * g_vertexPerCube
				);
				iFace++;
			}

			if (chunk.blocks[iFront].type != BlockType::AIR &&
				iPos.z < g_chunkSizeZ - 1)
			{
				updateFacePos(back, chunk.blocks[iFront]);
				std::copy(
					back, back + g_vertexPerFace,
					chunk.mesh.vertices + iFace * g_vertexPerCube
				);
				iFace++;
			}

			if (chunk.blocks[iRight].type != BlockType::AIR &&
				iPos.x < g_chunkSizeX - 1)
			{
				updateFacePos(left, chunk.blocks[iRight]);
				std::copy(
					left, left + g_vertexPerFace,
					chunk.mesh.vertices + iFace * g_vertexPerCube
				);
				iFace++;
			}
		}
		else
		{
			if (chunk.blocks[iTop].type == BlockType::AIR &&
				iPos.y < g_chunkSizeY - 1)
			{
				updateFacePos(top, chunk.blocks[iBlock]);
				std::copy(
					top, top + g_vertexPerFace,
					chunk.mesh.vertices + iFace * g_vertexPerCube
				);
				iFace++;
			}

			if (chunk.blocks[iFront].type == BlockType::AIR &&
				iPos.z < g_chunkSizeZ - 1)
			{
				updateFacePos(front, chunk.blocks[iBlock]);
				std::copy(
					front, front + g_vertexPerFace,
					chunk.mesh.vertices + iFace * g_vertexPerCube
				);
				iFace++;
			}

			if (chunk.blocks[iRight].type == BlockType::AIR &&
				iPos.x < g_chunkSizeX - 1)
			{
				updateFacePos(right, chunk.blocks[iBlock]);
				std::copy(
					right, right + g_vertexPerFace,
					chunk.mesh.vertices + iFace * g_vertexPerCube
				);
				iFace++;
			}
		}
	}
	chunk.mesh.size = sizeof(Engine::Renderer::Vertex) * nBlocks * g_vertexPerCube;
}

void GameModule::loadChunkMesh(Chunk& chunk)
{
	Engine::Renderer::loadData(&chunk.mesh);
}

void GameModule::drawChunk(const Chunk& chunk)
{
	if (chunk.mesh.size == 0)
	{
		return;
	}

	Engine::Renderer::renderMesh(&chunk.mesh);
}

void GameModule::checkNeighbourChunk(Chunk& chunk1, Chunk& chunk2)
{

}

bool GameModule::rayStartInChunk(const Chunk& chunk, const Engine::Ray& ray)
{
	return 
		chunk.pos.x <= ray.start.x && ray.start.x <= chunk.pos.x + g_chunkSizeX &&
		chunk.pos.z <= ray.start.z && ray.start.z <= chunk.pos.z + g_chunkSizeZ;
}

bool GameModule::processRayInChunk(Chunk& chunk, const Engine::Ray& ray, RayType type)
{
	glm::vec3 dir = glm::normalize(ray.end - ray.start);

	auto rayLength = glm::length(ray.end - ray.start);
	auto nBlocks = g_chunkSizeX * g_chunkSizeY * g_chunkSizeZ;

	uint32_t x;
	uint32_t y;
	uint32_t z;

	for (float rayFrac = 0; rayFrac < rayLength; rayFrac += g_rayDeltaMag)
	{
		glm::vec3 iPos = ray.start + rayFrac * dir - chunk.pos;
		
		x = static_cast<uint32_t>(iPos.x);
		y = static_cast<uint32_t>(iPos.y);
		z = static_cast<uint32_t>(iPos.z);

		uint32_t iBlock = y * g_chunkSizeX * g_chunkSizeZ + z * g_chunkSizeX + x;

		if (iBlock > nBlocks)
		{
			return false;
		}

		if (chunk.blocks[iBlock].type != BlockType::AIR)
		{
			if (type == RayType::REMOVE)
			{
				chunk.blocks[iBlock].type = BlockType::AIR;
				return true;
			}
			else
			{
				iPos -= rayFrac * dir;
				x = static_cast<uint32_t>(iPos.x);
				y = static_cast<uint32_t>(iPos.y);
				z = static_cast<uint32_t>(iPos.z);
				iBlock = y * g_chunkSizeX * g_chunkSizeZ + z * g_chunkSizeX + x;

				chunk.blocks[iBlock].type = BlockType::GRASS_DIRT;
				return true;
			}
		}
	}

	if (type == RayType::PLACE)
	{
		uint32_t iBlock = y * g_chunkSizeX * g_chunkSizeZ + z * g_chunkSizeX + x;
		
		if (iBlock > nBlocks)
		{
			return false;
		}
		chunk.blocks[iBlock].type = BlockType::GRASS_DIRT;
		return true;
	}

	return false;
}