#pragma once

#include "Event.h"
#include "Core/Time.h"

namespace VolcaniCore {

struct ApplicationEvent : public Event {
protected:
	ApplicationEvent(Event::Type type)
		: Event(Event::Category::Application, type) { }
};

struct ApplicationUpdatedEvent : public ApplicationEvent {
	TimeStep DeltaTime;

	ApplicationUpdatedEvent(TimeStep ts)
		: ApplicationEvent(Event::Type::ApplicationUpdated), DeltaTime(ts) { }
};

}