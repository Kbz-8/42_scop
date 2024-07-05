#include <Core/Engine.h>
#include <Core/Logs.h>

int main(int ac, char** av)
{
	if(ac < 2)
	{
		Scop::Error("not enough arguments");
		return 1;
	}
	Scop::ScopEngine engine(ac, av, "Scop", 1280, 720);
	Scop::Actor model(Scop::LoadModelFromObjFile(av[ac - 1]));
	engine.RegisterActor(model);
	engine.Run();
	return 0;
}
