#pragma once

class Options
{
private:
	Options() {}

public:
	static void Init() {
		Options::shadow_width = 2048;
		Options::shadow_height = 2048;
		Options::soft_shadow = false;
	}

	static int shadow_width;
	static int shadow_height;
	static bool soft_shadow;
};



