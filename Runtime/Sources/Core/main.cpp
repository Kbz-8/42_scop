#include <Core/Engine.h>

int main(int ac, char** av)
{
	Scop::ScopEngine engine(ac, av);
	engine.Run();
	return 0;
}
