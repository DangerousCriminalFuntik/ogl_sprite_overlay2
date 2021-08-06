#include <iostream>

#include <glad/glad.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "sprite.h"

// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------

bool   g_bFirstRendering = true;
float  g_fElpasedTime = 0.0f;
double g_dCurTime;
double g_dLastTime;

int main()
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	GLFWwindow* window = glfwCreateWindow(1024, 768, "OpenGL - Animated Sprite Overlays", nullptr, nullptr);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize OpenGL context" << std::endl;
		return -1;
	}

	if (GLAD_GL_VERSION_4_6) {
		std::cout << "We support at least OpenGL version 4.6" << std::endl;
	}

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	sprite g_donutSprite;
	sprite g_numberSprite;

	g_donutSprite.setPosition(glm::vec2(0.0f, 0.0f));
	g_donutSprite.setSize(glm::vec2(1.0f, 1.0f));
	g_donutSprite.setFrameDelay(0.01f);
	g_donutSprite.setTextureAnimeInfo(512, 512, 64, 64, 5, 6, 30);

	//-------------------------------------------------------------------------
	//
	// -- Argument explantion for setTextureAnimeInfo() call above -- 
	//
	// From a 512 x 512 texture map we're going to pull a series of frames 
	// that are 64 x 64.
	// 
	// The frames have been laid out in a column/row fashion starting in the 
	// upper left corner with 5 frames in each of the 6 rows.
	//
	// The total frame count is 30 because all rows are fully complete and 
	// contain 5 frames with none of them being short any frames.
	//
	//-------------------------------------------------------------------------

	//
	// Load up the number sprite...
	//

	g_numberSprite.setPosition(glm::vec2(-5.0f,-3.0f));
	g_numberSprite.setSize(glm::vec2(0.5f, 0.5f));
	g_numberSprite.setFrameDelay(1.0f);
	g_numberSprite.setTextureAnimeInfo(512, 512, 15, 20, 5, 2, 10, 328, 4);

	//-------------------------------------------------------------------------
	//
	// -- Argument explantion for setTextureAnimeInfo() call above -- 
	//
	// From the same 512 x 512 texture map we're going to pull a series of 
	// frames that are 15 x 20.
	// 
	// The frames have been laid out in a column/row fashion starting in the 
	// upper left corner with 5 frames in each of the 2 rows.
	//
	// The total frame count is 10 because all rows are fully complete and 
	// contain 5 frames with none of them being short any frames.
	//
	// Finally, unlike the donut sprites, the first frame for our number 
	// sprites is not located in the upper left hand corner. This means we 
	// need to provide an x/y offset measured in pixels from the upper left
	// corner so the code that generates the correct texture coordinates for 
	// the sprite can find the first frame.
	//
	//-------------------------------------------------------------------------

	while(!glfwWindowShouldClose(window))
	{
		if (g_bFirstRendering == true)
		{
			g_dLastTime = g_dCurTime = glfwGetTime();
			g_bFirstRendering = false;
		}

		g_dCurTime = glfwGetTime();
		g_fElpasedTime = (float)(g_dCurTime - g_dLastTime);
		g_dLastTime = g_dCurTime;

		//
		// ѕриводим спрайт-пончик в движение, чтобы мы могли протестировать методы
		// класса. 
		//

		static glm::vec2 velocity = glm::vec2(5.0f, 5.0f);
		static float widthRate = 2.0f;
		static float heightRate = 2.0f;
				
		g_donutSprite.setPosition(glm::vec2(g_donutSprite.getPosition().x + (velocity.x * g_fElpasedTime), g_donutSprite.getPosition().y + (velocity.y * g_fElpasedTime)));
		g_donutSprite.setSize(glm::vec2(g_donutSprite.getSize().x + (widthRate * g_fElpasedTime), g_donutSprite.getSize().y + (heightRate * g_fElpasedTime)));

		if (g_donutSprite.getPosition().x > 5.0f || g_donutSprite.getPosition().x < -5.0f)
			velocity.x *= -1.0f; // // инвертировать направление вдоль оси x

		if (g_donutSprite.getPosition().y > 3.0f || g_donutSprite.getPosition().y < -3.0f)
			velocity.y *= -1.0f; // инвертировать направление вдоль оси y

		if (g_donutSprite.getSize().x > 5.0f || g_donutSprite.getSize().x < 1.0f)
			widthRate *= -1.0f; // инвертировать изменение ширины

		if (g_donutSprite.getSize().y > 5.0f || g_donutSprite.getSize().y < 1.0f)
			heightRate *= -1.0f; // инвертировать изменение высоты


		// ¬ыполним проверку границ и убедимс€, что спрайт не исчез с экрана во врем€ обновлени€
		if (g_donutSprite.getPosition().x > 5.0f && g_donutSprite.getPosition().y > 3.0f)
			g_donutSprite.setPosition(glm::vec2(5.0f, 3.0f));

		if (g_donutSprite.getPosition().x < -5.0f && g_donutSprite.getPosition().y < -3.0f)
			g_donutSprite.setPosition(glm::vec2(-5.0f, -3.0f));

		//
		// Render...
		//

		glClearBufferfv(GL_COLOR, 0, &glm::vec4(0.35f, 0.53f, 0.7f, 1.0f)[0]);
		glClearBufferfv(GL_DEPTH, 0, &glm::vec4(1.0f)[0]);

		//
		// Finally, we render our two animated sprites as overlays...
		//
		g_numberSprite.render();
		g_donutSprite.render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}