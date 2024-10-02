#pragma once

#include <cstring>

#include "Core/Defines.h"

namespace VolcaniCore {

template<typename T>
class Buffer {
public:
	Buffer() = default;
	Buffer(uint64_t count)
		: m_MaxCount(count), m_Count(0)
	{
		if(count != 0)
			m_Data = new T[m_MaxCount];
	}
	Buffer(const Buffer& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		m_Data = new T[m_MaxCount];
		memcpy(m_Data, other.Get(), other.GetSize());
	}
	Buffer(Buffer&& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		std::swap(m_Data, other.m_Data);
	}
	Buffer(Buffer<T>& buffer, uint64_t count)
		: m_MaxCount(buffer.GetMaxCount() - count), m_Count(0)
	{
		m_Data = buffer.Get() + count;
	}
	Buffer(const List<T>& list)
		: m_MaxCount(list.size()), m_Count(list.size())
	{
		m_Data = new T[m_MaxCount];
		memcpy(m_Data, list.data(), GetSize());
	}
	Buffer(T* buffer, uint64_t count = 0)
		: m_MaxCount(count), m_Count(count)
	{
		m_Data = (T*)buffer;
	}

	~Buffer() {
		if(m_MaxCount != 0) // We do infact own this point
			Delete();
	}

	Buffer& operator =(const Buffer& other) {
		memcpy(m_Data, other.m_Data, other.GetSize());
		m_MaxCount = other.m_MaxCount;
		m_Count = other.m_Count;
		return *this;
	}

	Buffer& operator =(Buffer&& other) {
		std::swap(m_Data, other.m_Data);
		m_MaxCount = other.m_MaxCount;
		m_Count = other.m_Count;
		return *this;
	}

	T* Get() const { return m_Data; }

	List<T> GetList() const {
		return List<T>(m_Data, m_Data + m_Count);
	}

	T& operator [](uint64_t index) const { return m_Data[index]; }

	uint64_t GetCount()	   const { return m_Count; }
	uint64_t GetMaxCount() const { return m_MaxCount; }
	uint64_t GetSize()	   const { return m_Count	 * sizeof(T); }
	uint64_t GetMaxSize()  const { return m_MaxCount * sizeof(T); }

	Buffer<T> Copy() {
		Buffer<T> newBuffer(m_MaxCount);
		newBuffer.Add(*this);

		return newBuffer;
	}

	Buffer<T>&& Partition(uint32_t count = 0) {
		Buffer<T> newBuffer;
		newBuffer.m_Data = this->m_Data + this->m_Count;
		this->m_Count += count ? count : this->m_MaxCount - this->m_Count;

		return newBuffer;
	}

	void Add(const T& element) {
		if(m_Count >= m_MaxCount)
			Reallocate(100);

		m_Data[m_Count++] = element;
	}
	void Add(const Buffer& buffer) {
		if(m_Count + buffer.GetCount() >= m_MaxCount)
			Reallocate(buffer.GetCount());

		memcpy(m_Data + m_Count, buffer.Get(), buffer.GetSize());
		m_Count += buffer.GetCount();
	}
	void Add(const List<T>& list) {
		if(m_Count + list.size() >= m_MaxCount)
			Reallocate(list.size());

		memcpy(m_Data + m_Count, list.data(), list.size() * sizeof(T));
		m_Count += list.size();
	}

	void Clear() {
		m_Count = 0;
	}

	void Delete() {
		delete m_Data;
		m_Count = 0;
		m_MaxCount = 0;
	}

	void Reallocate(uint64_t surplus) {
		m_MaxCount += surplus;

		T* newData = new T[m_MaxCount];
		memcpy(newData, m_Data, GetSize());
		delete m_Data;
		m_Data = newData;
	}

	using iterator = T*;
	using const_iterator = const T*;
	iterator begin() { return m_Data; }
	iterator end() { return m_Data + m_Count; }
	const_iterator cbegin() const { return m_Data; }
	const_iterator cend()	const { return m_Data + m_Count; }
	const_iterator begin()	const { return cbegin(); }
	const_iterator end()	const { return cend(); }

private:
	T* m_Data = nullptr;
	uint64_t m_MaxCount = 0;
	uint64_t m_Count = 0;
};

}