#include "animation.h"

namespace animation
{

Animation play(
    bn::sprite_ptr& sprite,
    AnimationFactory factory)
{
    return factory(sprite);
}

void update(Animation& animation)
{
    if(animation)
    {
        animation->update();
    }
}

bool playing(const Animation& animation)
{
    return animation.has_value() && ! animation->done();
}

}