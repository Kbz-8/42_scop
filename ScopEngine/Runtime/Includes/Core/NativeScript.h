#ifndef __SCOP_NATIVE_SCRIPT__
#define __SCOP_NATIVE_SCRIPT__

#include <functional>

#include <Core/Script.h>

namespace Scop
{
	class NativeScript : public Script
	{
		public:
			NativeScript(std::function<void(NonOwningPtr<class Actor>)> on_init, std::function<void(NonOwningPtr<class Actor>, float)> on_update, std::function<void(NonOwningPtr<class Actor>)> on_quit)
			: f_on_init(std::move(on_init)), f_on_update(std::move(on_update)), f_on_quit(std::move(on_quit))
			{}

			inline void OnInit(NonOwningPtr<class Actor> actor) override { if(f_on_init) f_on_init(actor); }
			inline void OnUpdate(NonOwningPtr<class Actor> actor, float delta) override { if(f_on_update) f_on_update(actor, delta); }
			inline void OnQuit(NonOwningPtr<class Actor> actor) override { if(f_on_quit) f_on_quit(actor); }

			~NativeScript() = default;

		private:
			std::function<void(NonOwningPtr<class Actor>)> f_on_init;
			std::function<void(NonOwningPtr<class Actor>, float)> f_on_update;
			std::function<void(NonOwningPtr<class Actor>)> f_on_quit;
	};
}

#endif
