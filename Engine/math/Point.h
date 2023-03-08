#pragma once

namespace ds {
	namespace math {

		class Point
		{
		public:
			Point() {
			};
			Point(float _x, float _y) {
				x = _x;
				y = _y;
			}

		public:
			float x, y;
		};

	};

};