#include <flecs.h>

int main() {
	flecs::world world;
	flecs::entity entity = world.entity("TestEntity");

	return 0;
}