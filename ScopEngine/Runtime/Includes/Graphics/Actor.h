#ifndef __SCOP_RENDERER_ACTOR__
#define __SCOP_RENDERER_ACTOR__

#include <Maths/Vec3.h>
#include <Maths/Vec4.h>
#include <Core/Script.h>
#include <Graphics/Model.h>

namespace Scop
{
	class Actor
	{
		public:
			Actor();
			Actor(Model model);

			inline void AttachScript(std::shared_ptr<Script> script) { p_script = script; }
			void Update(class Inputs& input, float timestep);

			inline void SetColor(Vec4f color) noexcept { m_color = color; }
			inline void SetPosition(Vec3f position) noexcept { m_position = position; }
			inline void SetScale(Vec3f scale) noexcept { m_scale = scale; }
			inline void SetOrientation(Vec3f orientation) noexcept { m_orientation = orientation; }

			[[nodiscard]] inline const Vec4f& GetColor() const noexcept { return m_color; }
			[[nodiscard]] inline const Vec3f& GetPosition() const noexcept { return m_position; }
			[[nodiscard]] inline const Vec3f& GetScale() const noexcept { return m_scale; }
			[[nodiscard]] inline const Vec3f& GetOrientation() const noexcept { return m_orientation; }
			[[nodiscard]] inline const Model& GetModel() const noexcept { return m_model; }

			~Actor();

		private:
			Model m_model;
			std::shared_ptr<Script> p_script;
			Vec4f m_color;
			Vec3f m_position;
			Vec3f m_scale;
			Vec3f m_orientation;
	};
}

#endif
