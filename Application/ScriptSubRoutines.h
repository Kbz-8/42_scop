#ifndef __SCRIPT_SUB_ROUTINES__
#define __SCRIPT_SUB_ROUTINES__

#include <ScopCore.h>
#include <ScopGraphics.h>

void RotationHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta);
void MovementHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta);
void ColorsTransitionHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta, Scop::MaterialData& data);
void TextureTransitionHandler(Scop::NonOwningPtr<Scop::Actor> actor, Scop::Inputs& inputs, float delta, Scop::MaterialData& data);

#endif
