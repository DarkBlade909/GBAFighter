#include "char_ramlethal.h"

#include "bn_sprite_ptr.h"

#include "character.h"

#include "bn_sprite_items_ram_movement.h"
#include "bn_sprite_items_ram_attacks.h"
#include "bn_assert.h"
#include "bn_log.h"

namespace ramlethal
{

    ///
    /// ANIMATIONS
    ///

    // MOVEMENT FRAMES

    bn::sprite_animate_action<32> idle_l(bn::sprite_ptr& sprite)
    {
    return bn::create_sprite_animate_action_forever(
               sprite,
               2,
               bn::sprite_items::ram_movement.tiles_item(),
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30);
    }

    bn::sprite_animate_action<32> idle_r(bn::sprite_ptr& sprite)
    {
    return bn::create_sprite_animate_action_forever(
               sprite,
               2,
               bn::sprite_items::ram_movement.tiles_item(),
        1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31);
    }

    bn::sprite_animate_action<32> walk_f_startup_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            32, 34, 36, 38);
    }

    bn::sprite_animate_action<32> walk_f_startup_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            33, 35, 37, 39);
    }

    bn::sprite_animate_action<32> walk_f_loop_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70);
    }

    bn::sprite_animate_action<32> walk_f_loop_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71);
    }

    bn::sprite_animate_action<32> walk_b_startup_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            72, 74, 76, 78);
    }

    bn::sprite_animate_action<32> walk_b_startup_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            73, 75, 77, 79);
    }

    bn::sprite_animate_action<32> walk_b_loop_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110);
    }

    bn::sprite_animate_action<32> walk_b_loop_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111);
    }

    bn::sprite_animate_action<32> dash_startup_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            112, 114);
    }

    bn::sprite_animate_action<32> dash_startup_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            113, 115);
    }

    bn::sprite_animate_action<32> dash_loop_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            116, 118, 120, 122);
    }

    bn::sprite_animate_action<32> dash_loop_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            117, 119, 121, 123);
    }

    bn::sprite_animate_action<32> backdash_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            152, 154, 156, 158, 160, 162, 164, 166, 168);
    }

    bn::sprite_animate_action<32> backdash_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            153, 155, 157, 159, 161, 163, 165, 167, 169);
    }

    bn::sprite_animate_action<32> crouch_startup_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            172, 174, 176, 178, 180, 182, 184, 186);
    }

    bn::sprite_animate_action<32> crouch_startup_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            173, 175, 177, 179, 181, 183, 185, 187);
    }

    bn::sprite_animate_action<32> crouch_loop_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210);
    }

    bn::sprite_animate_action<32> crouch_loop_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_forever(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211);
    }

    bn::sprite_animate_action<32> crouch_end_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            212, 214, 216, 218, 220, 222);
    }

    bn::sprite_animate_action<32> crouch_end_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            213, 215, 217, 219, 221, 223);
    }

    bn::sprite_animate_action<32> jump_start_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250);
    }

    bn::sprite_animate_action<32> jump_start_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            225, 227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251);
    }

    bn::sprite_animate_action<32> jump_loop_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            252, 254, 256, 258);
    }

    bn::sprite_animate_action<32> jump_loop_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            253, 255, 257, 259);
    }

    bn::sprite_animate_action<32> jump_end_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            260, 262, 264, 266, 268, 270);
    }

    bn::sprite_animate_action<32> jump_end_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_movement.tiles_item(),
            261, 263, 265, 267, 269, 271);
    }




    // ATTACK FRAMES
    bn::sprite_animate_action<32> punch_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
            0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20);
    }

    bn::sprite_animate_action<32> punch_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
        sprite,
        2,
        bn::sprite_items::ram_attacks.tiles_item(),
            1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21);
    }

    bn::sprite_animate_action<32> kick_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
        22, 24, 26, 28, 30, 32, 34, 36, 38, 40,
            42);
    }

    bn::sprite_animate_action<32> kick_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
        23, 25, 27, 29, 31, 33, 35, 37, 39, 41,
            43);
    }

    bn::sprite_animate_action<32> slash_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
        44, 46, 48, 50, 52, 54, 56, 58, 60, 62,
            64, 66, 68, 70, 72, 74, 76, 78);
    }

    bn::sprite_animate_action<32> slash_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
        45, 47, 49, 51, 53, 55, 57, 59, 61, 63,
            65, 67, 69, 71, 73, 75, 77, 79);
    }

    bn::sprite_animate_action<32> heavy_l(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
        80, 82, 84, 86, 88, 90, 92, 94, 96, 98,
            100, 102, 104, 106, 108, 110, 112, 114, 116);
    }

    bn::sprite_animate_action<32> heavy_r(bn::sprite_ptr& sprite)
    {
        return bn::create_sprite_animate_action_once(
                   sprite,
                   2,
                   bn::sprite_items::ram_attacks.tiles_item(),
        81, 83, 85, 87, 89, 91, 93, 95, 97, 99,
            101, 103, 105, 107, 109, 111, 113, 115, 117);
    }

    // MOTIONS

    const MoveData idle = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = idle_l,
        .animation_r = idle_r
    };

    const MoveData walkForwardStartup = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = walk_f_startup_l,
        .animation_r = walk_f_startup_r,
    };

    const MoveData walkForward = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = walk_f_loop_l,
        .animation_r = walk_f_loop_r,
    };

    const MoveData walkBackwardStartup = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = walk_b_startup_l,
        .animation_r = walk_b_startup_r,
    };

    const MoveData walkBackward = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = walk_b_loop_l,
        .animation_r = walk_b_loop_r,
    };

    const MoveData dashStartup = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = dash_startup_l,
        .animation_r = dash_startup_r,
    };

    const MoveData dashLoop = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = dash_loop_l,
        .animation_r = dash_loop_r,
    };

    const MoveData backdash = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = backdash_l,
        .animation_r = backdash_r,
    };

    const MoveData crouchStartup = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = crouch_startup_l,
        .animation_r = crouch_startup_r,
    };

    const MoveData crouchLoop = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = crouch_loop_l,
        .animation_r = crouch_loop_r,
    };

    const MoveData crouchEnd = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = crouch_end_l,
        .animation_r = crouch_end_r,
    };

    const MoveData jumpStartup = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = jump_start_l,
        .animation_r = jump_start_r,
    };

    const MoveData jumpLoop = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = jump_loop_l,
        .animation_r = jump_loop_r,
    };

    const MoveData jumpEnd = {
        .hurtbox = Hitbox(0,0,6,6),
        .animation_l = jump_end_l,
        .animation_r = jump_end_r,
    };
    // ATTACKS

    const MoveData punch =
        {
            .damage = 5,
            .guard = 1,
            .startup = 6,
            .active = 4,
            .recovery = 8,
            .blockstun = 5,

            .dash_cancellable = false,
            .invulnerable = false,
            .hitbox = Hitbox(18, 0, 24, 16),
            .hurtbox = Hitbox(0, 0, 1, 1),

            .animation_l = punch_l,
            .animation_r = punch_r,
        };

    const MoveData kick =
        {
            .damage = 5,
            .guard = 1,
            .startup = 6,
            .active = 4,
            .recovery = 8,
            .blockstun = 5,

            .dash_cancellable = false,
            .invulnerable = false,
            .hitbox = Hitbox(18, 0, 24, 16),
            .hurtbox = Hitbox(0, 0, 1, 1),

            .animation_l = kick_l,
            .animation_r = kick_r,
        };

    const MoveData slash =
        {
            .damage = 5,
            .guard = 1,
            .startup = 6,
            .active = 4,
            .recovery = 24,
            .blockstun = 5,

            .dash_cancellable = false,
            .invulnerable = false,
            .hitbox = Hitbox(18, 0, 24, 16),
            .hurtbox = Hitbox(0, 0, 1, 1),

            .animation_l = slash_l,
            .animation_r = slash_r,
        };

    const MoveData heavy =
        {
            .damage = 5,
            .guard = 1,
            .startup = 6,
            .active = 4,
            .recovery = 24,
            .blockstun = 5,

            .dash_cancellable = false,
            .invulnerable = false,
            .hitbox = Hitbox(18, 0, 24, 16),
            .hurtbox = Hitbox(0, 0, 1, 1),

            .animation_l = heavy_l,
            .animation_r = heavy_r,
        };

    // CHARACTER DATA
    const MoveData _motionset[] =
    {
        idle,
        walkForwardStartup,
        walkForward,
        walkBackwardStartup,
        walkBackward,
        dashStartup,
        dashLoop,
        backdash,
        crouchStartup,
        crouchLoop,
        crouchEnd,
        jumpStartup,
        jumpLoop,
        jumpEnd,

        punch,
        kick,
        slash,
        heavy
    };

    const MoveData _moveset[] =
    {
        punch,
        kick,
        slash,
        heavy
    };

    const CharacterData character =
        {
            .max_hp = 100,
            .movement_sprite = &bn::sprite_items::ram_movement,
            .attack_sprite = &bn::sprite_items::ram_attacks,
            .motionset = _motionset,
            .moveset = _moveset,

        };
}