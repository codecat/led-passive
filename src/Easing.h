#pragma once

namespace easing
{
	enum class type
	{
		linear,

		inQuad,
		inCubic,
		inQuart,
		inQuint,
		inSine,
		inExpo,
		inCircle,
		inElastic,
		inBack,
		inBounce,

		outQuad,
		outCubic,
		outQuart,
		outQuint,
		outSine,
		outExpo,
		outCircle,
		outElastic,
		outBack,
		outBounce,

		inOutQuad,
		inOutCubic,
		inOutQuart,
		inOutQuint,
		inOutSine,
		inOutExpo,
		inOutCircle,
		inOutElastic,
		inOutBack,
		inOutBounce,

		_count,
	};

	inline int getTypeCount() { return (int)type::_count; }
	const char* getTypeName(type type);
	type parseTypeName(const char* name);

	double transform(type type, double x);
	float transform(type type, float x);
}
