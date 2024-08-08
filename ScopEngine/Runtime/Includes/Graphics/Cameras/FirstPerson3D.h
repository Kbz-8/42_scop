#ifndef __SCOP_CAMERAS_FIRST_PERSON_3D__
#define __SCOP_CAMERAS_FIRST_PERSON_3D__

#include <Graphics/Cameras/Base.h>
#include <Maths/Vec3.h>

namespace Scop
{
	class FirstPerson3D : public BaseCamera
	{
		public:
			FirstPerson3D();
			FirstPerson3D(Vec3f position, float fov = 90.0f);

			void Update(class Inputs& input, float aspect) override;

			inline constexpr std::string GetCameraType() override { return "FirstPerson3D"; }

			~FirstPerson3D() = default;

		private:
			void UpdateView();

		private:
			const Vec3f m_up;
			Vec3f m_position;
			Vec3f m_left;
			Vec3f m_forward;
			Vec3f m_target;
			Vec3f m_direction;
			Vec3f m_mov;

			float m_theta = 0.0;
			float m_phi = 0.0;

			const float m_speed = 0.3f;
			const float m_sensivity = 0.7f;
			float m_fov = 90.0f;
	};
}

#endif
