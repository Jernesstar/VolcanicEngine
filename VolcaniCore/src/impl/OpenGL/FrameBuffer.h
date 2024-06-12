#pragma once

#include <cstdint>

#include "Renderer/FrameBuffer.h"

namespace VolcaniCore::OpenGL {

enum class AttachmentType { Texture, RenderBuffer, None };

struct AttachmentSpecification {
	uint32_t Width, Height;
	AttachmentType Color, Depth, Stencil;

	AttachmentSpecification(uint32_t width, uint32_t height,
		AttachmentType color = AttachmentType::Texture,
		AttachmentType depth = AttachmentType::RenderBuffer,
		AttachmentType stencil = AttachmentType::RenderBuffer
	) : Width(width), Height(height), Color(color), Depth(depth), Stencil(stencil) { }
};

class FrameBuffer : public VolcaniCore::FrameBuffer {
public:
	FrameBuffer(const AttachmentSpecification& specs);
	~FrameBuffer();

	void Bind() const override;
	void Unbind() const override;

	void BindTexture() const;
	void BindRenderbuffer() const;

	// TODO: Implement
	void Resize(uint32_t width, uint32_t height) { }

	uint32_t GetColorAttachmentID() { return m_TextureID; }

private:
	uint32_t m_BufferID;
	uint32_t m_TextureID;
	uint32_t m_RenderbufferID;
};

}