#include "main.hpp"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <algorithm>
#include <cstdio>

//!
//!


#define LOG_ERR(c, m) std::fprintf(stderr, "[ERR: %x] %s \n", (c), (m))

void toggle_fulscreen();
void update_simulation();
void render_simulation();
void update_ui();
void render_ui();

#define FRAME_SKIP (30)
#define FRAME_TIME (1.f / 62.f)

//!
//!

void createProgram()
{
	glGenVertexArrays(1, &g_VAO);

	g_Program[0] = glCreateProgram();
	g_Program[1] = glCreateProgram();

	GLuint l_VShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint l_FShaderRender = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint l_FShaderKernel = glCreateShader(GL_FRAGMENT_SHADER);

	//!
	//! RENDER PROGRAM
	//!

	glShaderSource(l_VShader, 1, &g_vert, NULL);
		glCompileShader(l_VShader);

	glShaderSource(l_FShaderRender, 1, &g_frag_render, NULL);
	glShaderSource(l_FShaderKernel, 1, &g_frag_kernel, NULL);
		glCompileShader(l_FShaderRender);
		glCompileShader(l_FShaderKernel);

	glAttachShader(g_Program[0], l_VShader);
	glAttachShader(g_Program[0], l_FShaderRender);
		glLinkProgram(g_Program[0]);
	glDetachShader(g_Program[0], l_FShaderRender);
	glDetachShader(g_Program[0], l_VShader);

	//!
	//! KERNEL PROGRAM
	//!

	glAttachShader(g_Program[1], l_VShader);
	glAttachShader(g_Program[1], l_FShaderKernel);
		glLinkProgram(g_Program[1]);
	glDetachShader(g_Program[1], l_FShaderKernel);
	glDetachShader(g_Program[1], l_VShader);

	//!
	//! CLEANUP
	//!

	glDeleteShader(l_FShaderKernel);
	glDeleteShader(l_FShaderRender);
	glDeleteShader(l_VShader);
}

void createTexture()
{
	glGenTextures(2, g_Texture);

	glBindTexture(GL_TEXTURE_2D, g_Texture[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16, g_canvas[0], g_canvas[1], 0, GL_RG, GL_FLOAT, NULL);

	glBindTexture(GL_TEXTURE_2D, g_Texture[1]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16, g_canvas[0], g_canvas[1], 0, GL_RG, GL_FLOAT, NULL);

	glGenFramebuffers(1, &g_FBO);
}

void clear_texture()
{
	glBindFramebuffer(GL_FRAMEBUFFER, g_FBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_Texture[0], 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, g_Texture[1], 0);		
			glClearColor(1.f, 0.f, 0.f, 0.f);
				glClear(GL_COLOR_BUFFER_BIT);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, 0, 0);
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
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);

	g_Window = glfwCreateWindow(SCREEN_W, SCREEN_H, "Morph", NULL, NULL);

	//!
	//!

	if (!g_Window)
	{
		//!
		//! ERROR WINDOW
		//!

		return (glfwTerminate(), -1);
	}

	//!
	//! RESIZE CALLBACK
	//!

	glfwSetWindowSizeCallback(g_Window, [](GLFWwindow*, int w, int h)
		{
			g_screen[0] = w;
			g_screen[1] = h;
		});

	glfwSetCharCallback(g_Window, [](GLFWwindow*, unsigned int c)
		{
			if (c == 'f') toggle_fulscreen();
			if (c == 'F') toggle_fulscreen();
			if (c == 'p') g_Pause = !g_Pause;
			if (c == 'P') g_Pause = !g_Pause;
			if (c == 'v') g_Vsync = !g_Vsync;
			if (c == 'V') g_Vsync = !g_Vsync;
		});

	//!
	//!

	glfwMakeContextCurrent(g_Window);

	//!
	//! INIT GLAD
	//!

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		//!
		//! ERROR LOADER
		//!

		return (glfwDestroyWindow(g_Window), glfwTerminate(), -1);
	}

	glEnable(GL_DEBUG_OUTPUT_KHR);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR);

	glDebugMessageCallbackKHR([](GLenum source,GLenum type,GLuint id,GLenum severity,GLsizei length,const GLchar *message,const void *userParam)
	{
		if (severity == GL_DEBUG_SEVERITY_HIGH_KHR ||
			severity == GL_DEBUG_SEVERITY_MEDIUM_KHR ||
			severity == GL_DEBUG_SEVERITY_LOW_KHR)
		{
			LOG_ERR(id, message);
		}
	}, NULL);

	ImGui::CreateContext();

	ImGui_ImplGlfw_InitForOpenGL(g_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	createProgram();
	createTexture();

	//!
	//!

	while (!glfwWindowShouldClose(g_Window))
	{
		glBindVertexArray(g_VAO);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();

		if (!g_Pause)
		{
			update_simulation();
		}

		render_simulation();
		update_ui();
		render_ui();

		glfwSwapInterval(g_Vsync ? 1 : 0);
		glfwSwapBuffers(g_Window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &g_VAO);
	glDeleteFramebuffers(1, &g_FBO);
	glDeleteTextures(2, g_Texture);
	glDeleteProgram(g_Program[1]);
	glDeleteProgram(g_Program[0]);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	//!
	//!

	return (ImGui::DestroyContext(), glfwDestroyWindow(g_Window), glfwTerminate(), 0);
}

//!
//!

void toggle_fulscreen()
{
	const GLFWvidmode * l_Mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	//!
	//!

	if (g_Frame)
	{
		glfwSetWindowAttrib(g_Window, GLFW_DECORATED, GLFW_FALSE);
		glfwSetWindowAttrib(g_Window, GLFW_RESIZABLE, GLFW_FALSE);

		//!
		//!

		glfwSetWindowSize(g_Window, l_Mode->width, l_Mode->height);
		glfwSetWindowPos(g_Window, 0, 0);

	}
	else
	{
		glfwSetWindowAttrib(g_Window, GLFW_DECORATED, GLFW_TRUE);
		glfwSetWindowAttrib(g_Window, GLFW_RESIZABLE, GLFW_TRUE);

		//!
		//!

		glfwSetWindowSize(g_Window, SCREEN_W, SCREEN_H);
		glfwSetWindowPos(g_Window, 50, 50);

	}

	//!
	//!

	g_Frame = !g_Frame;
}

void update_simulation()
{
	Timer l_Timer;

	double x = -1;
	double y = -1;
	
	if (glfwGetMouseButton(g_Window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		//!
		//!

		glfwGetCursorPos(g_Window, &x, &y);
	}

	g_pencil[0] = (int)(x);
	g_pencil[1] = (int)(y);

	if (g_Clear)
	{
		 g_Clear = (clear_texture(), false);
	}

	glViewport(0, 0, SCREEN_W, SCREEN_H);

	glUseProgram(g_Program[1]);
		glUniform1f(u_Dt(), g_Dt); //! Dt
		glUniform1f(u_Da(), g_Da); //! Da
		glUniform1f(u_Db(), g_Db); //! Db
		glUniform1f(u_f(), g_Feed); //! f
		glUniform1f(u_k(), g_Kill); //! k
		glUniform2iv(u_pencil(), 1, g_pencil); //! pencil
		glUniform2iv(u_canvas(), 1, g_canvas); //! canvas
		glUniform2iv(u_screen(), 1, g_screen); //! screen

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, g_FBO);

	for (int i = 0; i < FRAME_SKIP; i++)
	{
		if (l_Timer.elapsed() > FRAME_TIME)
		{
			break;
		}

		glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, g_Texture[1], 0);
			
		glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, g_Texture[0]);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		//!
		//! SWAP TARGETS
		//!

		std::swap(g_Texture[0], g_Texture[1]);
	}
}

