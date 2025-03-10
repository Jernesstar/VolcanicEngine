#include <iostream>
#include <filesystem>

#include <VolcaniCore/Core/List.h>
#include <VolcaniCore/Graphics/RendererAPI.h>

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