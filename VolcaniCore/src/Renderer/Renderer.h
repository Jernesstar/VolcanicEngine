#pragma once

namespace VolcaniCore {

class APIRenderer {
protected:
	APIRenderer();
	~APIRenderer();
};

class Renderer {
public:
	static void Init() { };
	static void Close();
	static void Render();

protected:
	Renderer() = delete;
	~Renderer() = delete;
};

}