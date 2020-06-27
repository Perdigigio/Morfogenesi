#define LINE(l) l "\n"

#define SCREEN_W (1280)
#define SCREEN_H (720)

#include <glad/glad.h>
#include <GLFW/glfw3.h>

//!
//! KERNEL
//!

/***************************************************************************************
#version 330

uniform sampler2D t_seed;

uniform float u_Dt; //! = 0.8
uniform float u_Da; //! = 1.0
uniform float u_Db; //! = 0.5
uniform float u_f;
uniform float u_k;

uniform ivec2 u_screen; //! DIMENSIONE DELLA FINESTRA
uniform ivec2 u_canvas; //! DIMENSIONE DEL BUFFER
uniform ivec2 u_pencil; //! POSIZIONE DEL CURSORE IN COORDINATE DELLA FINESTRA (0, 0) ANGOLO ALTO A SINISTRA

in vec2 f_uv; //! COORDINATE DEL PIXEL NORMALIZZATE [0-1][0-1]

float sqrLength(vec2 v)
{
	return dot(v, v);
}
vec2 getBrush()
{
	return vec2(u_pencil.x, u_screen.y - u_pencil.y) - f_uv * u_screen; //! IN OPENGL (PURTROPPO) L'ORIGINE E' IN BASSO A SINISTRA
}

void main()
{
	vec2 step = 1.f / u_canvas;

	vec4 t = texture2D(t_seed, f_uv + vec2(0, +step.y)); //! CAMPIONE PIXEL ADIACENTE
	vec4 b = texture2D(t_seed, f_uv + vec2(0, -step.y)); //! CAMPIONE PIXEL ADIACENTE
	vec4 l = texture2D(t_seed, f_uv + vec2(-step.x, 0)); //! CAMPIONE PIXEL ADIACENTE
	vec4 r = texture2D(t_seed, f_uv + vec2(+step.x, 0)); //! CAMPIONE PIXEL ADIACENTE
	vec4 c = texture2D(t_seed, f_uv);

	vec4 lap = ((t + b + l + r) - 4 * c) / 5; //! LAPLACIANO

	float A = c.x + ((u_Da * lap.x - c.x * c.y * c.y) + u_f * (1.0 - c.x)) * u_Dt;
	float B = c.y + ((u_Db * lap.y + c.x * c.y * c.y) - c.y * (u_k + u_f)) * u_Dt;

	if (u_pencil.x > 0.f)
	{
		if (sqrLength(getBrush()) < 10.f) B = .9;
	}

	gl_FragColor.r = A;
	gl_FragColor.g = B;
	gl_FragColor.b = 0; //! VA SCRITTO PER FORZA 
	gl_FragColor.a = 1; //! VA SCRITTO PER FORZA
}
***************************************************************************************/

//!
//!

static const GLchar * g_vert =
	LINE("#version 330")
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
	LINE("	f_uv.y = (gl_VertexID % 2) == 0 ? 0 : 1;")
	LINE("")
	LINE("}");

static const GLchar * g_frag_render =
	LINE("#version 330")
	LINE("")
	LINE("uniform sampler2D t_data;")
	LINE("")
	LINE("vec4 g_c = vec4(0, 1, 1, 1);")
	LINE("vec4 g_m = vec4(1, 0, 1, 1);")
	LINE("vec4 g_y = vec4(1, 1, 0, 1);")
	LINE("vec4 g_k = vec4(0, 0, 0, 1);")
	LINE("")
	LINE("in vec2 f_uv;")
	LINE("")
	LINE("void main()")
	LINE("{")
	LINE("	vec2 v = texture(t_data, f_uv).xy;")
	LINE("")
	LINE("	vec4 r = mix(g_c, g_k, 0 + v.x);")
	LINE("	vec4 g = mix(g_m, g_k, 1 - v.y);")
	LINE("")
	LINE("	gl_FragColor = mix(r, g, v.x - v.y);")
	LINE("}");

