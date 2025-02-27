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
		: m_Buffer(5) { }
	List(uint64_t size)
		: m_Buffer(size) { }
	List(const std::initializer_list<T>& list)
		: m_Buffer(list.size())
	{
		for(auto& element : list)
			Add(element);
	}
	List(List&& other)
		: m_Buffer(other.m_Buffer.GetMaxCount())
	{
		for(auto& val : other)
			Add(val);
	}
	List(const List& other)
		: m_Buffer(other.m_Buffer.GetMaxCount())
	{
		for(auto& val : other)
			Add(val);
	}

	~List() {
		Clear();
		m_Buffer.Delete();
	}

	List& operator =(const std::initializer_list<T>& list) {
		Clear();
		m_Buffer.Delete();
		m_Buffer = Buffer<T>(list.size());

		for(auto& val : list)
			Add(val);

		return *this;
	}

	List& operator =(List&& other) {
		Clear();
		m_Buffer.Delete();
		m_Buffer = Buffer<T>(other.m_Buffer.GetMaxCount());

		for(auto& val : other)
			Add(val);

		return *this;
	}

	List& operator =(const List& other) {
		Clear();
		m_Buffer.Delete();
		m_Buffer = Buffer<T>(other.m_Buffer.GetMaxCount());

		for(auto& val : other)
			Add(val);

		return *this;
	}

	operator bool() const { return Count(); }

	uint64_t Count() const { return m_Back - m_Front; }

	T& operator[](int64_t idx) { return *At(idx); }
	const T& operator[](int64_t idx) const { return *At(idx); }

	T* At(int64_t idx) const {
		return m_Buffer.Get(Absolute(idx));
	}

	template<typename ...Args>
	T& Emplace(Args&&... args) {
		return EmplaceAt(m_Back, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	T& EmplaceAt(int64_t location, Args&&... args) {
		Insert(location, T(std::forward<Args>(args)...));
		return *At(location);
	}

	void Add(const T& element) {
		Insert(-1, element);
	}

	void Stack(const T& element) {
		Insert(0, element);
	}

	void Queue(const T& element) {
		Insert(-1, element);
	}

	T Pop(int64_t pos = -1) {
		VOLCANICORE_ASSERT(Count());
		T val = *At(idx);
		Remove(idx);
		return val;
	}

	void Insert(int64_t pos, const T& element) {
		uint64_t abs = Absolute(pos);

		if(Count() >= m_Buffer.GetMaxCount()) {
			auto newMax = m_Buffer.GetMaxCount() + 5;
			T* newData = (T*)malloc(newMax * sizeof(T));

			for(uint64_t i = 0; i < Count() + 1; i++) {
				if(i == abs)
					new (newData + m_Front + i) T(element);
				else {
					new (newData + m_Front + i) T(*At(i));
					Remove(i);
				}
			}
	
			m_Buffer.Delete();
			m_Buffer = Buffer<T>(newData, Count() + 1, newMax);
			return;
		}
		
		if(abs == m_Front) {
			if(m_Front == 0)
				ShiftRight(abs, m_Back, 1);
			else
				m_Front--;
		}
		else if(abs == m_Back) {
			if(m_Back == m_Buffer.GetMaxCount())
				ShiftLeft(m_Front, abs, 1);
			else
				m_Back--;
		}

		m_Buffer.Add();
		new (At(pos)) T(element);
	}

	// Opposite of Insert
	void Remove(int64_t idx) {
		m_Buffer.Remove();
		At(idx)->~T();
		auto abs = Absolute(idx);

		if(abs == m_Front) {
			m_Front++;
			return;
		}
		if(abs == m_Back) {
			m_Back--;
			return;
		}
		else {
			ShiftLeft(abs, m_Back, 1);
		}
		if(m_Front == m_Back)
			m_Front = m_Back = 0;
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
		Allocate(m_Buffer.GetMaxCount() + additional);
	}

	void Allocate(uint64_t count) {
		if(count == 0 && count == m_Buffer.GetMaxCount())
			return;

		T* newData = (T*)malloc(count * sizeof(T));
		for(uint64_t i = 0; i < Count(); i++) {
			new (newData + m_Front + i) T(*At(i));
			Remove(i);
		}

		m_Buffer.Delete();
		m_Buffer = Buffer<T>(newData, Count());
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

private:
	uint64_t Absolute(int64_t idx) const {
		if(idx < 0)
			return (int64_t)m_Back + idx;
		return m_Front + (uint64_t)idx;
	}

	void ShiftLeft(uint64_t beg, uint64_t end, uint64_t dx) {
		if((int64_t)m_Front - dx < 0) {
			Reallocate(dx + 1);
			return;
		}

		for(auto i = beg; i < end; i++)
			new (m_Buffer.Get(i) - dx) T(*m_Buffer.Get(i));
	}

	void ShiftRight(uint64_t beg, uint64_t end, uint64_t dx) {
		// Since m_Back points at the next available block of memory
		// The condition checks for a hard inequality
		if(m_Back + dx > m_Buffer.GetMaxCount()) {
			Reallocate(dx + 1);
			return;
		}
		for(auto i = end; i > beg; i--)
			new (m_Buffer.Get(i) + dx) T(*m_Buffer.Get(i));
	}
};

}