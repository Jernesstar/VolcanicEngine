#pragma once

#include <unordered_map>

#include <ft2build.h>
#include <freetype/freetype.h>

#include <glm/vec3.hpp>

// #include "Core/Application.h"

namespace VolcaniCore {

class Font {
// private:
// 	struct Character {
// 		uint32_t   TextureID; // ID handle of the glyph texture
// 		glm::ivec2 Size;      // Size of glyph
// 		glm::ivec2 Bearing;   // Offset from baseline to left/top of glyph
// 		uint32_t   Advance;   // Offset to advance to next glyph
// 	};

// 	class CharacterQuad {
// 	public:
// 		CharacterQuad(Font::Character character)
// 			: m_Character(character)
// 		{
// 			m_Vertices[0] = { 0, 0 };
// 			m_Vertices[1] = { m_Character.Size.x, 0 };
// 			m_Vertices[2] = { m_Character.Size.x, m_Character.Size.y };
// 			m_Vertices[3] = { 0, m_Character.Size.y };
// 		}
// 		~CharacterQuad() { glDeleteTextures(1, &m_Character.TextureID); }

// 		const Font::Character GetCharacter() const { return m_Character; }
// 		const glm::vec2 GetVertices(uint32_t i) const
// 		{
// 			VOLCANICORE_ASSERT(i <= 4);
// 			return m_Vertices[i];
// 		}

// 		void Bind(uint32_t slot) { glBindTextureUnit(slot, m_Character.TextureID); }

// 	private:
// 		Font::Character m_Character;
// 		glm::vec2 m_Vertices[4];
// 	};

// public:
// 	const std::string FontPath;

// public:
// 	Font(const std::string& font_path, uint32_t width, uint32_t height);
// 	~Font();

// 	static void Init() { VOLCANICORE_ASSERT(!FT_Init_FreeType(&s_FT), "Could not initialize FreeType Library"); }
// 	static void Close() { FT_Done_FreeType(s_FT); }

// 	void SetWidth(uint32_t width) { FT_Set_Pixel_Sizes(m_Face, width, 0); UpdateCharacters(); }
// 	void SetHeight(uint32_t height) { FT_Set_Pixel_Sizes(m_Face, 0, height); UpdateCharacters(); };
// 	void SetSize(uint32_t width, uint32_t height) { FT_Set_Pixel_Sizes(m_Face, width, height); UpdateCharacters(); }

// 	glm::vec2 GetSize(const std::string& text) const;

// 	const CharacterQuad* GetQuad(char character) const
// 	{
// 		VOLCANICORE_ASSERT_ARGS(m_Quads.find(character) != m_Quads.end(), "%s is not a valid character!", character);
// 		return m_Quads.at(character);
// 	};

// private:
// 	FT_Face m_Face;
// 	std::unordered_map<char, CharacterQuad*> m_Quads;

// 	void DeleteCharacters();
// 	void UpdateCharacters();

// 	inline static FT_Library s_FT;

// 	friend class Renderer2D;
};

}