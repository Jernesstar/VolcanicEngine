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

void func2(Ref<Tab> tab) {
	theList.Add(tab);
}

void func() {
	theList.Allocate(5);

	auto tab = std::make_shared<Tab>(std::string("First Tab"), 2);
	func2(tab);
	theList.Add(std::make_shared<Tab>(std::string("Second Tab"), 4));
	theList.Add(std::make_shared<Tab>(std::string("Third Tab"), 1));

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