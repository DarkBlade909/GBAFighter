#ifndef ANIMATION_H
#define ANIMATION_H

#include "bn_optional.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"

using Animation =
    bn::optional<bn::sprite_animate_action<32>>;

using AnimationFactory =
    bn::sprite_animate_action<32>(*)(bn::sprite_ptr&);

namespace animation
{

Animation play(
    bn::sprite_ptr& sprite,
    AnimationFactory factory);

void update(Animation& animation);

bool playing(const Animation& animation);

}

#endif