#pragma once

#include <cstring>

#include "Core/Defines.h"

namespace VolcaniCore {

template<typename T>
class Buffer {
public:
	Buffer() = default;

	Buffer(uint32_t count)
		: m_MaxCount(count), m_Index(0)
	{
		m_Data = new T[count];
	}
	Buffer(const List<T>& list)
		: m_MaxCount(list.size()), m_Index(list.size())
	{
		m_Data = new T[m_MaxCount];
		memcpy(m_Data, list.data(), GetSize());
	}
	Buffer(T* buffer, uint32_t count, uint32_t index = 0)
		: m_MaxCount(count), m_Index(index)
	{
		m_Data = buffer;
	}
	Buffer(void* buffer, uint32_t count, uint32_t index = 0)
		: m_MaxCount(count), m_Index(index)
	{
		m_Data = (T*)buffer;
	}
	Buffer(const Buffer&) = default;
	Buffer(Buffer&&) = default;
	~Buffer() = default;
	Buffer& operator =(const Buffer&) = default;

	T* Get() const { return m_Data; }

	uint32_t GetCount()	   const { return m_Index; }
	uint32_t GetMaxCount() const { return m_MaxCount; }
	uint32_t GetSize()	   const { return m_Index * sizeof(T); }
	uint32_t GetMaxSize()  const { return m_MaxCount * sizeof(T); }

	void Add(const T& element) {
		if(m_Index >= m_MaxCount)
			Reallocate(50);

		m_Data[m_Index++] = element;
	}
	void Add(const Buffer& buffer) {
		m_Index += buffer.GetCount();
		if(m_Index >= m_MaxCount)
			Reallocate(buffer.GetCount());

		memcpy(m_Data + buffer.GetCount(), buffer.Get(), buffer.GetSize());
	}
	void Add(const List<T>& list) {
		m_Index += list.size();
		if(m_Index >= m_MaxCount)
			Reallocate(list.size());

		memcpy(m_Data + list.size(), list.data(), list.size() * sizeof(T));
	}

	void Clear() {
		m_Index = 0;
	}

	void Delete() {
		delete m_Data;
	}

	void Reallocate(uint32_t surplus) {
		m_MaxCount += surplus;

		T* newData = new T[m_MaxCount];
		memcpy(newData, m_Data, GetSize());
		delete m_Data;
		m_Data = newData;
	}

	// List<T>::iterator() begin() const {

	// }
	// List<T>::iterator() end() const {

	// }

private:
	T* m_Data;
	uint32_t m_MaxCount;
	uint32_t m_Index;
};

}