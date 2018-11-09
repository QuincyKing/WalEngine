#include "pbr.h"

int screen_width, screen_height;
std::shared_ptr<Window> pro;

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
	pro = std::make_shared<Pbr>(screen_width, screen_height);
	pro->onrun();
	return 0;
}

