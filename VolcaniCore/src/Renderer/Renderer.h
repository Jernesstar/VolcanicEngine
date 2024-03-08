#pragma once

namespace VolcaniCore {

class APIRenderer {
protected:
    APIRenderer():
    ~APIRenderer();
};

class Renderer {
public:
	void Init();
	void Close();
	void Render();

protected:
	Renderer() = delete;
	~Renderer() = delete;
};

}