#pragma once

namespace VolcaniCore {

enum class EventCategory { Key, Mouse, Window, Application };

enum class EventType {
	KeyPressed,
	KeyReleased,
	KeyCharEvent,
	MouseMoved,
	MouseScrolled,
	MouseButtonPressed,
	MouseButtonReleased,
	WindowResized,
	WindowMoved,
	WindowClosed,
	ApplicationUpdated
};

struct Event {
	const EventCategory Category;
	const EventType Type;
	bool Handled = false;

	bool IsInCategory(EventCategory category) const {
		return this->Category == category;
	}
	bool Is(EventType type) const { return this->Type == type; }

	template<typename TEvent>
	requires std::derived_from<TEvent, Event>
	TEvent& As() const { return *((TEvent*)(this)); }

protected:
	Event(EventCategory category, EventType type)
		: Category(category), Type(type) { }
};

}