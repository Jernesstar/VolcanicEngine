#include "Panel.h"

#include "Tab.h"

namespace Magma {

Panel::Panel(const std::string& name)
	: Name(name) { }

void Panel::SetTab(Tab* tab) {
	m_Tab = tab;
}

}