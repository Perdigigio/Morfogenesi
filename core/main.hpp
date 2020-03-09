#define LINE(l) l "\n"

#include <glad/glad.h>

#include <glm/glm.hpp>

#include <GLFW/glfw3.h>

static const GLchar * g_vert =
	LINE("#version 130")
	LINE("")
	LINE("out vec2 f_uv;")
	LINE("")
	LINE("void main()")
	LINE("{")
	LINE("	gl_Position.x = (gl_VertexID / 2) == 0 ? -1 : 1;")
	LINE("	gl_Position.y = (gl_VertexID % 2) == 0 ? -1 : 1;")
	LINE("	gl_Position.z = 0;")
	LINE("	gl_Position.w = 1;")
	LINE("")
	LINE("	f_uv.x = (gl_VertexID / 2) == 0 ? 0 : 1;")
	LINE("	f_uv.y = (gl_VertexID & 2) == 0 ? 0 : 1;")
	LINE("")
	LINE("}");

static const GLchar * g_frag =
	LINE("#version 130")
	LINE("")
	LINE("uniform vec4 u_color;")
	LINE("")
	LINE("void main()")
	LINE("{")
	LINE("	gl_FragColor = u_color;")
	LINE("}");

//!
//!

#include <memory>

template<size_t W, size_t H> struct Matrix
{
	Matrix() : matrix(std::make_unique<glm::vec2[]>(W * H))
	{
		for (size_t i = 0; i < H; i++)
		for (size_t j = 0; j < W; j++) matrix[i * W + j] = glm::vec2(1.f, 0.f);
	}

	//!
	//!

	inline glm::vec2 & operator()(int i, int j) const
	{
		//!
		//!

		return matrix[i * W + j];
	}

	//!
	//!

	std::unique_ptr<glm::vec2[]> matrix;
};

//!
//!

#include <imgui.h>
