#pragma once

#include "Buffer.h"

namespace VolcaniCore {

struct SearchResult {
	bool Found;
	uint64_t Index;

	operator bool() const { return Found; }
};

template<typename T>
class List {
public:
	List()
		: m_Buffer(10) { }
	List(uint64_t size)
		: m_Buffer(size)
	{
		VOLCANICORE_LOG_INFO("Alloc");
	}
	List(const std::initializer_list<T>& list)
		: m_Buffer(list.size())
	{
		for(auto& element : list)
			Add(element);
	}
	List(List&& other)
		: m_Buffer(other.m_Buffer.GetMaxCount())
	{
		m_Front = other.m_Front;
		m_Back = other.m_Back;
		for(uint64_t i = 0; i < other.Count(); i++)
			Add(*other.At(i));
	}
	List(const List& other)
		: m_Buffer(other.m_Buffer.GetMaxCount())
	{
		m_Front = other.m_Front;
		m_Back = other.m_Back;
		for(uint64_t i = 0; i < other.Count(); i++)
			Add(*other.At(i));
	}

	~List() {
		Clear();
		m_Buffer.Delete();
	}

	List& operator =(const List& other) {
		m_Buffer = Buffer<T>(other.m_Buffer.GetMaxCount());
		m_Front = other.m_Front;
		m_Back = other.m_Back;
		for(uint64_t i = 0; i < other.Count(); i++)
			Add(*other.At(i));

		return *this;
	}

	List& operator =(List&& other) {
		m_Buffer = Buffer<T>(other.m_Buffer.GetMaxCount());
		m_Front = other.m_Front;
		m_Back = other.m_Back;
		for(uint64_t i = 0; i < other.Count(); i++)
			Add(*other.At(i));

		return *this;
	}

	operator bool() const { return Count(); }

	T& operator[](int64_t idx) { return *At(idx); }
	const T& operator[](int64_t idx) const { return *At(idx); }

	T* At(int64_t idx) const {
		if(idx < 0)
			return m_Buffer.Get() + (uint64_t)((int64_t)m_Back + idx);

		return m_Buffer.Get() + m_Front + (uint64_t)idx;
	}

	uint64_t Count() const { return m_Back - m_Front; }

	template<typename ...Args>
	T& Emplace(Args&&... args) {
		m_Buffer.Add();
		T* t = new (m_Buffer.Get() + m_Back++) T(std::forward<Args>(args)...);
		return *t;
	}

	void Add(const T& element, int32_t pos = -1) {
		if(Count() >= m_Buffer.GetMaxCount())
			Reallocate(10);

		if(pos < 0)
			pos += ++m_Back;
		else
			pos += --m_Front;

		new (m_Buffer.Get() + (uint64_t)pos) T(element);
		m_Buffer.Add();
	}

	void Add(const std::vector<T>& list) {
		if(Count() + list.size() >= m_Buffer.GetMaxCount())
			Reallocate(list.size());

		m_Buffer.Add(list);
	}

	void Add(const List& list) {
		if(Count() + list.size() >= m_Buffer.GetMaxCount())
			Reallocate(list.size());

		m_Buffer.Add(list.m_Buffer);
	}

	void Push(const T& element, bool back = true) {
		if(back)
			Add(element);
		else
			Add(element, 0);
	}

	T Pop(bool back = true) {
		auto idx = back ? --m_Back : m_Front++;
		T val = *At(idx);
		Remove(idx);
		return val;
	}

	void Remove(int64_t idx) {
		At(idx)->~T();
		m_Buffer.Remove();
	}

	template<typename TOut, class TPredicate>
	void For(TPredicate&& func) {
		for(auto& val : *this)
			func(val);
	}

	template<typename TOut, class TPredicate>
	List<TOut> Apply(TPredicate&& func) {
		List<TOut> out;
		for(auto& val : *this)
			out.Add(func(val));
		return out;
	}

	SearchResult Find(Func<bool, const T&> func) const {
		for(uint64_t i = 0; i < Count(); i++)
			if(func(*At(i)))
				return { true, i };

		return { false, 0 };
	}

	void Reallocate(uint64_t additional) {
		auto maxCount = m_Buffer.GetMaxCount() + additional;
		T* newData = (T*)malloc(maxCount * sizeof(T));

		for(uint64_t i = 0; i < Count(); i++) {
			new (newData + m_Front + i) T(*At(i));
			Remove(i);
		}

		m_Buffer.Delete();
		m_Buffer = Buffer<T>(newData, Count(), maxCount);
	}

	void Clear() {
		for(uint64_t i = 0; i < Count(); i++)
			Remove(i);

		m_Front = 0;
		m_Back = 0;
	}

	const Buffer<T>& Get() const { return m_Buffer; }

	using iterator = T*;
	using const_iterator = const T*;
	iterator begin() { return m_Buffer.Get() + m_Front; }
	iterator end() { return m_Buffer.Get() + m_Back; }
	const_iterator cbegin() const { return m_Buffer.Get() + m_Front; }
	const_iterator cend()	const { return m_Buffer.Get() + m_Back; }
	const_iterator begin()	const { return cbegin(); }
	const_iterator end()	const { return cend(); }

private:
	Buffer<T> m_Buffer;

	uint64_t m_Front = 0, m_Back = 0;
};

}