#include <iostream>

#include <VolcaniCore/Core/List.h>

struct DrawPass {

};

struct DrawCommand {
	DrawPass* Pass;

	uint32_t Width, Height;
};

using namespace VolcaniCore;

void func() {
	DrawPass pass;
	List<DrawCommand> theList(3);

	theList.Emplace(&pass);
	theList.Emplace(&pass);
	theList.Emplace(&pass);
	theList.EmplaceAt(0, &pass);

	std::cout << "Count: " << theList.Count() << "\n";
	for(auto& val : theList)
		std::cout << val.Pass << "\n";
}

int main() {
	std::cout << "Begin\n";
	func();
	std::cout << "End\n";

	return 0;
}