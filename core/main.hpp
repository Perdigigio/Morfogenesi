#define LINE(l) l "\n"

#include <glad/glad.h>
#include <GLFW/glfw3.h>


static const GLchar * g_vert =
	LINE("#version 130")
	LINE("")
	LINE("void main()")
	LINE("{")
	LINE("	gl_Position.x = (gl_VertexID / 2) == 0 ? -1 : 1;")
	LINE("	gl_Position.y = (gl_VertexID % 2) == 0 ? -1 : 1;")
	LINE("	gl_Position.z = 0;")
	LINE("	gl_Position.w = 1;")
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

#include <imgui.h>
