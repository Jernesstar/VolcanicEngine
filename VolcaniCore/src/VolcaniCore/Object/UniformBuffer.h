#pragma once

namespace VolcaniCore {

// template<typename THandle>
class UniformBuffer {
public:
	// static Ref<UniformBuffer<THandle>> Create(Ref<THandle> handle);
	static UniformBuffer Create();

public:
	UniformBuffer() = default;
// 	UniformBuffer(Ref<THandle> handle)
// 		: m_Handle(handle) { }
	virtual ~UniformBuffer() = default;

	template<typename T>
	virtual void SetData(Buffer<T> buffer) = 0;

// protected:
// 	Ref<THandle> m_Handle;
};

}