#ifndef FIGHTER_H
#define FIGHTER_H

#include "bn_regular_bg_ptr.h"
#include "bn_sprite_ptr.h"
#include "bn_sprite_animate_actions.h"
#include "bn_fixed_point.h"
#include "bn_forward_list.h"
#include "bn_vector.h"
#include "bn_rect.h"
#include "bn_unordered_map.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"

#include "bn_sprite_items_ram_movement.h"
#include "bn_sprite_items_ram_attacks.h"
#include "bn_affine_bg_items_hitbox.h"
// #include "bn_affine_bg_items_hurtbox.h"

namespace db

{
    // typedef struct {
    //     bn::rect *hitboxData;
    //     int hitboxAmt;

    //     bn::rect *hurtboxData;
    //     int hurtboxAmt;
    // } boxData;

    struct Hitbox {
        int offset_x;
        int offset_y;
        int width;
        int height;
        bn::rect rect = bn::rect(offset_x, offset_y, width, height);
    };

    struct Move {
        int damage;
        int guard;
        int startup;
        int active;
        int recovery;
        int blockstun;

        bool dash_cancellable;
        bool invulnerable;

        bn::vector<int, 4> hitbox_data;
        Hitbox hitbox;
        Hitbox hurtbox;

        bn::sprite_animate_action<32> action_l;
        bn::sprite_animate_action<32> action_r;
    };

    class Fighter
    {
        private:
            // Config
            const bn::fixed _max_hp = 100;

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

            bool _flip = false;

            // Variables

            bn::fixed_point _position;
            bn::fixed_point _input_vector;
            bn::fixed_point _velocity;
            int _dash_ready = 0;
            int _dash_timer = 0;
            int _velocity_multiplier = 1;
            int _flip_offset = 32;
            int _elapsed_frames = 0;
            int _lockout_timer = 0;
            int _lockout_duration = 0;
            bool _lockout = false;

            Hitbox _hurtbox = Hitbox(2, 6, _hurtbox_scale.x(), _hurtbox_scale.y());

            //Sprite

            bn::sprite_ptr _sprite_l = bn::sprite_items::ram_movement.create_sprite(_position.x()-_flip_offset, _position.y());
            bn::sprite_ptr _sprite_r = bn::sprite_items::ram_movement.create_sprite(_position.x()+_flip_offset, _position.y());

            bn::sprite_animate_action<32> _action_l = bn::create_sprite_animate_action_forever(
                _sprite_l, 4, bn::sprite_items::ram_movement.tiles_item(), 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30);
            bn::sprite_animate_action<32> _action_r = bn::create_sprite_animate_action_forever(
                _sprite_r, 4, bn::sprite_items::ram_movement.tiles_item(), 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31);

            // DEBUG
            bool _DEBUG = true;
            bn::affine_bg_ptr _hurtbox_debug = bn::affine_bg_items::hitbox.create_bg(_position.x()+_hurtbox.offset_x, _position.y()+_hurtbox.offset_y);
            bn::affine_bg_ptr _hitbox_debug = bn::affine_bg_items::hitbox.create_bg(_position);

            // Attacks

            Move punch =
            {
                .damage = 5,
                .guard = 1,
                .startup = 6,
                .active = 4,
                .recovery = 10,
                .blockstun = 5,

                .dash_cancellable = false,
                .invulnerable = false,
                .hitbox = Hitbox(18, 0, 24, 16),
                .hurtbox = Hitbox(0, 0, 1, 1),

                .action_l = bn::create_sprite_animate_action_once(_sprite_l, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20),
                .action_r = bn::create_sprite_animate_action_once(_sprite_r, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21),
            };

            Move kick =
            {
                .damage = 5,
                .guard = 1,
                .startup = 6,
                .active = 4,
                .recovery = 10,
                .blockstun = 5,

                .dash_cancellable = false,
                .invulnerable = false,
                .hitbox = Hitbox(24, 0, 32, 6),
                .hurtbox = Hitbox(0, 0, 1, 1),

                .action_l = bn::create_sprite_animate_action_once(_sprite_l, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 42),
                .action_r = bn::create_sprite_animate_action_once(_sprite_r, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  23, 25, 27, 29, 31, 33, 35, 37, 39, 41, 43),
            };

            Move slash =
            {
                .damage = 5,
                .guard = 1,
                .startup = 6,
                .active = 4,
                .recovery = 10,
                .blockstun = 5,

                .dash_cancellable = false,
                .invulnerable = false,
                .hitbox = Hitbox(-16, 0, 6, 6),
                .hurtbox = Hitbox(0, 0, 1, 1),
                
                .action_l = bn::create_sprite_animate_action_once(_sprite_l, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70, 72, 74, 76, 78),
                .action_r = bn::create_sprite_animate_action_once(_sprite_r, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71, 73, 75, 77, 79),
            };

            Move heavy =
            {
                .damage = 5,
                .guard = 1,
                .startup = 6,
                .active = 4,
                .recovery = 10,
                .blockstun = 5,

                .dash_cancellable = false,
                .invulnerable = false,
                .hitbox = Hitbox(-16, 0, 6, 6),
                .hurtbox = Hitbox(0, 0, 1, 1),

                .action_l = bn::create_sprite_animate_action_once(_sprite_l, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110, 112, 114, 116),
                .action_r = bn::create_sprite_animate_action_once(_sprite_r, 2, bn::sprite_items::ram_attacks.tiles_item(),
                                                                  81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111, 113, 115, 117),
            };

        public:
            Fighter(bn::fixed_point spawn_position);

            void attack(int attack_id);
            void check_inputs();
            void animate();
            void move();
            void jump();
            void dash();
            void backdash();
            void crouch();
            void flip();
            void lockout(int frames);
    };
}

#endif