#pragma once

#include "Event.h"

namespace VolcaniCore {

struct WindowEvent : public Event {
protected:
	WindowEvent(Event::Type type)
		: Event(Event::Category::Window, type) { }
};

struct WindowResizedEvent : public WindowEvent {
	const int Width, Height;

	WindowResizedEvent(int width, int height)
		: WindowEvent(Event::Type::WindowResized),
			Width(width), Height(height) { }
};

struct WindowMovedEvent : public WindowEvent {
	const int x, y;

	WindowMovedEvent(int x, int y)
		: WindowEvent(Event::Type::WindowMoved), x(x), y(y) { }
};

struct WindowClosedEvent : public WindowEvent {
	WindowClosedEvent()
		: WindowEvent(Event::Type::WindowClosed) { }
};

}
