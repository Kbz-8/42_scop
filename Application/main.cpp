#include <filesystem>

#include <ScopCore.h>
#include <ScopGraphics.h>

#include <climits>
#include <memory>
#include <unistd.h>

std::filesystem::path GetExecutablePath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

int main(int ac, char** av)
{
	if(ac < 2)
	{
		Scop::Error("not enough arguments");
		return 1;
	}
	Scop::ScopEngine engine(ac, av, "Scop", 1280, 720, GetExecutablePath().parent_path().parent_path() / "ScopEngine/Assets");

	Scop::SceneDescriptor main_scene_desc;
	main_scene_desc.fragment_shader = Scop::RenderCore::Get().GetDefaultFragmentShader();
	main_scene_desc.camera = std::make_shared<Scop::FirstPerson3D>(Scop::Vec3f{ 1.0f, 1.0f, 1.0f });

	Scop::Scene main_scene("main", main_scene_desc);
	Scop::Actor& object = main_scene.CreateActor(Scop::LoadModelFromObjFile(av[ac - 1]));

	auto object_update = [](Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& input, float delta)
	{
	};

	using hook = std::function<void(Scop::NonOwningPtr<Scop::Actor>)>;
	object.AttachScript(std::make_shared<Scop::NativeScript>(hook{}, object_update, hook{}));

	engine.RegisterMainScene(main_scene);
	engine.Run();
	return 0;
}
