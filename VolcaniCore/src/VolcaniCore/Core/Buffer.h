#pragma once

#include <cstring>

#include "Core/Defines.h"
#include "Core/Assert.h"

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
	Buffer(Buffer&& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		std::swap(m_Data, other.m_Data);
	}
	Buffer(const Buffer& other)
		: m_MaxCount(other.GetMaxCount())
	{
		m_Data = new T[m_MaxCount];
		Set(other.Get(), other.GetCount());
	}
	Buffer(const List<T>& list)
		: m_MaxCount(list.size())
	{
		m_Data = new T[m_MaxCount];
		Set(list.data(), list.size());
	}
	Buffer(T* buffer, uint64_t count, uint64_t maxCount = 0)
		: m_MaxCount(maxCount), m_Count(count)
	{
		m_Data = buffer;
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

	T& operator [](uint64_t index) const {
		VOLCANICORE_ASSERT(index < m_Count);
		return m_Data[index];
	}

	uint64_t GetCount()	   const { return m_Count; }
	uint64_t GetMaxCount() const { return m_MaxCount; }
	uint64_t GetSize()	   const { return m_Count	 * sizeof(T); }
	uint64_t GetMaxSize()  const { return m_MaxCount * sizeof(T); }

	Buffer<T> Copy() {
		return Buffer<T>(*this);
	}

	Buffer<T> Partition(uint64_t count = 0) {
		if(count == 0)
			count = m_MaxCount;
		return Buffer<T>(m_Data + (m_Count += count), count, 0);
	}

	void Add(const T& element) {
		if(m_MaxCount != 0 && m_Count >= m_MaxCount)
			Reallocate(100);

		m_Data[m_Count++] = element;
	}
	void Add(const Buffer& buffer) {
		if(m_MaxCount != 0 && m_Count + buffer.GetCount() >= m_MaxCount)
			Reallocate(buffer.GetCount());

		Set(buffer.Get(), buffer.GetCount(), m_Count);
	}
	void Add(const List<T>& list) {
		if(m_MaxCount != 0 && m_Count + list.size() >= m_MaxCount)
			Reallocate(list.size());

		Set(list.data(), list.size(), m_Count);
	}
	void Add(const void* data, uint64_t count) {
		Set(data, count, m_Count);
	}

	void Set(const void* data, uint64_t count, uint64_t offset = 0) {
		if(offset >= m_MaxCount)
			return;
		if(offset + count >= m_MaxCount)
			count = m_MaxCount - offset;

		memcpy(m_Data + offset, data, count * sizeof(T));

		if(offset + count > m_Count)
			m_Count = offset + count;
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
	uint64_t m_RefCount = 0;
};

template<>
class Buffer<void> {
public:
	Buffer() = default;

	Buffer(uint64_t maxCount, uint64_t size)
		: m_MaxCount(maxCount), m_Count(0), m_SizeT(size)
	{
		m_Data = malloc(GetMaxSize());
	}
	Buffer(Buffer&& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount()),
			m_SizeT(other.m_SizeT)
	{
		std::swap(m_Data, other.m_Data);
	}
	Buffer(const Buffer& other)
		: m_MaxCount(other.GetMaxCount()), m_SizeT(other.m_SizeT)
	{
		m_Data = malloc(GetMaxSize());
		Set(other.Get(), other.GetCount());
	}

	template<typename T>
	Buffer(const List<T>& list)
		: m_MaxCount(list.size()), m_SizeT(sizeof(T))
	{
		m_Data = malloc(GetMaxSize());
		Set(list.data(), list.size());
	}

	Buffer(void* buffer, uint64_t count, uint64_t size)
		: m_MaxCount(count), m_Count(count), m_SizeT(size)
	{
		m_Data = (void*)buffer;
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

	void* Get() const { return m_Data; }

	uint64_t GetCount()	   const { return m_Count; }
	uint64_t GetMaxCount() const { return m_MaxCount; }
	uint64_t GetSize()	   const { return m_Count	 * m_SizeT; }
	uint64_t GetMaxSize()  const { return m_MaxCount * m_SizeT; }

	Buffer<void> Copy() {
		return Buffer<void>(*this);
	}

	Buffer<void> Partition(uint64_t count = 0) {
		if(count == 0)
			count = m_MaxCount;
		return Buffer(m_Data + (m_Count += count), count, m_SizeT);
	}

	// template<typename T>
	// void Add(const T& element) {
	// 	if(m_MaxCount != 0 && m_Count >= m_MaxCount)
	// 		Reallocate(100);

	// 	memset(m_Data, element, sizeof(T));
	// }
	void Add(const Buffer& buffer) {
		if(m_MaxCount != 0 && m_Count + buffer.GetCount() >= m_MaxCount)
			Reallocate(buffer.GetCount());

		Set(buffer.Get(), buffer.GetCount(), m_Count);
	}
	// void Add(const List<T>& list) {
	// 	if(m_MaxCount != 0 && m_Count + list.size() >= m_MaxCount)
	// 		Reallocate(list.size());

	// 	memcpy(m_Data + m_Count, list.data(), list.size() * m_SizeT);
	// 	m_Count += list.size();
	// }
	void Add(const void* data, uint64_t count) {
		Set(data, count, m_Count);
	}
	void Set(const void* data, uint64_t count, uint64_t offset = 0) {
		if(offset >= m_MaxCount)
			return;
		if(offset + count >= m_MaxCount)
			count = m_MaxCount - offset;

		memcpy(m_Data + offset, data, count * m_SizeT);

		if(offset + count > m_Count)
			m_Count = offset + count;
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

private:
	void* m_Data = nullptr;
	uint64_t m_SizeT = 0;
	uint64_t m_MaxCount = 0;
	uint64_t m_Count = 0;

	// TODO(Implement): Reference counting
	uint64_t m_RefCount = 0;
};

}