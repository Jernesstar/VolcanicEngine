#pragma once

namespace VolcaniCore {

// template<typename T>
class UniformBuffer {
public:
	UniformBuffer() = default;
	virtual ~UniformBuffer() = default;

	// virtual void SetData(Ref<T> data) = 0;

	static Ref<UniformBuffer> Create();
};

}