#include "Window.h"
#include <iostream>

Input Window::Inputs = Input();
float Window::deltaTime = 0.0f;
float Window::lastFrame = 0.0f;
Camera Window::MainCamera = Camera();

Window::Window(unsigned int _Width, unsigned int _Height, unsigned int _posX,
	unsigned int _posY, int _multisample, std::string _title )
{
	mProInfo.title = _title;
	mProInfo.posX = _posX;
	mProInfo.posY = _posY;
	multisample = _multisample;
	Inputs.set_win_size(_Width, _Height);

	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
	}

	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
	mWindow = glfwCreateWindow(Inputs.get_win_size_x(), Inputs.get_win_size_y(), mProInfo.title.c_str(), nullptr, nullptr);
	if (mWindow == NULL)
	{
		std::cerr << "Failed to create GLFW Triangle" << std::endl;
		glfwTerminate();
	}
	glfwSetWindowPos(mWindow, mProInfo.posX, mProInfo.posY);
	glfwMakeContextCurrent(mWindow);

	glfwSetInputMode(mWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_SAMPLES, multisample);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	
	glfwSetWindowPos(mWindow, mProInfo.posX, mProInfo.posY);
	glfwMakeContextCurrent(mWindow);
	glfwSwapInterval(1); // Enable vsync
}

void Window::bind_render_target() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, Inputs.get_win_size_x(), Inputs.get_win_size_y());
}

int Window::onrun()
{
	const char* glsl_version = "#version 460";

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(mWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImGuiIO& io = ImGui::GetIO();
	ImFont* pFont = io.Fonts->AddFontFromFileTTF("../res/font/yuan.ttf", 16.0f);
	ImGui::StyleColorsDark();

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	auto scroll = [](GLFWwindow* w, double x, double y)
	{
		static_cast<Window*>(glfwGetWindowUserPointer(w))->_scroll(x, y);
	};

	auto framebuffersize = [](GLFWwindow* w, int x, int y)
	{
		static_cast<Window*>(glfwGetWindowUserPointer(w))->_framebuffersize(x, y);
	};

	auto key = [](GLFWwindow* w, int key, int scancode, int action, int mods)
	{
		static_cast<Window*>(glfwGetWindowUserPointer(w))->_key(key, scancode, action, mods);
	};

	auto mouse = [](GLFWwindow* w, int button, int action, int mods)
	{
		static_cast<Window*>(glfwGetWindowUserPointer(w))->_mouse(button, action, mods);
	};

	//auto cursor = [](GLFWwindow* w, double x, double y)
	//{
	//	static_cast<Window*>(glfwGetWindowUserPointer(w))->_cursor_pos(x, y);
	//};

	//注册事件函数
	glfwSetScrollCallback(mWindow, scroll);
	glfwSetFramebufferSizeCallback(mWindow, framebuffersize);
	glfwSetKeyCallback(mWindow, key);
	glfwSetMouseButtonCallback(mWindow, mouse);
	//glfwSetCursorPosCallback(mWindow, cursor);

	//初始化摄像机
	Quaternion cameraRot = Quaternion(0, 0, 0, 1.0);
	glm::vec3  cameraPos = glm::vec3(5.0f, 2.0f, 15.0f);
	std::shared_ptr<Transform> cameraT = std::make_shared<Transform>(cameraPos, cameraRot);
	MainCamera.set_transform(cameraT);

	//初始化函数
	if(mInitFun) mInitFun();

	while (!glfwWindowShouldClose(mWindow))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//控制程序关闭
		if (Inputs.get_key_down(Input::KEY_ESCAPE))
			glfwSetWindowShouldClose(mWindow, true);

		glfwPollEvents();

		double x, y;
		glfwGetCursorPos(mWindow, &x, &y);
		if (Inputs.get_mouse_y() == -1 && Inputs.get_mouse_x() == -1)
		{
			Inputs.set_mouse_x(x);
			Inputs.set_mouse_y(y);
		}

		Inputs.set_offset_x(x - Inputs.get_mouse_x());
		Inputs.set_offset_y(Inputs.get_mouse_y() - y); // reversed since y-coordinates go from bottom to top

		Inputs.set_mouse_x(x);
		Inputs.set_mouse_y(y);

		glm::mat4 projection = glm::perspective
		(
			glm::radians(Camera::Zoom),
			Inputs.get_win_size_y() != 0 ?
			(float)Inputs.get_win_size_x() / (float)Inputs.get_win_size_y() : 0,
			Camera::Near,
			Camera::Far
		);
		MainCamera.set_projection(projection);

		//按键事件
		if(mKeyFun) mKeyFun();

		//UI事件
		_gui();
		if(mGuiFun) mGuiFun();

		glfwMakeContextCurrent(mWindow);
		glfwGetFramebufferSize(mWindow, &Inputs.get_win_size_x(), &Inputs.get_win_size_y());
		glViewport(0, 0, Inputs.get_win_size_x(), Inputs.get_win_size_y());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//渲染事件
		_update();
		if(mUpdateFun) mUpdateFun();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwMakeContextCurrent(mWindow);
		glfwSwapBuffers(mWindow);
	}
	//关闭函数
	if(mDisableFun) mDisableFun();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwDestroyWindow(mWindow);
	glfwTerminate();

	return 0;
}

