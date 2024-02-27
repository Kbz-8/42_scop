#include <Core/Engine.h>

int main(int ac, char** av)
{
	ScopEngine engine(ac, av);
	engine.Run();
	return 0;
}
