#ifndef FIGHTER_H
#define FIGHTER_H

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed_point.h"
#include "bn_list.h"
#include "bn_forward_list.h"
#include "bn_vector.h"
#include "bn_rect.h"
#include "bn_unordered_map.h"
#include "bn_span.h"
#include "bn_array.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"
#include "bn_sprite_animate_actions.h"
#include "animation.h"

#include "char_ramlethal.h"

#include "bn_affine_bg_items_hitbox.h"
#include "bn_affine_bg_items_hurtbox.h"
#include "bn_sprite_items_ram_movement.h"

namespace db

{

    enum class Motion : uint8_t
    {
        Idle,
        WalkForwardStartup,
        WalkForward,
        WalkBackwardStartup,
        WalkBackward,
        DashStartup,
        DashLoop,
        Backdash,
        CrouchStartup,
        CrouchLoop,
        CrouchEnd,
        JumpStartup,
        JumpLoop,
        JumpEnd,

        Punch,
        Kick,
        Slash,
        Heavy
    };

    class Fighter
    {
        private:

        const CharacterData& _character;

        // Config
        const bn::fixed _acceleration = 3;

        const bn::fixed _drag = 0.3;

        const bn::fixed _attacking_drag = 0.95;

        const bn::fixed _walk_speed = 0.6;

        const int _dash_speed = 4;

        const bn::fixed _dash_window = 15;

        const int _backdash_speed = 5;

        const bn::fixed _backdash_duration = 15;

        const bn::fixed_point _spawn_position = bn::fixed_point(-32,25);

        const bn::fixed_point _gravity = bn::fixed_point(0, 0.2);

        const bn::fixed _jump_impulse = -4;

        const bn::point _hurtbox_scale = bn::point(24,48);

        // Anim States
        bool _walking_r = false;
        bool _walking_l = false;
        bool _jumping = false;
        bool _crouching = false;
        bool _dashing = false;
        bool _backdashing = false;
        bool _grounded = false;
        bool _attacking = false;
        int _action = 0;
        int _frame_timer = 0;
        int _anim_frame = 0;

        bool _flip = false;

        // Variables

        bn::sprite_ptr _sprite_l;
        bn::sprite_ptr _sprite_r;

        Animation _animation_l;
        Animation _animation_r;

        Motion _motion = Motion::Idle;

        int _motion_frame = 0;
        bool _motion_changed = false;

        bn::fixed_point _position;
        bn::fixed_point _input_vector;
        bn::fixed_point _velocity;
        int _hp = 100;
        int _dash_ready = 0;
        int _dash_timer = 0;
        int _velocity_multiplier = 1;
        int _flip_offset = 32;
        int _elapsed_frames = 0;
        int _lockout_timer = 0;
        int _lockout_duration = 0;
        int _attack_timer = 0;
        bool _lockout = false;


        // DEBUG
        bool _DEBUG = true;
        // bn::affine_bg_ptr _hurtbox_debug = bn::affine_bg_items::hurtbox.create_bg(_position.x()+_hurtbox.offset_x, _position.y()+_hurtbox.offset_y);
        // bn::affine_bg_ptr _hitbox_debug = bn::affine_bg_items::hitbox.create_bg(_position);

        public:
            Fighter(const CharacterData& character, bn::fixed_point spawn_position);

            void attack(Motion motion);
            void check_inputs();
            void animate();
            void move();
            void jump();
            void dash();
            void backdash();
            void crouch();
            void flip();
            void lockout(int frames);
            void state_machine();
            void set_motion(Motion motion);
    };
}

#endif