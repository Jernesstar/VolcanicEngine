#pragma once

namespace VolcaniCore {

// template<typename THandle>
class UniformBuffer {
public:
	// static Ref<UniformBuffer<THandle>> Create(Ref<THandle> handle);
	// static UniformBuffer Create();

public:
	UniformBuffer() = default;
// 	UniformBuffer(Ref<THandle> handle)
// 		: m_Handle(handle) { }
	virtual ~UniformBuffer() = default;

// 	virtual void Update(uint32_t size = 0, uint32_t offset = 0) = 0;

// protected:
// 	Ref<THandle> m_Handle;
};

}