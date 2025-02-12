#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

class Window
{
	public:
		Window(short width, short height);
		~Window();

		GLFWwindow* getGLFWWindow()		{ return window;		}
		GLfloat		getXChange();
		GLfloat		getYChange();
		GLint		getWidth()			{ return width;			}
		GLfloat		getMouseX()			{ return lastX;			}	
		GLfloat		getMouseY()			{ return lastY;			}
		GLint		getHeight()			{ return height;		}
		GLfloat&	getScrollValue()	{ return scrollValue;	}

		void resetXYChange();

		void keepCursorInWindow();
		void fullScreen();
		void createCallbacks();
		
		// Fonction de rappel pour les erreurs GLFW
		static void glfwErrorCallback(int error, const char* description);		
		static void handleMouse(GLFWwindow* window, double xPos, double yPos);
		static void handleScroll(GLFWwindow* window, double xOffset, double yOffset);
		

	private:
		GLFWwindow* window = nullptr;
		GLint       width  , height;
		GLfloat     xChange, yChange, lastX, lastY, scrollValue;
		bool		mouseFirstMoved;

		
};

