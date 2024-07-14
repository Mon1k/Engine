#pragma once

class Options
{
private:
	Options() {}

public:
	static void Init() {
		Options::shadow_width = 2048;
		Options::shadow_height = 2048;

		Options::shadow_depth = 1000.0f;
		Options::shadow_near = 0.1f;
		Options::soft_shadow = true;
		Options::shadow_enabled = true;

		Options::full_screen = false;
		Options::screen_width = 1920;
		Options::screen_height = 1080;

		Options::reflectionLevel = 0; // 0 - all, 1 - only objects, 2 - disable
		Options::screen_depth = 1000.0f;
		Options::screen_near = 0.1f;
		Options::screen_vsync = false;
	}

	static int shadow_width;
	static int shadow_height;
	static float shadow_near;
	static float shadow_depth;
	static bool soft_shadow;
	static bool shadow_enabled;
	
	static bool full_screen;
	static int screen_width;
	static int screen_height;

	static unsigned int reflectionLevel;
	static float screen_depth;
	static float screen_near;
	static bool screen_vsync;
};