#pragma once

#include <Common.h>

namespace cfg
{
	const size_t MAX_STRING_SIZE = 64;

	enum type
	{
		t_null,
		t_int,
		t_float,
		t_string,
	};

	class value
	{
	private:
		type m_type = t_null;
		union {
			int m_int;
			float m_float;
			char m_string[MAX_STRING_SIZE];
		} m_value;

	public:
		inline value() { m_type = t_null; }
		inline value(int v) { m_type = t_int; m_value.m_int = v; }
		inline value(float v) { m_type = t_int; m_value.m_int = v; }
		inline value(const char* v)
		{
			m_type = t_string;
			size_t len = strlen(v);
			assert(len < MAX_STRING_SIZE);
			if (len >= MAX_STRING_SIZE) {
				len = MAX_STRING_SIZE - 1;
			}
			memcpy(m_value.m_string, v, len);
			m_value.m_string[len] = '\0';
		}

		inline type getType() { return m_type; }
		inline int getInt() { return m_value.m_int; }
		inline float getFloat() { return m_value.m_float; }
		inline const char* getString() { return m_value.m_string; }
	};

	class block
	{
	private:
		s2::dict<s2::string, value> m_values;
		s2::dict<s2::string, block*> m_blocks;

	public:
		~block();

		block* getBlock(const char* name);
		int getInt(const char* name, int def = 0);
		float getFloat(const char* name, float def = 0.0f);
		const char* getString(const char* name, const char* def = nullptr);

		void loadFromFile(FILE* fh);
	};

	block* fromFile(const char* filename);
}
