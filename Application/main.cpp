#include <ScopCore.h>
#include <ScopGraphics.h>

#include <ScriptSubRoutines.h>

#include <climits>
#include <memory>
#include <unistd.h>
#include <filesystem>
#include <string>
#include <thread>
#include <cmath>

std::filesystem::path GetExecutablePath()
{
	char result[PATH_MAX];
	ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
	return std::string(result, (count > 0) ? count : 0);
}

constexpr std::uint32_t WIDTH = 1280;
constexpr std::uint32_t HEIGHT = 720;

int main(int ac, char** av)
{
	if(ac < 2)
	{
		Scop::Error("not enough arguments");
		return 1;
	}
	Scop::ScopEngine engine(ac, av, "Scop", WIDTH, HEIGHT, GetExecutablePath().parent_path().parent_path() / "ScopEngine/Assets");

	bool skip_splashscreen = false;
	for(int i = 2; i < ac; i++)
	{
		if(std::strcmp(av[i], "--skip-splash") == 0)
			skip_splashscreen = true;
	}

	Scop::SceneDescriptor splashscreen_scene_desc;
	splashscreen_scene_desc.fragment_shader = Scop::RenderCore::Get().GetDefaultFragmentShader();
	splashscreen_scene_desc.render_3D_enabled = false;
	splashscreen_scene_desc.render_skybox_enabled = false;
	Scop::Scene splashscreen_scene("splash", splashscreen_scene_desc);
	Scop::Vec2ui32 splash_size;
	Scop::Sprite& splash = splashscreen_scene.CreateSprite(std::make_shared<Scop::Texture>(Scop::LoadBMPFile(GetExecutablePath().parent_path().parent_path() / "ScopEngine/Assets/Images/splashscreen.bmp", splash_size), splash_size.x, splash_size.y));
	splash.SetPosition(Scop::Vec2ui{ WIDTH / 2 - splash_size.x / 2, HEIGHT / 2 - splash_size.y / 2 });

	auto splash_update = [skip_splashscreen](Scop::NonOwningPtr<Scop::Scene> scene, Scop::NonOwningPtr<Scop::Sprite> sprite, Scop::Inputs& input, float delta)
	{
		using namespace std::chrono_literals;
		if(skip_splashscreen)
			scene->SwitchToChild("main");

		static float x = 0.02f;
		Scop::Vec4f color = sprite->GetColor();
		color.w = std::abs(std::sin(x)) * 1.1;
		x += 0.02f;
		sprite->SetColor(color); 
		if(color.w <= 0.02f)
			scene->SwitchToChild("main");

		std::this_thread::sleep_for(33ms); // 30fps
	};

	using sprite_hook = std::function<void(Scop::NonOwningPtr<Scop::Sprite>)>;
	splash.AttachScript(std::make_shared<Scop::NativeSpriteScript>(sprite_hook{}, splash_update, sprite_hook{}));

	Scop::SceneDescriptor main_scene_desc;
	main_scene_desc.fragment_shader = Scop::RenderCore::Get().GetDefaultFragmentShader();
	main_scene_desc.camera = std::make_shared<Scop::FirstPerson3D>(Scop::Vec3f{ -10.0f, 0.0f, 0.0f });
	Scop::Scene& main_scene = splashscreen_scene.AddChildScene("main", main_scene_desc);
	Scop::Vec2ui32 skybox_size;
	main_scene.AddSkybox(std::make_shared<Scop::CubeTexture>(Scop::LoadBMPFile(GetExecutablePath().parent_path().parent_path() / "Resources/skybox.bmp", skybox_size), skybox_size.x, skybox_size.y));

	Scop::Actor& object = main_scene.CreateActor(Scop::LoadModelFromObjFile(av[1]));
	object.SetScale(Scop::Vec3f{ 5.0f, 5.0f, 5.0f });

	Scop::Actor& object2 = main_scene.CreateActor(Scop::CreateSphere());
	object2.SetScale(Scop::Vec3f{ 5.0f, 5.0f, 5.0f });
	object2.SetPosition(Scop::Vec3f{ 10.0f, 10.0f, 10.0f });

	if(ac > 2)
	{
		for(std::size_t i = 2, j = 0; i < ac; i++, j++)
		{
			if(!std::filesystem::exists(av[i]))
				continue;
			Scop::Vec2ui32 albedo_size;
			Scop::MaterialTextures material_params;
			material_params.albedo = std::make_shared<Scop::Texture>(Scop::LoadBMPFile(av[i], albedo_size), albedo_size.x, albedo_size.y);
			std::shared_ptr<Scop::Material> material = std::make_shared<Scop::Material>(material_params);
			object.GetModelRef().SetMaterial(material, j);
		}
	}

	auto object_update = [](Scop::NonOwningPtr<Scop::Scene> scene, Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& input, float delta)
	{
		static Scop::MaterialData material_data{};

		WireframeHandler(scene, input);
		RotationHandler(actor, input, delta);
		MovementHandler(actor, input, delta);
		ColorsTransitionHandler(actor, input, delta, material_data);
		TextureTransitionHandler(actor, input, delta, material_data);

		for(std::shared_ptr<Scop::Material> material : actor->GetModelRef().GetAllMaterials())
		{
			if(material)
				material->SetMaterialData(material_data);
		}
	};

	using actor_hook = std::function<void(Scop::NonOwningPtr<Scop::Actor>)>;
	object.AttachScript(std::make_shared<Scop::NativeActorScript>(actor_hook{}, object_update, actor_hook{}));

	engine.RegisterMainScene(&splashscreen_scene);
	engine.Run();
	return 0;
}
