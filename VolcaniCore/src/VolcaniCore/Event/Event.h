#pragma once

namespace VolcaniCore {

struct Event {

	enum class Category {
		Key,
		Mouse,
		Window,
		Application,
	};

	enum class Type {
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

	const Event::Category Category;
	const Event::Type Type;
	bool Handled = false;

	bool IsInCategory(Event::Category category) const {
		return this->Category == category;
	}
	bool Is(Event::Type type) const { return this->Type == type; }

	template<typename TEvent>
	requires std::derived_from<TEvent, Event>
	TEvent& As() const { return *((TEvent*)(this)); }

protected:
	Event(Event::Category category, Event::Type type)
		: Category(category), Type(type) { }
};

}