#pragma once

class Options
{
private:
	Options() {}

public:
	static void Init() {
		Options::shadow_width = 2048;
		Options::shadow_height = 2048;

		Options::shadow_depth = 50.0f;
		Options::shadow_near = 1.0f;
		Options::soft_shadow = true;
		Options::shadow_enabled = true;

		Options::full_screen = false;
		Options::screen_width = 1980;
		Options::screen_height = 1080;

		Options::reflectionLevel = 0;
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
};