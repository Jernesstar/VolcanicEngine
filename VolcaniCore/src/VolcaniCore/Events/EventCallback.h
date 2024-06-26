#pragma once

#include <functional>

#include "Core/UUID.h"

namespace VolcaniCore {

template<typename TEvent>
class EventCallback {
public:
	EventCallback(const std::function<void(TEvent&)>& callback
					= [](TEvent& event) { }) : m_ID(), m_Callback(callback) { }

	const UUID& GetID() const { return m_ID; }
	const std::function<void(TEvent&)>& GetCallback() const { return m_Callback; }

	void operator ()(TEvent& event) const { this->m_Callback(event); }

	EventCallback<TEvent>& operator =(const EventCallback<TEvent>& other) = default;
	EventCallback<TEvent>& operator =(const std::function<void(TEvent&)>& callback) {
		m_Callback = callback;
		return *this;
	}

	operator bool() const { return bool(this->m_Callback); }

	bool operator ==(const EventCallback<TEvent>& other) const {
		return this->m_ID == other.m_ID;
	}
	bool operator !=(const EventCallback<TEvent>& other) const {
		return this->m_ID != other.m_ID;
	}

private:
	UUID m_ID;
	std::function<void(TEvent&)> m_Callback;
};

}