#pragma once

#include "Buffer.h"

namespace VolcaniCore {

template<typename T>
class List {
public:
	List()
		: m_Buffer(5)
	{

	}
	~List() = default;

	operator bool() const { return m_Buffer.GetCount(); }

	T& operator[](int64_t idx) {
		if(idx < 0)
			return m_Buffer[(uint32_t)((int64_t)m_Buffer.GetCount() + idx)];
		return m_Buffer[idx];
	}

	T* At(int32_t idx) {
		if(idx < 0)
			return m_Buffer.Get() + (uint32_t)((int32_t)Count() + idx);
		return m_Buffer.Get() + (uint32_t)(idx);
	}

	template<typename ...Args>
	void Emplace(Args&&... args) {
		m_Buffer.Add(T(std::forward<Args>(args)...));
	}

	void Add(const T& element) {
		m_Buffer.Add(element);
	}
	void Push() {
		
	}
	void Pop() {

	}

	uint64_t Count() { return m_Buffer.GetCount(); }

	template<typename TOut, class TPredicate>
	void For(TPredicate&& func) {
		for(auto& val : *this)
			func(val);
	}

	// TODO(Implement): auto deduction
// decltype(auto)
// using value_type = decltype(std::declval<Lambda>()(*std::declval<Iterator>()))
	template<typename TOut, class TPredicate>
	List<TOut> Apply(TPredicate&& func) {
		List<TOut> out;
		for(auto& val : *this)
			out.Add(func(val));
		return out;
	}

	using iterator = T*;
	using const_iterator = const T*;
	iterator begin() { return m_Buffer.begin(); }
	iterator end() { return m_Buffer.end(); }
	const_iterator cbegin() const { return m_Buffer.cbegin(); }
	const_iterator cend()	const { return m_Buffer.cend(); }
	const_iterator begin()	const { return cbegin(); }
	const_iterator end()	const { return cend(); }

private:
	Buffer<T> m_Buffer;
};

}