//鼠标滚动
void Window::_scroll(double xoffset, double yoffset)
{
	if (yoffset < 0.0)
		Inputs.set_mouse_down(Input::MOUSE_SCROLL_DOWN, true);
	else if (yoffset > 0.0)
		Inputs.set_mouse_down(Input::MOUSE_SCROLL_UP, true);
}

//窗口变化
void Window::_framebuffersize(int width, int height)
{
	glViewport(0, 0, width, height);
}

//GUI
void Window::_gui()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}

void Window::_update()
{
	if (Inputs.get_mouse_down(Input::MOUSE_SCROLL_UP))
	{
		MainCamera.move(glm::vec3(0.0, 0.0, 1.0));
		Inputs.set_mouse_down(Input::MOUSE_SCROLL_UP, false);
	}
	if (Inputs.get_mouse_down(Input::MOUSE_SCROLL_DOWN))
	{
		MainCamera.move(glm::vec3(0.0, 0.0, -1.0));
		Inputs.set_mouse_down(Input::MOUSE_SCROLL_DOWN, false);
	}
	if (Inputs.get_mouse(Input::MOUSE_BUTTON_MIDDLE))
	{
		if(fabs(Inputs.get_offset_x()) + fabs(Inputs.get_offset_y()) > 1.0)
			MainCamera.move(glm::normalize(glm::vec3(Inputs.get_offset_x(), Inputs.get_offset_y(), 0.0)));
	}
	//if (Inputs.get_mouse(Input::MOUSE_BUTTON_RIGHT))
	//{
	//	if (fabs(Inputs.get_offset_x()) > 4.0)
	//		MainCamera.rotate(glm::vec3(0.0, 1.0, 0.0), Inputs.get_offset_x() * 0.01);
	//	if(fabs(Inputs.get_offset_y()) > 4.0)
	//		MainCamera.rotate(glm::vec3(1.0, 0.0, 0.0), Inputs.get_offset_x() * 0.01);
	//}
	//if (Inputs.get_key(Input::KEY_W))
	//	MainCamera.ProcessKeyboard(FORWARD, 0.01f);
	//if (Inputs.get_key(Input::KEY_S))
	//	MainCamera.ProcessKeyboard(BACKWARD, 0.01f);
	//if (Inputs.get_key(Input::KEY_A))
	//	MainCamera.ProcessKeyboard(LEFT, 0.01f);
	//if (Inputs.get_key(Input::KEY_D))
	//	MainCamera.ProcessKeyboard(RIGHT, 0.01f);
}

//键盘按键
void Window::_key(int key, int scancode, int action, int mods)
{
	for (int i = 0; i < Input::NUM_KEYS; i++)
	{
		Inputs.set_key_down(i, false);
		Inputs.set_key_up(i, false);
	}

	if (key != GLFW_KEY_UNKNOWN && action == GLFW_PRESS)
	{
		Inputs.set_key(key, true);
		Inputs.set_key_down(key, true);
	}
	if (key != GLFW_KEY_UNKNOWN && action == GLFW_RELEASE)
	{
		Inputs.set_key(key, false);
		Inputs.set_key_up(key, true);
	}
}

//鼠标按键
void Window::_mouse(int button, int action, int mods)
{
	for (int i = 0; i < Input::NUM_MOUSEBUTTONS; i++)
	{
		Inputs.set_mouse_down(i, false);
		Inputs.set_mouse_up(i, false);
	}

	if (action == GLFW_PRESS)
	{
		Inputs.set_mouse(button, true);
		Inputs.set_mouse_down(button, true);
	}
	if (action == GLFW_RELEASE)
	{
		Inputs.set_mouse(button, false);
		Inputs.set_mouse_up(button, true);
	}
}
