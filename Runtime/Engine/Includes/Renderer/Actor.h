#ifndef __SCOP_RENDERER_ACTOR__
#define __SCOP_RENDERER_ACTOR__

#include <kvf.h>
#include <Maths/Vec3.h>
#include <Maths/Vec4.h>

#include <Renderer/Model.h>

namespace Scop
{
	class Actor
	{
		public:
			Actor();

			~Actor();

		private:
			Model m_model;
			Vec4f m_color;
			Vec3f m_position;
			Vec3f m_scale;
			Vec3f m_orientation;
	};
}

#endif
