#pragma once

#include <cstring>
#include <initializer_list>

#include "Core/Defines.h"
#include "Core/Assert.h"

namespace VolcaniCore {

template<typename T>
class Buffer {
public:
	Buffer() = default;
	Buffer(uint64_t maxCount)
		: m_MaxCount(maxCount)
	{
		VOLCANICORE_LOG_INFO("Buffer - Count Ctor: %i", maxCount);
		if(maxCount)
			m_Data = (T*)malloc(GetMaxSize());
	}
	Buffer(Buffer&& other)
		: m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		VOLCANICORE_LOG_INFO("Buffer - Move Ctor");
		VOLCANICORE_LOG_INFO("{ %i, %i, %i } vs. { %i, %i, %i }",
			other.GetCount(), other.GetMaxCount(), other.Get(),
			GetCount(), GetMaxCount(), Get());
		std::swap(m_Data, other.m_Data);
	}
	Buffer(const Buffer& other)
		: m_MaxCount(other.GetMaxCount())
	{
		VOLCANICORE_LOG_INFO("Buffer - Copy Ctor: %i", m_MaxCount);
		if(!m_MaxCount)
			return;

		m_Data = (T*)malloc(GetMaxSize());
		Set(other.Get(), other.GetCount());
	}
	Buffer(std::initializer_list<T> list)
		: m_MaxCount(list.size())
	{
		VOLCANICORE_LOG_INFO("Buffer - Init Ctor: %i", m_MaxCount);
		if(!m_MaxCount)
			return;

		m_Data = (T*)malloc(GetMaxSize());
		Set(list.begin(), list.size());
	}
	Buffer(T* data, uint64_t count, uint64_t maxCount = 0)
		: m_Data(data), m_MaxCount(maxCount), m_Count(count)
	{
		VOLCANICORE_LOG_INFO("Buffer - Own Ctor: { %i, %i }", count, maxCount);
		if(!m_MaxCount)
			m_MaxCount = count;
	}

	~Buffer() {
		Delete();
	}

	Buffer& operator =(const Buffer& other) = delete;

	Buffer& operator =(Buffer&& other) {
		std::swap(m_Data, other.m_Data);
		m_MaxCount = other.m_MaxCount;
		m_Count = other.m_Count;
		return *this;
	}

	operator bool() const { return m_Data && m_Count; }

	T* Get(uint64_t i = 0) const {
		VOLCANICORE_ASSERT(i < m_MaxCount);
		return m_Data + i;
	}

	template<typename ...Args>
	void New(uint64_t i, Args&&... args) {
		new ((void*)Get(i)) T(std::forward<Args>(args)...);
	}

	uint64_t GetCount()	   const { return m_Count; }
	uint64_t GetMaxCount() const { return m_MaxCount; }
	uint64_t GetSize()	   const { return m_Count	 * sizeof(T); }
	uint64_t GetMaxSize()  const { return m_MaxCount * sizeof(T); }

	Buffer<T> Copy() const { return Buffer<T>(*this); }

	void Add(const T& element) {
		if(m_Count >= m_MaxCount)
			return;

		m_Data[m_Count++] = element;
	}
	void Add(const Buffer& buffer) {
		Set(buffer.Get(), buffer.GetCount(), m_Count);
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

	void Add() {
		m_Count++;
	}

	void AddRange(uint64_t count) {
		m_Count += count;
	}

	void Remove() {
		m_Count--;
	}

	void Clear() {
		m_Count = 0;
	}

	void Delete() {
		if(!m_Data)
			return;

		free(m_Data);
		m_Data = nullptr;
		m_Count = 0;
		m_MaxCount = 0;
	}

	void Reallocate(uint64_t additional) {
		Allocate(m_MaxCount + additional);
	}

	void Allocate(uint64_t count) {
		if(count <= m_MaxCount)
			return;

		m_MaxCount = count;
		T* newData = (T*)malloc(GetMaxSize());
		memcpy(newData, m_Data, GetSize());
		free(m_Data);
		m_Data = newData;
	}

private:
	T* m_Data = nullptr;
	uint64_t m_MaxCount = 0;
	uint64_t m_Count = 0;
};

template<>
class Buffer<void> {
public:
	Buffer() = default;

	Buffer(uint64_t size, uint64_t maxCount)
		: m_SizeT(size), m_MaxCount(maxCount)
	{
		VOLCANICORE_ASSERT(size != 0);
		if(maxCount)
			m_Data = malloc(GetMaxSize());
	}
	Buffer(Buffer&& other)
		: m_SizeT(other.m_SizeT),
			m_MaxCount(other.GetMaxCount()), m_Count(other.GetCount())
	{
		std::swap(m_Data, other.m_Data);
	}
	Buffer(const Buffer& other)
		: m_SizeT(other.m_SizeT), m_MaxCount(other.GetMaxCount())
	{
		m_Data = malloc(GetMaxSize());
		Set(other.Get(), other.GetCount());
	}

	template<typename T>
	Buffer(const std::vector<T>& list)
		: m_SizeT(sizeof(T)), m_MaxCount(list.size())
	{
		m_Data = malloc(GetMaxSize());
		Set(list.data(), list.size());
	}

	Buffer(void* data, uint64_t size, uint64_t count = 0)
		: m_SizeT(size), m_MaxCount(count), m_Count(count)
	{
		m_Data = data;
	}

	~Buffer() {
		Delete();
	}

	operator bool() const { return m_Data && m_Count; }

	Buffer& operator =(const Buffer& other) = delete;

	Buffer& operator =(Buffer&& other) {
		std::swap(m_Data, other.m_Data);
		m_SizeT = other.m_SizeT;
		m_MaxCount = other.m_MaxCount;
		m_Count = other.m_Count;
		return *this;
	}

	void* Get() const { return m_Data; }

	template<typename T>
	T* Get() const { return (T*)m_Data; }

	uint64_t GetCount()	   const { return m_Count; }
	uint64_t GetMaxCount() const { return m_MaxCount; }
	uint64_t GetSize()	   const { return m_Count	 * m_SizeT; }
	uint64_t GetMaxSize()  const { return m_MaxCount * m_SizeT; }
	uint64_t GetSizeT() const { return m_SizeT; }

	Buffer<void> Copy() {
		return Buffer<void>(*this);
	}

	void Add(const Buffer& buffer) {
		if(m_Count + buffer.GetCount() >= m_MaxCount)
			return;

		Set(buffer.Get(), buffer.GetCount(), m_Count);
	}

	void Add(const void* data, uint64_t count) {
		Set(data, count, m_Count);
	}

	void Set(const void* data, uint64_t count, uint64_t offset = 0) {
		if(offset >= m_MaxCount)
			return;
		if(offset + count >= m_MaxCount)
			count = m_MaxCount - offset;

		memcpy((char*)m_Data + offset * m_SizeT, data, count * m_SizeT);

		if(offset + count > m_Count)
			m_Count = offset + count;
	}

	void Clear() {
		m_Count = 0;
	}

	void Delete() {
		free(m_Data);
		m_Data = nullptr;
		m_SizeT = 0;
		m_MaxCount = 0;
		m_Count = 0;
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
};

}