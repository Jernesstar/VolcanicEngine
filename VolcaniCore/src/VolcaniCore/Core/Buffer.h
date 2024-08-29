#pragma once

#include <cstdint>

namespace VolcaniCore {

template<typename T>
class Buffer {
public:
	Buffer() = default;

	Buffer(uint32_t count)
		: m_Count(count)
	{
		m_Data = new T[count];
		m_LastElement = 0;
	}
	Buffer(const Buffer&) = default;
	Buffer(Buffer&&) = default;
	~Buffer() = default;

	Buffer& operator =(const Buffer&) = default;

	T* Get() { return m_Data; }
	uint32_t GetCount() const { return m_LastElement; }
	uint32_t GetMaxCount() const { return m_Count; }
	uint32_t GetSize() const { return m_Count * sizeof(T); }

	void Add(const T& element) {
		if(m_LastElement >= m_Count)
			Reallocate();

		m_Data[m_LastElement] = element;
		m_LastElement++;
	}

	void Clear() {
		m_LastElement = 0;
	}

	void Delete() {
		delete m_Data;
	}

private:
	T* m_Data;
	uint32_t m_Count;
	uint32_t m_LastElement;

private:
	void Reallocate() {

	}
};

}