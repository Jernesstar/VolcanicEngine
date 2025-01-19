#pragma once

template<typename T>
class TheList {
public:
	TheList()
		: m_Buffer(10) { }
	TheList(uint64_t size)
		: m_Buffer(size) { }
	TheList(const std::initializer_list<T>& list)
		: m_Buffer(list.size()), m_Back(list.size())
	{
		m_Buffer.Set(list.begin(), list.size());
	}
	TheList(TheList&& other) = default;
	TheList(const TheList& other) {
		m_Buffer = other.m_Buffer.Copy();
		m_Front = other.m_Front;
		m_Back = other.m_Back;
	}

	~TheList() = default;

	TheList& operator =(const TheList& other) {
		m_Buffer = other.m_Buffer.Copy();
		m_Front = other.m_Front;
		m_Back = other.m_Back;
		return *this;
	}
	TheList& operator =(TheList&& other) = default;

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
		m_Buffer.Add(T(std::forward<Args>(args)...));
		return *At(-1);
	}

	void Add(const T& element) {
		if(Count() >= m_Buffer.GetMaxCount())
			Reallocate(10);

		m_Buffer.Add(element);
		m_Back++;
	}

	void Add(const std::vector<T>& list) {
		if(Count() + list.size() >= m_Buffer.GetMaxCount())
			Reallocate(list.size());

		m_Buffer.Add(list);
	}

	void Add(const TheList& list) {
		if(Count() + list.size() >= m_Buffer.GetMaxCount())
			Reallocate(list.size());

		m_Buffer.Add(list.m_Buffer);
	}

	void Push(const T& element, bool back = true) {
		if(back) {
			Add(element);
			return;
		}

		// TODO(Implement): Push front
	}

	T Pop(bool back = true) {
		return back ? *At(m_Back--) : *At(m_Front++);
	}

	void Remove(int64_t idx) {
		
	}

	template<typename TOut, class TPredicate>
	void For(TPredicate&& func) {
		for(auto& val : *this)
			func(val);
	}

	// TODO(Implement): auto deduction
// decltype(auto)
// using value_type = decltype(std::declval<Lambda>()(*std::declval<Iterator>()))
	template<typename TOut, class TPredicate>
	TheList<TOut> Apply(TPredicate&& func) {
		TheList<TOut> out;
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
		m_Buffer.Reallocate(additional);
	}

	void Clear() {
		m_Buffer.Clear();
		m_Front = 0;
		m_Back = 0;
	}

	const Buffer<T>& GetBuffer() const { return m_Buffer; }

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

	uint64_t m_Front = 0, m_Back = 0;
};

namespace Demo {

class Template : public Application {
public:
	Template();

	void OnUpdate(TimeStep ts) override;

private:
	// TheList<uint32_t> list;

	Ref<Camera> camera;
	CameraController controller;
};

Template::Template() {
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::Escape)
				Application::Close();
		});
	Events::RegisterListener<KeyPressedEvent>(
		[](const KeyPressedEvent& event)
		{
			if(event.Key == Key::K)
				VOLCANICORE_LOG_INFO("Test Key K");
		});

	camera = CreateRef<StereographicCamera>(75.0f);
	camera->SetPosition({ 2.5f, 2.5f, 2.5f });
	camera->SetDirection({ -0.5f, -0.5f, -0.5f });
	controller = CameraController{ camera };

	// list = { 1, 2, 3, 4, 5 };
	VOLCANICORE_LOG_INFO("Template Project Started");

	// uint32_t* data = list.At(-1);

	// VOLCANICORE_LOG_INFO("%i", *data);

	// auto [found, idx] = list.Find([](const uint32_t& val) { return val == 23; });
	// if(!found)
	// 	VOLCANICORE_LOG_INFO("No 23 in the list");

	// uint32_t last = list.Pop();
	// VOLCANICORE_LOG_INFO("%i", last);
}

void Template::OnUpdate(TimeStep ts) {

}

}