void render_simulation()
{
	glUseProgram(g_Program[0]);

	glViewport(0, 0, g_screen[0], g_screen[1]);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, g_Texture[1]);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

void render_ui()
{	
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static const char * g_items[] =
{
	"Coral", "Solitons", "Pulsating solitons", "Worms"
};

void update_ui()
{
	ImGui::NewFrame();

	//!
	//!

	ImGui::SetNextWindowPos(ImVec2(0, 0));

	ImGui::Begin("Parametri", NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);

		if (ImGui::Button("Reset"))
		{
			g_Clear = true;
		}

		if (ImGui::Combo("Presets", &g_preset, g_items, 4))
		{
			g_Feed = g_presets[g_preset][0];
			g_Kill = g_presets[g_preset][1];
		}

		ImGui::SliderFloat("Da", &g_Da, 0, 1.f);
		ImGui::SliderFloat("Db", &g_Db, 0, 1.f);
		ImGui::SliderFloat("feed rate", &g_Feed, 0, .1f);
		ImGui::SliderFloat("kill rate", &g_Kill, 0, .1f);
	ImGui::End();


	ImGui::SetNextWindowPos(ImGui::GetIO().DisplaySize, 0, ImVec2(1, 1));

#ifdef _DEBUG
	ImGui::Begin("Stat", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f  ms", ImGui::GetIO().DeltaTime * 1000.f);
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "%.2f FPS", 1.f / ImGui::GetIO().DeltaTime);
	ImGui::End();
#endif

	ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x, 0), 0, ImVec2(1, 0));

	ImGui::Begin("Legend", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "[F] toggle fullscreen");
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "[P] toggle pause: %s", g_Pause ? "off" : "on");
		ImGui::TextColored(ImVec4(0, 1, 0, 1), "[V] toggle vsync: %s", g_Vsync ? "off" : "on");
	ImGui::End();

	//!
	//! CLAMP DATA
	//!

	g_Feed = std::max(std::min(g_Feed, 0.1f), 0.f);
	g_Kill = std::max(std::min(g_Kill, 0.1f), 0.f);

	//!
	//!

	ImGui::Render();
}
