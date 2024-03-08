#pragma once

#include <string>

#include "Event.h"
#include "Core/Codes.h"

namespace VolcaniCore {

struct KeyEvent : public Event {
    const KeyCode Key;

protected:
    KeyEvent(EventType type, KeyCode key) : Event(EventCategory::Key, type), Key(key) { }
};

struct KeyPressedEvent : public KeyEvent {
    const bool IsRepeat;
    
    KeyPressedEvent(KeyCode key, bool repeat = false)
        : KeyEvent(EventType::KeyPressed, key), IsRepeat(repeat) { }
};

struct KeyReleasedEvent : public KeyEvent {
    KeyReleasedEvent(KeyCode key) : KeyEvent(EventType::KeyReleased, key) { }
};

struct KeyCharEvent : public KeyEvent {
    const char Char;

    KeyCharEvent(KeyCode key, const char& _char)
        : KeyEvent(EventType::KeyCharEvent, key), Char(_char) { };

    std::string ToString() const
    {
        std::string str;
        return str = Char;
    }
};

}