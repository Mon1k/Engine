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

		Options::soft_shadow = false;
	}

	static int shadow_width;
	static int shadow_height;
	static float shadow_near;
	static float shadow_depth;
	static bool soft_shadow;
};