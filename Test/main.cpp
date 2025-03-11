#include <iostream>
#include <memory>

#include <VolcaniCore/Core/List.h>

using namespace VolcaniCore;

struct Tab {
	std::string Name;
	uint32_t Type;

	Tab(const std::string& name, uint32_t type)
		: Name(name), Type(type) { }
};

List<std::shared_ptr<Tab>> theList;

void func() {
	theList.Allocate(3);

	theList.Add(std::make_shared<Tab>(std::string("First Tab"), 2));
	theList.Add(std::make_shared<Tab>(std::string("Second Tab"), 4));
	theList.Add(std::make_shared<Tab>(std::string("Third Tab"), 1));
	theList.Add(std::make_shared<Tab>(std::string("Fourth Tab"), 1));
	theList.Add(std::make_shared<Tab>(std::string("Fifth Tab"), 0));
	theList.Add(std::make_shared<Tab>(std::string("Sixth Tab"), 1));

	std::cout << "Count: " << theList.Count() << "\n";
	for(auto& val : theList)
		std::cout << val->Name << ", Type: " << val->Type << "\n";
}

int main() {
	std::cout << "Begin\n";
	func();
	std::cout << "End\n";

	return 0;
}