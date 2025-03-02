#include <iostream>

#include <VolcaniCore/Core/List.h>

using namespace VolcaniCore;

struct Type {
	int Val;

	Type() = default;
	Type(int i)
		: Val(i) { }
	Type(Type&&) = default;
	Type(const Type&) = default;
	~Type() = default;
};

static List<Type> theList;

void func() {
	// static_assert(std::is_aggregate<Type>());
}

void func2() {
	theList.Emplace();
	theList.Emplace();
	theList.Emplace();
	theList.Emplace();

	for(auto& val : theList)
		std::cout << val.Val << "\n";
}

int main() {
	std::cout << "Begin\n";
	func();
	func2();
	std::cout << "End\n";

	return 0;
}