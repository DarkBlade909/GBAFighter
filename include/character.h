#ifndef CHARACTER_H
#define CHARACTER_H

#include "bn_sprite_item.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_rect.h"
#include "bn_span.h"
#include "animation.h"

enum class MoveId : uint8_t
{
    Punch,
    Kick,
    Slash,
    Heavy,

    Count
};

enum class MotionId : uint8_t
{
    Idle,
    WalkForwardStartup,
    WalkForward,
    WalkBackwardStartup,
    WalkBackward,
    DashStartup,
    Dash,
    Backdash,
    Crouch,
    CrouchLoop,
    Stand,
    Jump,
    Midair,
    Land,

    Count
};

struct Hitbox {
    int offset_x;
    int offset_y;
    int width;
    int height;

    bn::rect rect() const
    {
        return bn::rect(offset_x, offset_y, width, height);
    }
};

struct MotionData {
    Hitbox hurtbox;
    AnimationFactory animation_l;
    AnimationFactory animation_r;
};

struct MoveData {
    int damage;
    int guard;
    int startup;
    int active;
    int recovery;
    int blockstun;

    bool dash_cancellable;
    bool invulnerable;

    Hitbox hitbox;
    Hitbox hurtbox;

    AnimationFactory animation_l;
    AnimationFactory animation_r;
};

struct CharacterData {
    const int max_hp;
    const bn::sprite_item* movement_sprite;
    const bn::sprite_item* attack_sprite;
    bn::span<const MoveData> motionset;
    bn::span<const MoveData> moveset;
};

#endif // CHARACTER_H
