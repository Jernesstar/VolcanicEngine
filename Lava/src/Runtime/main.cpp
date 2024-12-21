#include <Lava/Project.h>

using namespace Lava;

int main(int argc, char** argv) {
	auto proj = new Project();
	proj->Load(std::string(argv[1]));
	proj->Reload();
	proj->Run();

	return 0;
}