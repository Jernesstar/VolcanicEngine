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
		m_Data = new T[m_MaxCount];
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

	List<T> GetList() const {
		return List<T>(m_Data, m_Data + m_Index);
	}

	T& operator [](uint32_t index) const { return m_Data[index]; }

	uint32_t GetCount()	   const { return m_Index; }
	uint32_t GetMaxCount() const { return m_MaxCount; }
	std::size_t GetSize()	 const { return m_Index	   * sizeof(T); }
	std::size_t GetMaxSize() const { return m_MaxCount * sizeof(T); }

	void Add(const T& element) {
		if(m_Index >= m_MaxCount)
			Reallocate(100);

		m_Data[m_Index++] = element;
	}
	void Add(const Buffer& buffer) {
		if(m_Index + buffer.GetCount() >= m_MaxCount)
			Reallocate(buffer.GetCount());

		memcpy(m_Data + m_Index, buffer.Get(), buffer.GetSize());
		m_Index += buffer.GetCount();
	}
	void Add(const List<T>& list) {
		if(m_Index + list.size() >= m_MaxCount)
			Reallocate(list.size());

		memcpy(m_Data + m_Index, list.data(), list.size() * sizeof(T));
		m_Index += list.size();
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

	using iterator = T*;
	using const_iterator = const T*;
	const_iterator cbegin() const { return m_Data; }
	const_iterator cend()	const { return m_Data + m_Index; }
	const_iterator begin()	const { return cbegin(); }
	const_iterator end()	const { return cend(); }
	iterator begin() { return m_Data; }
	iterator end() { return m_Data + m_Index; }

private:
	T* m_Data = nullptr;
	uint32_t m_MaxCount = 0;
	uint32_t m_Index = 0;
};

}