static const GLchar * g_frag_kernel =
	LINE("#version 330")
	LINE("")
	LINE("uniform sampler2D t_seed;")
	LINE("")
	LINE("uniform float u_Dt;")
	LINE("uniform float u_Da;")
	LINE("uniform float u_Db;")
	LINE("uniform float u_f;")
	LINE("uniform float u_k;")
	LINE("")
	LINE("uniform ivec2 u_screen;")
	LINE("uniform ivec2 u_canvas;")
	LINE("uniform ivec2 u_pencil;")
	LINE("")
	LINE("in vec2 f_uv;")
	LINE("")
	LINE("float sqrLength(vec2 v)")
	LINE("{")
	LINE("	return dot(v, v);")
	LINE("}")
	LINE("vec2 getBrush()")
	LINE("{")
	LINE("	return vec2(u_pencil.x, u_screen.y - u_pencil.y) - f_uv * u_screen;")
	LINE("}")
	LINE("")
	LINE("void main()")
	LINE("{")
	LINE("	vec2 step = 1.f / u_canvas;")
	LINE("")
	LINE("	vec4 t = texture2D(t_seed, f_uv + vec2(0, +step.y));")
	LINE("	vec4 b = texture2D(t_seed, f_uv + vec2(0, -step.y));")
	LINE("	vec4 l = texture2D(t_seed, f_uv + vec2(-step.x, 0));")
	LINE("	vec4 r = texture2D(t_seed, f_uv + vec2(+step.x, 0));")
	LINE("	vec4 c = texture2D(t_seed, f_uv);")
	LINE("")
	LINE("	vec4 lap = ((t + b + l + r) - 4 * c) / 5;")
	LINE("")
	LINE("	float A = c.x + ((u_Da * lap.x - c.x * c.y * c.y) + u_f * (1.0 - c.x)) * u_Dt;")
	LINE("	float B = c.y + ((u_Db * lap.y + c.x * c.y * c.y) - c.y * (u_k + u_f)) * u_Dt;")
	LINE("")
	LINE("	if (u_pencil.x > 0.f)")
	LINE("	{ ")
	LINE("		if (sqrLength(getBrush()) < 5.f) B = .9;")
	LINE("	} ")
	LINE("")
	LINE("	gl_FragColor.r = clamp(A, 0, 1); ")
	LINE("	gl_FragColor.g = clamp(B, 0, 1); ")
	LINE("	gl_FragColor.b = 0; ")
	LINE("	gl_FragColor.a = 1; ")
	LINE("")
	LINE("}");

//!
//!

#include <imgui.h>

//!
//! TIMER HELPER
//!

struct Timer
{
	inline Timer() : m_Start(glfwGetTime())
	{
	}

	//!
	//!

	inline double elapsed() const
	{
		return glfwGetTime() - m_Start;
	}

	//!
	//!

	double m_Start;
};

//!
//!

GLFWwindow * g_Window;

GLuint g_Program[2];
GLuint g_Texture[2];
GLuint g_FBO;

float g_presets[][2] =
{
	{ 0.055f, 0.062f }, //! Coral
	{ 0.030f, 0.062f }, //! Solitons
	{ 0.025f, 0.060f }, //! Solitons pulse
	{ 0.078f, 0.061f }, //! Worms
};

float g_Dt = 0.8f;
float g_Da = 1.0f;
float g_Db = 0.5f;
float g_Feed = g_presets[0][0];
float g_Kill = g_presets[0][1];

int g_pencil[2];
int g_screen[2] = { SCREEN_W, SCREEN_H };
int g_canvas[2] = { SCREEN_W, SCREEN_H };

int g_preset = 0;

bool g_Clear = true;
bool g_Frame = true;
bool g_Vsync = true;
bool g_Pause = false;

static inline GLint u_Da()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_Da");
	}

	return i;
}

static inline GLint u_Dt()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_Dt");
	}

	return i;
}

static inline GLint u_Db()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_Db");
	}

	return i;
}

static inline GLint u_f()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_f");
	}

	return i;
}

static inline GLint u_k()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_k");
	}

	return i;
}

static inline GLint u_screen()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_screen");
	}

	return i;
}

static inline GLint u_pencil()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_pencil");
	}

	return i;
}


static inline GLint u_canvas()
{
	static GLint i = -1;

	if (i == -1)
	{
		i = glGetUniformLocation(g_Program[1], "u_canvas");
	}

	return i;
}