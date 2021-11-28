#include <Common.h>
#include <Easing.h>

const double PI = 3.14159265358979323846;

const char* easing::getTypeName(type type)
{
	switch (type) {
		case easing::type::linear: return "linear";

		case easing::type::inQuad: return "inQuad";
		case easing::type::inCubic: return "inCubic";
		case easing::type::inQuart: return "inQuart";
		case easing::type::inQuint: return "inQuint";
		case easing::type::inSine: return "inSine";
		case easing::type::inExpo: return "inExpo";
		case easing::type::inCircle: return "inCircle";
		case easing::type::inElastic: return "inElastic";
		case easing::type::inBack: return "inBack";
		case easing::type::inBounce: return "inBounce";

		case easing::type::outQuad: return "outQuad";
		case easing::type::outCubic: return "outCubic";
		case easing::type::outQuart: return "outQuart";
		case easing::type::outQuint: return "outQuint";
		case easing::type::outSine: return "outSine";
		case easing::type::outExpo: return "outExpo";
		case easing::type::outCircle: return "outCircle";
		case easing::type::outElastic: return "outElastic";
		case easing::type::outBack: return "outBack";
		case easing::type::outBounce: return "outBounce";

		case easing::type::inOutQuad: return "inOutQuad";
		case easing::type::inOutCubic: return "inOutCubic";
		case easing::type::inOutQuart: return "inOutQuart";
		case easing::type::inOutQuint: return "inOutQuint";
		case easing::type::inOutSine: return "inOutSine";
		case easing::type::inOutExpo: return "inOutExpo";
		case easing::type::inOutCircle: return "inOutCircle";
		case easing::type::inOutElastic: return "inOutElastic";
		case easing::type::inOutBack: return "inOutBack";
		case easing::type::inOutBounce: return "inOutBounce";
	}
	return "???";
}

easing::type easing::parseTypeName(const char* name)
{
	for (int i = 0; i < (int)type::_count; i++) {
		if (!strcmp(getTypeName((type)i), name)) {
			return (type)i;
		}
	}
	return type::linear;
}

double easing::transform(type type, double x)
{
	switch (type) {
		case easing::type::linear: return x;

		case easing::type::inQuad: return x * x;
		case easing::type::inCubic: return x * x * x;
		case easing::type::inQuart: return x * x * x * x;
		case easing::type::inQuint: return x * x * x * x * x;
		case easing::type::inSine: return -cos(x * (PI / 2)) + 1;
		case easing::type::inExpo: return (x == 0) ? 0 : pow(2, 10 * (x - 1));
		case easing::type::inCircle: return -(sqrt(1 - x * x) - 1);
		case easing::type::inElastic: {
			double s = 0.075;
			if (x == 0) return 0;
			if (x == 1) return 1;
			return -(pow(2, 10 * (x -= 1)) * sin((x - s) * (2 * PI) / 0.3));
		}
		case easing::type::inBack: {
			double s = 1.70158;
			return x * x * ((s + 1) * x - s);
		}
		case easing::type::inBounce: return 1 - transform(type::outBounce, 1 - x);

		case easing::type::outQuad: return -x * (x - 2);
		case easing::type::outCubic: return (x -= 1) * x * x + 1;
		case easing::type::outQuart: return -((x -= 1) * x * x * x - 1);
		case easing::type::outQuint: return (x -= 1) * x * x * x * x + 1;
		case easing::type::outSine: return sin(x * (PI / 2));
		case easing::type::outExpo: return (x == 1) ? 1 : -pow(2, -10 * x) + 1;
		case easing::type::outCircle: return sqrt(1 - (x -= 1) * x);
		case easing::type::outElastic: {
			double s = 0.075;
			if (x == 0) return 0;
			if (x == 1) return 1;
			return pow(2, -10 * x) * sin((x - s) * (2 * PI) / 0.3) + 1;
		}
		case easing::type::outBack: {
			double s = 1.70158;
			return (x -= 1) * x * ((s + 1) * x + s) + 1;
		}
		case easing::type::outBounce:
			if (x < (1 / 2.75)) {
				return 7.5625 * x * x;
			} else if (x < (2 / 2.75)) {
				return 7.5625 * (x -= (1.5 / 2.75)) * x + 0.75;
			} else if (x < (2.5 / 2.75)) {
				return 7.5625 * (x -= (2.25 / 2.75)) * x + 0.9375;
			} else {
				return 7.5625 * (x -= (2.625 / 2.75)) * x + 0.984375;
			}

		case easing::type::inOutQuad:
			if ((x /= 0.5) < 1) return 0.5 * x * x;
			return -0.5 * ((--x) * (x - 2) - 1);
		case easing::type::inOutCubic:
			if ((x /= 0.5) < 1) return 0.5 * x * x * x;
			return 0.5 * ((x -= 2) * x * x + 2);
		case easing::type::inOutQuart:
			if ((x /= 0.5) < 1) return 0.5 * x * x * x * x;
			return -0.5 * ((x -= 2) * x * x * x - 2);
		case easing::type::inOutQuint:
			if ((x /= 0.5) < 1) return 0.5 * x * x * x * x * x;
			return 0.5 * ((x -= 2) * x * x * x * x + 2);
		case easing::type::inOutSine:
			return -0.5 * (cos(PI * x) - 1);
		case easing::type::inOutExpo:
			if (x == 0) return 0;
			if (x == 1) return 1;
			if ((x /= 0.5) < 1) return 0.5 * pow(2, 10 * (x - 1));
			return 0.5 * (-pow(2, -10 * --x) + 2);
		case easing::type::inOutCircle:
			if ((x /= 0.5) < 1) return -0.5 * (sqrt(1 - x * x) - 1);
			return 0.5 * (sqrt(1 - (x -= 2) * x) + 1);
		case easing::type::inOutElastic: {
			double s = 0.1125;
			if (x == 0) return 0;
			if ((x /= 0.5) == 2) return 1;
			if (x < 1) return -0.5 * (pow(2, 10 * (x -= 1)) * sin((x - s) * (2 * PI) / 0.45));
			return pow(2, -10 * (x -= 1)) * sin((x - s) * (2 * PI) / 0.45) * 0.5 + 1;
		}
		case easing::type::inOutBack: {
			double s = 1.70158;
			if ((x /= 0.5) < 1) return 0.5 * (x * x * (((s *= 1.525) + 1) * x - s));
			return 0.5 * ((x -= 2) * x * (((s *= 1.525) + 1) * x + s) + 2);
		}
		case easing::type::inOutBounce:
			if (x < 0.5) return transform(type::inBounce, x * 2) * 0.5;
			return transform(type::outBounce, x * 2 - 1) * 0.5 + 0.5;
	}

	return x;
}

float easing::transform(type type, float x)
{
	return (float)transform(type, (double)x);
}
