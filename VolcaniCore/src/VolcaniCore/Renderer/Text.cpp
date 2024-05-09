#include "Text.h"

namespace VolcaniCore {

// void Text::SetText(const std::string& text, const Font& font)
// {
// 	m_Quads.reserve(text.size());

// 	float x = 0.0f, y = 0.0f;
// 	for(auto c = text.begin(); c != text.end(); c++)
// 	{
// 		const Font::Character& ch = font.GetCharacter(*c);

// 		float w = ch.Size.x;
// 		float h = ch.Size.y;

// 		float xpos = x + ch.Bearing.x + 0.5f * w;
// 		float ypos = y - (ch.Size.y - ch.Bearing.y) + 0.5f * h;

// 		CharacterQuad quad;
// 		quad.Character = ch;
// 		quad.Vertices[0] = { xpos,     ypos     };
// 		quad.Vertices[1] = { xpos + w, ypos     };
// 		quad.Vertices[2] = { xpos + w, ypos + h };
// 		quad.Vertices[3] = { xpos,     ypos + h };

// 		m_Quads.push_back(quad);
// 		x += (ch.Advance >> 6);
// 	}
// }

}