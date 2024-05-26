// #include "Font.h"

// #include <stb_image/stb_image.h>

// namespace VolcaniCore {

// Font::Font(const std::string& font_path, uint32_t width, uint32_t height)
// 	: FontPath(font_path)
// {
// 	if(FT_New_Face(Font::s_FT, font_path.c_str(), 0, &m_Face))
// 	{
// 		SADDLE_CORE_LOG_ERROR("Failed to load font. Path: %s", font_path.c_str());
// 	}

// 	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

// 	m_Quads.reserve(128);

// 	SetSize(width, height);
// }

// Font::~Font()
// {
// 	FT_Done_Face(m_Face);

// 	DeleteCharacters();
// }

// void Font::DeleteCharacters()
// {
// 	for(const auto& [_, quad] : m_Quads)
// 		delete quad;
// }

// void Font::UpdateCharacters()
// {
// 	DeleteCharacters();

// 	for(unsigned char c = 0; c < 128; c++)
// 	{
// 		if(FT_Load_Char(m_Face, c, FT_LOAD_RENDER))
// 		{
// 			SADDLE_CORE_LOG_ERROR("Failed to load glyph: %s", c);
// 		}

// 		uint32_t w = m_Face->glyph->bitmap.width, h = m_Face->glyph->bitmap.rows;

// 		uint32_t texture_id;
// 		glGenTextures(1, &texture_id);
// 		glBindTexture(GL_TEXTURE_2D, texture_id);
// 		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, m_Face->glyph->bitmap.buffer);

// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
// 		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

// 		Character ch =
// 		{
// 			texture_id,
// 			glm::ivec2(w, h),
// 			glm::ivec2(m_Face->glyph->bitmap_left, m_Face->glyph->bitmap_top),
// 			(uint32_t)m_Face->glyph->advance.x >> 6
// 		};

// 		CharacterQuad* quad = new CharacterQuad(ch);
// 		m_Quads[c] = quad;
// 	}
// }

// glm::vec2 Font::GetSize(const std::string& text) const
// {
// 	float x = 0.0f, high_y = 0.0f;
// 	for(auto c = text.begin(); c != text.end(); c++)
// 	{
// 		const Character& ch = GetQuad(*c)->GetCharacter();

// 		x += ch.Advance >> 6;
// 		high_y = std::max((float)high_y, (float)ch.Bearing.y);
// 	}

// 	return { x, high_y };
// }

// }