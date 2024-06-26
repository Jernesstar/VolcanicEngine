#pragma once

#include "Event.h"

namespace VolcaniCore {

struct WindowEvent : public Event {
protected:
	WindowEvent(EventType type)
		: Event(EventCategory::Window, type) { }
};

struct WindowResizedEvent : public WindowEvent {
	const int Width, Height;

	WindowResizedEvent(int width, int height)
		: WindowEvent(EventType::WindowResized),
			Width(width), Height(height) { }
};

struct WindowMovedEvent : public WindowEvent {
	const int x, y;

	WindowMovedEvent(int x, int y)
		: WindowEvent(EventType::WindowMoved), x(x), y(y) { }
};

struct WindowClosedEvent : public WindowEvent {
	WindowClosedEvent()
		: WindowEvent(EventType::WindowClosed) { }
};

}
