#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

//!
//!

#define SCREEN_W (800)
#define SCREEN_H (800)

#define LOG_ERR(c, m) (std::clog << "[ERR:" << (c) << "] " << (m) << std::endl)

void update();
void upload();



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

	//!
	//! MAIN LOOP
	//!

	glClearColor(1.f, 1.f, 1.f, 1.f);

	//!
	//!

	while(!glfwWindowShouldClose(l_Window))
	{
		glClear(GL_COLOR_BUFFER_BIT);



		glfwSwapInterval(1);
		glfwSwapBuffers(l_Window);
		glfwPollEvents();
	}

	glfwDestroyWindow(l_Window);

	//!
	//!

	return (glfwTerminate(), 0);
}
