#pragma once

#include <string>

#include "Event.h"
#include "Core/Codes.h"

namespace VolcaniCore {

struct KeyEvent : public Event {
	const KeyCode Key;

protected:
	KeyEvent(Event::Type type, KeyCode key)
		: Event(Event::Category::Key, type), Key(key) { }
};

struct KeyPressedEvent : public KeyEvent {
	const bool IsRepeat;

	KeyPressedEvent(KeyCode key, bool repeat = false)
		: KeyEvent(Event::Type::KeyPressed, key), IsRepeat(repeat) { }
};

struct KeyReleasedEvent : public KeyEvent {
	KeyReleasedEvent(KeyCode key)
		: KeyEvent(Event::Type::KeyReleased, key) { }
};

struct KeyCharEvent : public KeyEvent {
	const char Char;

	KeyCharEvent(KeyCode key, const char& _char)
		: KeyEvent(Event::Type::KeyCharEvent, key), Char(_char) { }

	std::string ToString() const { return std::string{ Char }; }
};

}