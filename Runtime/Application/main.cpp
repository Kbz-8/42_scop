#include <Core/Engine.h>

int main(int ac, char** av)
{
	Scop::ScopEngine engine(ac, av, "Scop", 1280, 720);
	engine.Run();
	return 0;
}
