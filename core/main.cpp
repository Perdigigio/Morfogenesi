#include "main.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <iostream>

//!
//!

#define SCREEN_W (800)
#define SCREEN_H (800)

#define LOG_ERR(c, m) (std::clog << "[ERR:" << (c) << "] " << m << std::endl)

void update_simulation();
void update_ui();
void render_ui();

//!
//!

GLuint g_Program = 0;

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

	//!
	//! MAIN LOOP
	//!

	while (!glfwWindowShouldClose(l_Window))
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		update_simulation();
		render_ui();
		update_ui();

		glfwSwapInterval(1);
		glfwSwapBuffers(l_Window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	//!
	//!

	return (ImGui::DestroyContext(), glfwDestroyWindow(l_Window), glfwTerminate(), 0);
}

//!
//!

void update_simulation()
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
