#pragma once

#include <glm/vec4.hpp>

#include "Texture.h"

namespace VolcaniCore {

class Quad {
public:
	Quad(Ref<Texture> texture)
		: m_Texture(texture),
			m_Width(texture->GetWidth()), m_Height(texture->GetHeight()) { }
	Quad(uint32_t width,
		 uint32_t height,
		 const glm::vec4& color = glm::vec4(1.0f))
		: m_Texture(nullptr), m_Color(color),
			m_Width(width), m_Height(height) { }
	Quad(const glm::vec4& color = glm::vec4(1.0f))
		: m_Texture(nullptr), m_Color(color), m_Width(1), m_Height(1) { }

	static Ref<Quad> Create(Ref<Texture> texture);
	static Ref<Quad> Create(uint32_t width, uint32_t height,
							const glm::vec4& color = glm::vec4(1.0f));
	static Ref<Quad> Create(const glm::vec4& color = glm::vec4(1.0f));

	template<typename Derived>
	Derived* As() const { return (Derived*)(this); }

protected:
	uint32_t m_Width, m_Height;
	Ref<Texture> m_Texture;
	glm::vec4 m_Color;
};

}