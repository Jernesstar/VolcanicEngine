#pragma once

#include <map>

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
using Callbacks = std::map<VolcaniCore::UUID, EventCallback<TEvent>>;

class Events {
public:
	static void Init();
	static void PollEvents() { glfwPollEvents(); }

	template<typename TEvent>
	requires std::derived_from<TEvent, Event>
	static void RegisterListener(const EventCallback<TEvent>& eventCallback) {
		if(!eventCallback)
			return;

		Callbacks<TEvent>& callbacks = GetCallbacks<TEvent>();
		callbacks[eventCallback.GetID()] = eventCallback;
	}

	template<typename TEvent>
	requires std::derived_from<TEvent, Event>
	static UUID RegisterListener(const Func<void,TEvent&>& callback) {
		EventCallback<TEvent> eventCallback(callback);
		RegisterListener<TEvent>(eventCallback);
		return eventCallback.GetID();
	}

	template<typename TEvent>
	requires std::derived_from<TEvent, Event>
	static void UnregisterListener(UUID callbackID) {
		GetCallbacks<TEvent>().erase(callbackID);
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
	template<typename TEvent>
	static void Dispatch(TEvent& event) {
		Callbacks<TEvent>& callbackList = GetCallbacks<TEvent>();
	
		for(auto& [_, callback] : callbackList) {
			if(event.Handled)
				return;

			callback(event);
		}
	}

	template<typename TEvent>
	static Callbacks<TEvent>& GetCallbacks();

	static void ErrorCallback(int error, const char* description);

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void KeyCharCallback(GLFWwindow* window, uint32_t codepoint);
	static void MouseMovedCallback(GLFWwindow* window, double x, double y);
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	static void MouseScrolledCallback(GLFWwindow* window, double x_scroll, double y_scroll);
	static void WindowResizedCallback(GLFWwindow* window, int width, int height);
	static void WindowMovedCallback(GLFWwindow* window, int x, int y);
	static void WindowClosedCallback(GLFWwindow* window);
};

}