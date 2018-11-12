#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <windows.h>
#include <vector>
#include <iostream>
#include <algorithm>

#include "../render/Camera.h"
#include "../editor/Panel.h"
#include "Input.h"

class Window
{
public:
	struct ProInfo
	{
		std::string title;
		int posX;
		int posY;
	}mProInfo;
	int multisample;
	Panel mPanel;
	Camera mCamera;
	static Input mInput;

	float deltaTime;
	float lastFrame;

protected:
	GLFWwindow* mWindow;
	
public:
	Window(unsigned int _Width, unsigned int _Height, unsigned int _posX = 0,
		unsigned int _posY = 30, int _multisample = 1, std::string _title = "WalnutEngine");
	Window() = default;
	virtual ~Window() {}

	int onrun();
	virtual void oninit() {}
	virtual void ongui() {}
	virtual void onupdate() {}
	virtual void ondisable() {}
	virtual void onkey() {}

	GLFWwindow* get_window() { return mWindow; }

private:
	void _update();
	void _gui(); 
	void _key(int key, int scancode, int action, int mods);
	void _mouse(int button, int action, int mods);
	void _scroll(double xoffset, double yoffset);
	void _framebuffersize(int width, int height);
};

