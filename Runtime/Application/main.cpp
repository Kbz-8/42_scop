#include <Core/Engine.h>
#include <Core/Logs.h>
#include <Renderer/Shaders/DefaultFragmentShader.h>

int main(int ac, char** av)
{
	if(ac < 2)
	{
		Scop::Error("not enough arguments");
		return 1;
	}
	Scop::ScopEngine engine(ac, av, "Scop", 1280, 720);

	Scop::SceneDescriptor main_scene_desc;
	main_scene_desc.fragment_shader = Scop::DefaultFragmentShader;

	Scop::Scene main_scene("main", main_scene_desc);
	main_scene.CreateActor(Scop::LoadModelFromObjFile(av[ac - 1]));

	engine.RegisterMainScene(main_scene);
	engine.Run();
	return 0;
}
