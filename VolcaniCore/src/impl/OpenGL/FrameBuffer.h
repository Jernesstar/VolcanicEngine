#pragma once

#include "Core/Application.h"

namespace VolcaniCore::OpenGL {

class AttachmentSpecification;

class FrameBuffer {
public:
	FrameBuffer(const AttachmentSpecification& specs);
	~FrameBuffer();

	void Bind();
	void Unbind();
	void BindTexture();
	void BindRenderbuffer();

private:
	uint32_t m_BufferID;
	uint32_t m_TextureID;
	uint32_t m_RenderbufferID;
};

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

}