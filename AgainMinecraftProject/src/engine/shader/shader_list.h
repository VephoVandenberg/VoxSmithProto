#pragma once

#include <map>

namespace Engine
{
	static const char* s_cubeShader = "cube";
	static const char* s_meshShader = "mesh";
	
	using ShaderProgram = std::pair<const char*, const char*>;

	static const std::map<const char*, ShaderProgram> s_shaderPaths = {
		{s_cubeShader, {"shaders/cube_shader.vert", "shaders/cube_shader.frag"}},
		{s_meshShader, {"shaders/mesh_shader.vert", "shaders/mesh_shader.frag"}}
	};
}
