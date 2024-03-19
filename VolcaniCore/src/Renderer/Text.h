#pragma once

namespace VolcaniCore {

class Text {
public:
	Text(uint32_t size);

	static Ref<Text> Create(uint32_t size);
};

}