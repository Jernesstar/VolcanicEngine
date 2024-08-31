#pragma once

namespace VolcaniCore {

template<typename TUniform, typename THandle>
class UniformBuffer {
public:
	static Ref<UniformBuffer<TUniform>> Create(Ref<THandle> handle);

public:
	std::function<TUniform(Ref<THandle)>> SetData = [](){};

public:
	UniformBuffer() = default;
	UniformBuffer(Ref<THandle> handle)
		: m_Handle(handle) { }

	virtual ~UniformBuffer() = default;

protected:
	Ref<THandle> m_Handle;
};

}