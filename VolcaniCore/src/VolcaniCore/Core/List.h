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
	List() = default;
	List(uint64_t size)
		: m_Buffer(size) { }
	List(std::initializer_list<T> list)
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
	}

	List& operator =(std::initializer_list<T> list) {
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
		VOLCANICORE_ASSERT(idx >= -(int64_t)m_Back || idx < Count());
		auto abs = Absolute(idx);
		return m_Buffer.Get(abs);
	}

	template<typename ...Args>
	T& Emplace(Args&&... args) {
		return EmplaceAt(-1, std::forward<Args>(args)...);
	}

	template<typename ...Args>
	T& EmplaceAt(int64_t idx, Args&&... args) {
		Free(idx);
		if constexpr(std::is_aggregate<T>())
			new (At(idx)) T{ std::forward<Args>(args)... };
		else
			new (At(idx)) T(std::forward<Args>(args)...);

		return *At(idx);
	}

	void Add(const T& element) {
		Insert(-1, element);
	}

	void Queue(const T& element) {
		Insert(0, element);
	}

	void Push(const T& element) {
		Insert(-1, element);
	}

	T Pop() {
		return Pop(-1);
	}

	T PopFront() {
		return Pop(0);
	}

	T Pop(int64_t idx) {
		VOLCANICORE_ASSERT(Count());
		T val = *At(idx);
		Remove(idx);
		auto abs = Absolute(idx);

		if(abs == m_Front)
			m_Front++;
		else if(abs == m_Back - 1)
			m_Back--;
		else
			ShiftRight(m_Front++, abs - 1, 1);

		if(m_Front == m_Back)
			m_Front = m_Back = 0;

		return val;
	}

	void Insert(int64_t idx, const T& element) {
		Free(idx);
		new (At(idx)) T(element);
	}

	template<typename TOut, class TPredicate>
	void For(TPredicate&& func) {
		for(auto& val : *this)
			func(val);
	}

	template<typename TOut>
	List<TOut> Apply(const Func<TOut, T&>& func) {
		List<TOut> out;
		for(auto& val : *this)
			out.Add(func(val));
		return out;
	}

	SearchResult Find(const Func<bool, const T&>& func) const {
		for(uint64_t i = 0; i < Count(); i++)
			if(func(*At(i)))
				return { true, i };
		return { false, 0 };
	}

	void Reallocate(uint64_t additional) {
		Allocate(m_Buffer.GetMaxCount() + additional);
	}

	void Allocate(uint64_t maxCount) {
		if(maxCount <= m_Buffer.GetMaxCount())
			return;

		T* newData = (T*)malloc(maxCount * sizeof(T));
		for(uint64_t i = 0; i < Count(); i++) {
			new ((newData + i)) T(*At(i));
			Remove(i);
		}

		m_Back -= m_Front;
		m_Front = 0;
		m_Buffer.Delete();
		m_Buffer = Buffer<T>(newData, m_Back, maxCount);
	}

	void Clear() {
		for(uint64_t i = 0; i < Count(); i++)
			Remove(i);

		m_Front = 0;
		m_Back = 0;
	}

	const Buffer<T>& GetBuffer() const { return m_Buffer; }

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
			return (uint64_t)((int64_t)m_Back + idx);
		return m_Front + (uint64_t)idx;
	}

	void Remove(int64_t idx) {
		m_Buffer.Remove();
		At(idx)->~T();
	}

	void ShiftLeft(uint64_t beg, uint64_t end, uint64_t dx) {
		for(int64_t i = (int64_t)beg; i <= (int64_t)end; i++) {
			new (m_Buffer.Get((uint64_t)i - dx)) T(*m_Buffer.Get((uint64_t)i));
			m_Buffer.Get((uint64_t)i)->~T();
		}
	}

	void ShiftRight(uint64_t beg, uint64_t end, uint64_t dx) {
		for(int64_t i = (int64_t)end; i >= (int64_t)beg; i--) {
			new (m_Buffer.Get((uint64_t)i + dx)) T(*m_Buffer.Get((uint64_t)i));
			m_Buffer.Get((uint64_t)i)->~T();
		}
	}

	void Free(int64_t idx) {
		if(!m_Buffer.GetMaxCount()) {
			m_Buffer = Buffer<T>(5);
			m_Back = 1;
			m_Front = 0;
			return;
		}

		if(Count() == m_Buffer.GetMaxCount()) {
			auto newMax = m_Buffer.GetMaxCount() + 11;
			T* newData = (T*)malloc(newMax * sizeof(T));

			uint64_t pos;
			if(idx < 0)
				pos = (uint64_t)((int64_t)m_Back + 1 + idx);
			else
				pos = (uint64_t)idx;

			uint64_t delta = 0;
			for(uint64_t i = 0; i < Count() + 1; i++) {
				if(i == pos)
					delta = 1;
				else {
					new ((newData + i)) T(*At(i - delta));
					Remove(i - delta);
				}
			}

			m_Buffer.Delete();
			m_Buffer = Buffer<T>(newData, ++m_Back, newMax);
			return;
		}

		m_Buffer.Add();

		if(!Count()) {
			m_Back++;
			return;
		}

		auto abs = Absolute(idx);
		if(abs == 0) {
			if(m_Front != 0)
				m_Front--;
			else
				ShiftRight(0, (m_Back++) - 1, 1);
		}
		else if(abs == m_Back - 1) {
			if(m_Back != m_Buffer.GetMaxCount())
				m_Back++;
			else
				ShiftLeft((m_Front--), m_Back - 1, 1);
		}
		else
			ShiftRight(abs, (m_Back++) - 1, 1);
	}
};

}