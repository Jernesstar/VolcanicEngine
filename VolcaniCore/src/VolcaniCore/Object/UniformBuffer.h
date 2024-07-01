#pragma once

namespace VolcaniCore {

template<typename T>
class UniformBuffer {
public:
	UniformBuffer();
	~UniformBuffer();

	virtual void SetData(Ref<T> data) = 0;

	static void Create();
};

}