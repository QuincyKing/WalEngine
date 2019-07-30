#include "./game/layered.h"
#include "./src/render/RenderEngine.h"

int screen_width, screen_height;
std::shared_ptr<Layered> game;

#include "./src/core/Quaternion.h"

void GetDesktopResolution(int& horizontal, int& vertical)
{
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);
	horizontal = desktop.right;
	vertical = desktop.bottom;
}

int main()
{
	GetDesktopResolution(screen_width, screen_height);
	Window window(screen_width, screen_height);
	RenderEngine renderer(window);
	game = std::make_shared<Layered>();
	window.mUpdateFun = [&renderer]() { game->render(renderer); };
	window.mInitFun = []() { game->init(); };
	renderer.precomputeEvent.push_back([]() { game->precompute(); });
	renderer.precompute();
	window.onrun();
	
	return 0;
}