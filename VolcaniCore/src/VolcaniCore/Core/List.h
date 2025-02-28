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

	T Pop() {
		return Pop(-1);
	}

	T PopFront() {
		return Pop(0);
	}

	T Pop(int64_t idx) {
		// VOLCANICORE_ASSERT(Count());
		T val = *At(idx);
		Remove(idx);
		*At(idx) = 1000;
		auto abs = Absolute(idx);

		if(abs == m_Front) {
			m_Front++;
			return val;
		}
		if(abs == m_Back - 1) {
			m_Back--;
			return val;
		}
		else {
			std::cout << "Abs: " << abs << "\n";
			// ShiftRight(m_Front++, abs, 1);
		}
		if(m_Front == m_Back)
			m_Front = m_Back = 0;

		return val;
	}

	void Insert(int64_t idx, const T& element) {
		if(Count() >= m_Buffer.GetMaxCount()) {
			auto newMax = m_Buffer.GetMaxCount() + 6;
			T* newData = (T*)malloc(newMax * sizeof(T));

			uint64_t pos;
			if(idx < 0)
				pos = (uint64_t)(((int64_t)m_Back + 1 + idx) - (int64_t)m_Front);
			else
				pos = (uint64_t)idx;

			uint64_t delta = 0;
			for(uint64_t i = 0; i < Count() + 1; i++) {
				if(i == pos) {
					new (newData + i) T(element);
					delta = 1;
				}
				else
					new (newData + i) T(*At(i - delta));

				Remove(i - delta);
			}

			m_Front = 0;
			m_Back = Count() + 1;
			m_Buffer.Delete();
			m_Buffer = Buffer<T>(newData, m_Back, newMax);
			return;
		}

		m_Buffer.Add();

		if(!Count()) {
			new (At(0)) T(element);
			m_Back++;
			return;
		}

		auto abs = Absolute(idx);
		if(idx == 0) {
			if(m_Front == 0) {
				ShiftRight(0, m_Back - 1, 1);
				m_Back++;
			}
			else
				m_Front--;
		}
		if(idx == -1) {
			if(m_Back == m_Buffer.GetMaxCount())
				ShiftLeft(m_Front, abs, 1);
			else
				m_Back++;
		}
		else {
			ShiftRight(abs, m_Back - 1, 1);
			m_Back++;
		}

		new (At(idx)) T(element);
	}

	template<typename TOut, class TPredicate>
	void For(TPredicate&& func) {
		for(auto& val : *this)
			func(val);
	}

	// template<typename TOut, class TPredicate>
	// List<TOut> Apply(TPredicate&& func) {
	// 	List<TOut> out;
	// 	for(auto& val : *this)
	// 		out.Add(func(val));
	// 	return out;
	// }

	// SearchResult Find(Func<bool, const T&> func) const {
	// 	for(uint64_t i = 0; i < Count(); i++)
	// 		if(func(*At(i)))
	// 			return { true, i };
	// 	return { false, 0 };
	// }

	void Reallocate(uint64_t additional) {
		Allocate(m_Buffer.GetMaxCount() + additional);
	}

	void Allocate(uint64_t maxCount) {
		if(maxCount == 0 && maxCount == m_Buffer.GetMaxCount())
			return;

		T* newData = (T*)malloc(maxCount * sizeof(T));
		for(uint64_t i = 0; i < Count() && i < maxCount; i++) {
			new (newData + i) T(*At(i));
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
		for(auto i = beg; i < end; i++)
			new (m_Buffer.Get(i) - dx) T(*m_Buffer.Get(i));
	}

	void ShiftRight(uint64_t beg, uint64_t end, uint64_t dx) {
		std::cout << beg << "," << end << "\n";
		for(auto i = end; i >= beg; i--) {
			new (m_Buffer.Get(i) + dx) T(*m_Buffer.Get(i));
			m_Buffer.Get(i)->~T();
		}
		// // Since m_Back points at the next available block of memory
		// // The condition checks for a hard inequality
		// if(end + dx > m_Buffer.GetMaxCount()) {
		// 	Reallocate(dx + 1);
		// 	return;
		// }
	}
};

}