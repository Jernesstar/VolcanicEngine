#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "Core/Window.h"
#include "Core/Defines.h"

#include "Event.h"
#include "KeyEvents.h"
#include "MouseEvents.h"
#include "WindowEvents.h"
#include "ApplicationEvents.h"
#include "EventCallback.h"

namespace VolcaniCore {

template<typename TEvent>
using Callbacks = std::unordered_map<VolcaniCore::UUID, EventCallback<TEvent>>;

class EventSystem {
public:
	static void Init();
	static void PollEvents() { glfwPollEvents(); }

	template<typename TEvent>
	static void RegisterListener(const EventCallback<TEvent>& event_callback) {
		if(!event_callback)
			return;

		Callbacks<TEvent>& callbacks = GetCallbacks<TEvent>();
		callbacks[event_callback.GetID()] = event_callback;
	}

	template<typename TEvent>
	static EventCallback<TEvent> RegisterListener(const std::function<void(TEvent&)>& callback) {
		EventCallback<TEvent> _event_callback(callback);
		RegisterListener<TEvent>((EventCallback<TEvent>&)_event_callback);
		return _event_callback;
	}

	template<typename TEvent>
	static void UnregisterListener(const EventCallback<TEvent>& event_callback) {
		Callbacks<TEvent>& callbacks = GetCallbacks<TEvent>();
		if(callbacks.count(event_callback.GetID()))
			callbacks.erase(event_callback.GetID());
	}

private:
	inline static Callbacks<KeyPressedEvent>          KeyPressedEventCallbacks;
	inline static Callbacks<KeyReleasedEvent>         KeyReleasedEventCallbacks;
	inline static Callbacks<KeyCharEvent>             KeyCharEventCallbacks;
	inline static Callbacks<MouseMovedEvent>          MouseMovedEventCallbacks;
	inline static Callbacks<MouseScrolledEvent>       MouseScrolledEventCallbacks;
	inline static Callbacks<MouseButtonPressedEvent>  MouseButtonPressedEventCallbacks;
	inline static Callbacks<MouseButtonReleasedEvent> MouseButtonReleasedEventCallbacks;
	inline static Callbacks<WindowResizedEvent>       WindowResizedEventCallbacks;
	inline static Callbacks<WindowMovedEvent>         WindowMovedEventCallbacks;
	inline static Callbacks<WindowClosedEvent>        WindowClosedEventCallbacks;
	inline static Callbacks<ApplicationUpdatedEvent>  ApplicationUpdatedEventCallbacks;

private:
	// template<typename TEvent>
	// static EventCallback<TEvent> RegisterListener(const std::function<void(const TEvent&)>& callback) {
	// 	return RegisterListener(callback);
	// }

	template<typename TEvent>
	static void Dispatch(TEvent& event)
	{
		Callbacks<TEvent>& callback_list = GetCallbacks<TEvent>();
	
		for(auto& [_, func] : callback_list) 
			func(event); 
	}

	template<typename TEvent>
	static Callbacks<TEvent>& GetCallbacks();

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void KeyCharCallback(GLFWwindow* window, uint32_t codepoint);
	static void MouseMovedCallback(GLFWwindow* window, double x, double y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrolledCallback(GLFWwindow* window, double x_scroll, double y_scroll);
	static void WindowResizedCallback(GLFWwindow* window, int width, int height);
	static void WindowMovedCallback(GLFWwindow* window, int x, int y);
	static void WindowClosedCallback(GLFWwindow* window);

	friend class Application;
	friend class Window;
};

}