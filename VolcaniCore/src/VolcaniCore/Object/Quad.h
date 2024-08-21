#pragma once

#include <glm/vec4.hpp>

#include "Texture.h"

namespace VolcaniCore {

class Quad {
public:
	static Ref<Quad> Create(Ref<Texture> texture);
	static Ref<Quad> Create(uint32_t width, uint32_t height,
							const glm::vec4& color = glm::vec4(1.0f));

public:
	const bool IsTextured;

public:
	Quad(Ref<Texture> texture)
		: IsTextured(true), m_Texture(texture),
			m_Width(texture->GetWidth()), m_Height(texture->GetHeight()) { }

	Quad(uint32_t width, uint32_t height,
		 const glm::vec4& color = glm::vec4(1.0f))
		: IsTextured(false), m_Color(color),
			m_Width(width), m_Height(height) { }

	Ref<Texture> GetColor() const { return m_Color; }
	Ref<Texture> GetTexture() const { return m_Texture; }

	uint32_t GetWidth() const { return m_Width; }
	uint32_t GetHeight() const { return m_Height; }

protected:
	uint32_t m_Width, m_Height;
	Ref<Texture> m_Texture = nullptr;
	glm::vec4 m_Color;
};

}