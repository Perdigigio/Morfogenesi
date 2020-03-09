#include "main.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

//!
//!

#define SCREEN_W (800)
#define SCREEN_H (800)

#define LOG_ERR(c, m) (std::clog << "[ERR:" << (c) << "] " << m << std::endl)

typedef Matrix<SCREEN_W, SCREEN_H> Mat;

void update_simulation();
void render_simulation();
void update_ui();
void render_ui();

//!
//!

GLuint g_Program;
GLuint g_Texture[2];
GLuint g_F;

Mat g_Simulation[2];

//!
//!

void createProgram()
{
	g_Program = glCreateProgram();

	GLuint l_VShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint l_FShader = glCreateShader(GL_FRAGMENT_SHADER);

	//!
	//!

	glShaderSource(l_VShader, 1, &g_vert, NULL);
	glShaderSource(l_FShader, 1, &g_frag, NULL);
		glCompileShader(l_VShader);
		glCompileShader(l_FShader);
			glAttachShader(g_Program, l_VShader);
			glAttachShader(g_Program, l_FShader);
				glLinkProgram(g_Program);
			glDetachShader(g_Program, l_VShader);
			glDetachShader(g_Program, l_FShader);

	glDeleteShader(l_FShader);
	glDeleteShader(l_VShader);
}

void createTexture()
{
	glGenTextures(2, g_Texture);

	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//!
	//! INITIALIZE STORAGE [0]
	//!

	//glTexImage2D(GL_TEXTURE_2D, 1, GL_RG32F, SCREEN_W, SCREEN_H, 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, g_Texture[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	//!
	//! INITIALIZE STORAGE [1]
	//!

	//glTexImage2D(GL_TEXTURE_2D, 1, GL_RG32F, SCREEN_W, SCREEN_H, 0, GL_RG, GL_FLOAT, NULL);
}

float g_color[4];

void update_color()
{
	static GLint l_color = 0;

	//!
	//!

	if (l_color == 0)
	{
		l_color = glGetUniformLocation(g_Program, "u_color");
	}

	//!
	//!

	glUniform4fv(l_color, 1, g_color);
}

int main()
{
	//!
	//! SET ERROR CALLBACK
	//!
	
	glfwSetErrorCallback([](int code, const char * desc)
		{
			LOG_ERR(code, desc);
		});

	//!
	//!

	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	GLFWwindow * l_Window = glfwCreateWindow(SCREEN_W, SCREEN_H, "Morph", NULL, NULL);

	//!
	//!

	if (!l_Window)
	{
		//!
		//! ERROR WINDOW
		//!

		return (glfwTerminate(), -1);
	}

	glfwMakeContextCurrent(l_Window);

	//!
	//! INIT GLAD
	//!

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		//!
		//! ERROR LOADER
		//!

		return (glfwDestroyWindow(l_Window), glfwTerminate(), -1);
	}

	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(l_Window, true);
	ImGui_ImplOpenGL3_Init();

	createProgram();
	createTexture();

	//!
	//! MAIN LOOP
	//!

	while (!glfwWindowShouldClose(l_Window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		//!update_simulation();
		render_simulation();
		render_ui();
		update_ui();

		g_F += 1;
		g_F %= 2;

		glfwSwapInterval(1);
		glfwSwapBuffers(l_Window);
		glfwPollEvents();
	}

	glDeleteProgram(g_Program);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	//!
	//!

	return (ImGui::DestroyContext(), glfwDestroyWindow(l_Window), glfwTerminate(), 0);
}

//!
//!

inline glm::vec2 laplacian(const Mat & m, int i, int j)
{
	//!
	//! WRAP COORDINATES
	//!

	int t = (i - 1) < 0 ? SCREEN_H - 1 : i - 1;
	int l = (j - 1) < 0 ? SCREEN_W - 1 : j - 1;
	int b = (i + 1) >= SCREEN_H ? 0 : i + 1;
	int r = (j + 1) >= SCREEN_W ? 0 : j + 1;

	//!
	//!

	glm::vec2 m00 = m(t, l);
	glm::vec2 m01 = m(t, j);
	glm::vec2 m02 = m(t, r);
	glm::vec2 m10 = m(i, l);
	glm::vec2 m11 = m(i, j);
	glm::vec2 m12 = m(i, r);
	glm::vec2 m20 = m(b, l);
	glm::vec2 m21 = m(b, j);
	glm::vec2 m22 = m(b, r);

	//!
	//!

	glm::vec2 dia = (m00 + m02 + m20 + m22) * (.05f);
	glm::vec2 adj = (m01 + m10 + m12 + m21) * (.20f);

	//!
	//!

	return  dia + adj - m11;
}

void update_simulation()
{
	GLuint l_N = (g_F + 1) % 2;

	float D_t = 1.0f;
	float D_a = 1.0f;
	float D_b = 0.5f;
	float f = 0.055f;
	float k = 0.062f;

	for (int i = 0; i < SCREEN_H; i++)
	for (int j = 0; j < SCREEN_W; j++)
	{
		const glm::vec2 lap = laplacian(g_Simulation[g_F], i, j);

		float A = g_Simulation[g_F](i, j).x;
		float B = g_Simulation[g_F](i, j).y;
		float C = A + ((D_a * lap.x - A * B * B) + f * (1 - A)) * D_t;
		float D = B + ((D_b * lap.y - A * B * B) - B * (k + f)) * D_t;

		//!
		//!

		g_Simulation[l_N](i, j) = glm::vec2(C, D);
	}

	glBindTexture(GL_TEXTURE_2D, g_Texture[g_F]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, SCREEN_W, SCREEN_H, 0, GL_RG, GL_FLOAT, g_Simulation[l_N].matrix.get());
}

void render_simulation()
{
	glUseProgram(g_Program);
		update_color();

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void update_ui()
{	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void render_ui()
{
	ImGui::NewFrame();

	//!
	//!

	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::Begin("Parametri", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::ColorPicker4("Background", g_color);
	ImGui::End();

	//!
	//!

	ImGui::Render();
}
