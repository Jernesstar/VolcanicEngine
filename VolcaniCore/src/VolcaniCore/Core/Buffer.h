#pragma once

#include <cstring>

#include "Core/Defines.h"

namespace VolcaniCore {

template<typename T>
class Buffer {
public:
	Buffer() = default;

	Buffer(uint64_t maxCount)
		: m_MaxCount(maxCount), m_Count(0)
	{
		if(maxCount != 0)
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
	Buffer(T* buffer, uint64_t maxCount = 0)
		: m_MaxCount(maxCount), m_Count(0)
	{
		m_Data = (T*)buffer;
	}
	Buffer(const List<T>& list)
		: m_MaxCount(list.size()), m_Count(list.size())
	{
		m_Data = new T[m_MaxCount];
		memcpy(m_Data, list.data(), GetSize());
	}

	~Buffer() {
		if(m_MaxCount != 0) // We do in fact own this pointer
			Delete();
	}

	Buffer& operator =(const Buffer& other) = delete;

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
		return Buffer<T>(*this);
	}

	Buffer<T> Partition(uint32_t count = 0) {
		if(count == 0)
			count = m_MaxCount;
		return Buffer<T>(m_Data + (m_Count += count), 0);
	}

	void Add(const T& element) {
		if(m_MaxCount != 0 && m_Count >= m_MaxCount)
			Reallocate(100);

		m_Data[m_Count++] = element;
	}
	void Add(const Buffer& buffer) {
		if(m_MaxCount != 0 && m_Count + buffer.GetCount() >= m_MaxCount)
			Reallocate(buffer.GetCount());

		memcpy(m_Data + m_Count, buffer.Get(), buffer.GetSize());
		m_Count += buffer.GetCount();
	}
	void Add(const List<T>& list) {
		if(m_MaxCount != 0 && m_Count + list.size() >= m_MaxCount)
			Reallocate(list.size());

		memcpy(m_Data + m_Count, list.data(), list.size() * sizeof(T));
		m_Count += list.size();
	}
	void Add(void* data, uint64_t count) {
		memcpy(m_Data + m_Count, data, count * sizeof(T));
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

	// TODO(Implement): Reference counting
	uint32_t m_RefCount = 0;
};

template<>
class Buffer<void> {
public:
	Buffer() = default;

	Buffer(uint64_t size, uint32_t maxCount)
		: m_SizeT(size), m_MaxCount(maxCount)
	{
		m_Data = malloc(m_SizeT * m_MaxCount);
	}
	Buffer(const Buffer& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		m_Data = malloc(m_MaxCount);
		memcpy(m_Data, other.Get(), other.GetSize());
	}
	Buffer(Buffer&& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		std::swap(m_Data, other.m_Data);
	}
	Buffer(void* buffer, uint64_t maxCount = 0)
		: m_MaxCount(maxCount), m_Count(0)
	{
		m_Data = (void*)buffer;
	}

	// Buffer(const List<T>& list)
	// 	: m_MaxCount(list.size()), m_Count(list.size())
	// {
	// 	m_Data = malloc(m_MaxCount);
	// 	memcpy(m_Data, list.data(), GetSize());
	// }

	~Buffer() {
		if(m_MaxCount != 0) // We do in fact own this pointer
			Delete();
	}

	Buffer& operator =(const Buffer& other) = delete;

	Buffer& operator =(Buffer&& other) {
		std::swap(m_Data, other.m_Data);
		m_MaxCount = other.m_MaxCount;
		m_Count = other.m_Count;
		return *this;
	}

	void* Get() const { return m_Data; }

	// List<T> GetList() const {
	// 	return List<T>(m_Data, m_Data + m_Count);
	// }

	// T& operator [](uint64_t index) const { return m_Data[index]; }

	uint64_t GetCount()	   const { return m_Count; }
	uint64_t GetMaxCount() const { return m_MaxCount; }
	uint64_t GetSize()	   const { return m_Count	 * m_SizeT; }
	uint64_t GetMaxSize()  const { return m_MaxCount * m_SizeT; }

	Buffer<void> Copy() {
		return Buffer<void>(*this);
	}

	Buffer<void> Partition(uint32_t count = 0) {
		if(count == 0)
			count = m_MaxCount;
		return Buffer(m_Data + (m_Count += count), 0);
	}

	// void Add(const T& element) {
	// 	if(m_MaxCount != 0 && m_Count >= m_MaxCount)
	// 		Reallocate(100);

	// 	m_Data[m_Count++] = element;
	// }
	void Add(const Buffer& buffer) {
		if(m_MaxCount != 0 && m_Count + buffer.GetCount() >= m_MaxCount)
			Reallocate(buffer.GetCount());

		memcpy(m_Data + m_Count, buffer.Get(), buffer.GetSize());
		m_Count += buffer.GetCount();
	}
	// void Add(const List<T>& list) {
	// 	if(m_MaxCount != 0 && m_Count + list.size() >= m_MaxCount)
	// 		Reallocate(list.size());

	// 	memcpy(m_Data + m_Count, list.data(), list.size() * m_SizeT);
	// 	m_Count += list.size();
	// }
	void Add(void* data, uint64_t count) {
		memcpy(m_Data + m_Count, data, count * m_SizeT);
	}

	void Clear() {
		m_Count = 0;
	}

	void Delete() {
		free(m_Data);
		m_Count = 0;
		m_MaxCount = 0;
	}

	void Reallocate(uint64_t surplus) {
		m_MaxCount += surplus;

		void* newData = malloc(m_MaxCount);
		memcpy(newData, m_Data, GetSize());
		free(m_Data);
		m_Data = newData;
	}

	// using iterator = void*;
	// using const_iterator = const void*;
	// iterator begin() { return m_Data; }
	// iterator end() { return m_Data + m_Count; }
	// const_iterator cbegin() const { return m_Data; }
	// const_iterator cend()	const { return m_Data + m_Count; }
	// const_iterator begin()	const { return cbegin(); }
	// const_iterator end()	const { return cend(); }

private:
	void* m_Data = nullptr;
	uint64_t m_SizeT = 0;
	uint64_t m_MaxCount = 0;
	uint64_t m_Count = 0;

	// TODO(Implement): Reference counting
	uint32_t m_RefCount = 0;
};

}