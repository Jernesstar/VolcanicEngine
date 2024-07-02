#pragma once

namespace VolcaniCore {


template<typename THandle>
class UniformBuffer {
public:
	UniformBuffer(Ref<THandle> handle)
		: m_Handle(handle) { }
	virtual ~UniformBuffer() = default;

	virtual void Update(uint32_t size = 0, uint32_t offset = 0) = 0;

	static Ref<UniformBuffer<THandle>> Create(Ref<THandle> handle) {
		RendererBackend backend = RendererAPI::Get()->Backend;

		switch(backend) {
			case RendererBackend::OpenGL:
				return CreateRef<OpenGL::UniformBuffer<THandle>>();
				break;
		}
	}

protected:
	Ref<THandle> m_Handle;
};


}