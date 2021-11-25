#include <Common.h>
#include <Buffer.h>

Buffer::Buffer()
{
	EnsureMemory(100);
}

Buffer::Buffer(void* buffer, size_t size)
{
	Write(buffer, size);
	SetPos(0);
}

Buffer::~Buffer()
{
	if (m_buffer != nullptr) {
		free(m_buffer);
	}
}

void Buffer::Clear()
{
	m_size = 0;
	m_pos = 0;
}

size_t Buffer::GetPos() const
{
	return m_pos;
}

void Buffer::SetPos(size_t pos)
{
	m_pos = pos;
}

void Buffer::Write(const void* p, size_t s)
{
	EnsureMemory(m_size + s);
	memcpy((uint8_t*)m_buffer + m_pos, p, s);
	m_pos += s;
	m_size += s;
}

size_t Buffer::Read(void* p, size_t s)
{
	size_t readable = m_size - m_pos;
	if (s > readable) {
		s = readable;
	}
	if (s > 0) {
		memcpy(p, (uint8_t*)m_buffer + m_pos, s);
		m_pos += s;
	}
	return readable;
}

s2::string Buffer::ReadString(size_t size)
{
	char* buffer = (char*)alloca(size);
	return s2::string(buffer, size);
}

s2::string Buffer::ReadStringWithLength()
{
	auto len = Read<uint32_t>();
	return ReadString(len);
}

void Buffer::WriteString(const char* str)
{
	Write(str, strlen(str));
}

void Buffer::WriteStringWithLength(const char* str)
{
	size_t len = strlen(str);
	Write((uint32_t)len);
	Write(str, len);
}

void* Buffer::GetBuffer()
{
	return m_buffer;
}

const void* Buffer::GetBuffer() const
{
	return m_buffer;
}

size_t Buffer::GetSize() const
{
	return m_size;
}

void Buffer::EnsureMemory(size_t size)
{
	if (m_buffer == nullptr || size > m_allocated) {
		size_t resize = m_allocated + m_allocated / 2;
		if (resize < SIZE_MAX && resize > size) {
			size = resize;
		}
		m_buffer = realloc(m_buffer, size);
	}
}
