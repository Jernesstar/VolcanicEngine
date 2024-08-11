#pragma once

#include "Event.h"
#include "Core/Codes.h"

namespace VolcaniCore {

struct MouseEvent : public Event {
protected:
	MouseEvent(Event::Type type)
		: Event(Event::Category::Mouse, type) { }
};

struct MouseMovedEvent : public MouseEvent {
	const float x, y;

	MouseMovedEvent(float x, float y)
		: MouseEvent(Event::Type::MouseMoved), x(x), y(y) { }
};

struct MouseScrolledEvent : public MouseEvent {
	const float ScrollX;
	const float ScrollY;

	MouseScrolledEvent(float scrollX, float scrollY)
		: MouseEvent(Event::Type::MouseScrolled),
			ScrollX(scrollX), ScrollY(scrollY) { }
};

struct MouseButtonEvent : public MouseEvent {
	const MouseCode Button;
	const float x, y;

protected:
	MouseButtonEvent(Event::Type type, MouseCode button, float x, float y)
		: MouseEvent(type), Button(button), x(x), y(y) { }
};

struct MouseButtonPressedEvent : public MouseButtonEvent {
	MouseButtonPressedEvent(MouseCode button, float x, float y)
		: MouseButtonEvent(Event::Type::MouseButtonPressed, button, x, y) { }
};

struct MouseButtonReleasedEvent : public MouseButtonEvent {
	MouseButtonReleasedEvent(MouseCode button, float x, float y)
		: MouseButtonEvent(Event::Type::MouseButtonReleased, button, x, y) { }
};

}