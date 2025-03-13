#include <iostream>
#include <memory>

#include <VolcaniCore/Core/List.h>

using namespace VolcaniCore;

void func() {
	List<uint32_t> list;

	list.Add(1);
	list.Add(1);
	list.Add(1);
	list.Add(1);

	list.Clear();
}

int main() {
	std::cout << "Begin\n";
	func();
	std::cout << "End\n";

	return 0;
}