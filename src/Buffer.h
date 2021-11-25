#pragma once

#include <Common.h>

class Buffer
{
private:
	void* m_buffer = nullptr;
	size_t m_size = 0;
	size_t m_allocated = 0;
	size_t m_pos = 0;

public:
	Buffer();
	Buffer(void* buffer, size_t size);
	~Buffer();

	void Clear();

	size_t GetPos() const;
	void SetPos(size_t pos);

	void Write(const void* p, size_t s);
	size_t Read(void* p, size_t s);

	s2::string ReadString(size_t size);
	s2::string ReadStringWithLength();

	void WriteString(const char* str);
	void WriteStringWithLength(const char* str);

	void* GetBuffer();
	const void* GetBuffer() const;
	size_t GetSize() const;

	template<typename T> void Write(const T& obj) { Write(&obj, sizeof(T)); }
	template<typename T> T Read() { T ret; Read(&ret, sizeof(T)); return ret; }

private:
	void EnsureMemory(size_t size);
};
