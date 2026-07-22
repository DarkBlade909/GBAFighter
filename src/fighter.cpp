#include "fighter.h"

#include "bn_math.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_regular_bg_builder.h"
#include "bn_sprite_builder.h"
#include "bn_regular_bg_animate_actions.h"
#include "bn_sprite_item.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_text_generator.h"
#include "bn_random.h"
#include "bn_forward_list.h"
#include "bn_pool.h"
#include "bn_log.h"
#include "bn_rect.h"
#include "bn_sound_items.h"
#include "bn_vector.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_builder.h"
#include "bn_blending.h"
#include "bn_affine_bg_ptr.h"
#include "bn_affine_bg_item.h"

#include "bn_sprite_items_ram_movement.h"
// #include "bn_affine_bg_items_hitbox.h"
// #include "bn_affine_bg_items_hurtbox.h"

namespace db
{
    Fighter::Fighter(bn::fixed_point spawn_position) :
    _position(spawn_position)
    {
        _sprite_l.put_above();
        _sprite_r.put_above();

        // DEBUG
        _hurtbox_debug.put_above();
        BN_LOG("height: ", _hurtbox.height/32, "width: ", _hurtbox.width/32);
        _hurtbox_debug.set_scale((bn::fixed(_hurtbox.width)/256), (bn::fixed(_hurtbox.height)/256));
        _hurtbox_debug.set_blending_enabled(true);
        _hurtbox_debug.set_priority(2);
        _hurtbox_debug.set_wrapping_enabled(false);
        _hurtbox_debug.set_z_order(-2);

        _hitbox_debug.set_wrapping_enabled(false);
        _hitbox_debug.set_blending_enabled(true);
        _hitbox_debug.set_priority(3);
        _hitbox_debug.set_z_order(-4);
        _hitbox_debug.put_above();
        _hitbox_debug.set_visible(false);



        bn::blending::set_transparency_alpha(0.5);
    }

    void Fighter::animate()
    {
        if(_input_vector.x() > 0)
        {
            _walking_r = true;
            _walking_l = false;
        }
        else if(_input_vector.x() < 0)
        {
            _walking_r = false;
            _walking_l = true;
        }
        else
        {
            _walking_r = false;
            _walking_l = false;
        }

        // DEBUG
        if(_DEBUG)
        {
            _hurtbox_debug.set_x(_position.x()+_hurtbox.offset_x);
            _hurtbox_debug.set_y(_position.y()+_hurtbox.offset_y);
            if(_attacking)
            {
                _hitbox_debug.set_visible(true);
                _hitbox_debug.set_priority(0);
                _hitbox_debug.set_z_order(4);
                _hitbox_debug.put_above();
                _hitbox_debug.set_scale(bn::fixed(punch.hitbox.width)/256, bn::fixed(punch.hitbox.height)/256);
                _hitbox_debug.set_position(_position.x()+punch.hitbox.offset_x, _position.y()+punch.hitbox.offset_y);
            }
            else
            {
                _hitbox_debug.set_visible(false);
            }
        }

        // Check for finished anims
        if(_attacking && _action_r.done()){
            BN_LOG("Attack Finished");
            _attacking = false;
            _action = 0;
        }
        if(_backdashing && _action_r.done()){
            _backdashing = false;
        }
        if(_crouching && _action_r.done()){
            _crouching = false;
        }

        // Lockout Timer
        if(_lockout)
        {
            if((_elapsed_frames - _lockout_timer) > _lockout_duration)
            {
                _lockout = false;
            }
        }

        // Anim States
        if(_attacking){
            BN_LOG("ATTACK:", _action);
            if(_grounded)
            {
                _jumping = false;
            }
            if(_action == 1)
            {
                if((_action_r.graphics_indexes().front() != punch.action_r.graphics_indexes().front()) || (_action_r.tiles_item() != punch.action_r.tiles_item()))
                    {
                        BN_LOG("ATTACK 1");
                        _action_l = punch.action_l;
                        _action_r = punch.action_r;
                    }
            }
            else if(_action == 2)
            {
                if((_action_r.graphics_indexes().front() != kick.action_r.graphics_indexes().front()) || (_action_r.tiles_item() != kick.action_r.tiles_item()))
                {
                    BN_LOG("ATTACK 2");
                    _action_l = kick.action_l;
                    _action_r = kick.action_r;
                }
            }
            else if(_action == 3)
            {
                if((_action_r.graphics_indexes().front() != slash.action_r.graphics_indexes().front()) || (_action_r.tiles_item() != slash.action_r.tiles_item()))
                {
                    BN_LOG("ATTACK 3");
                    _action_l = slash.action_l;
                    _action_r = slash.action_r;
                }
            }
            else if(_action == 4)
            {
                if((_action_r.graphics_indexes().front() != heavy.action_r.graphics_indexes().front()) || (_action_r.tiles_item() != heavy.action_r.tiles_item()))
                {
                    BN_LOG("ATTACK 4");
                    _action_l = heavy.action_l;
                    _action_r = heavy.action_r;
                }
            }
            else
            {
                BN_LOG("ATTACK DOES NOT EXIST");
                _attacking = false;
                _action = 0;
            }
        }
        else if(_jumping)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Jumping");
            //Land
            if(_grounded && _action_r.graphics_indexes().front() != 261)
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 260, 262, 264, 266, 268, 270);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 261, 263, 265, 267, 269, 271);
            }
            //Jump
            if(!_grounded && _action_r.graphics_indexes().front() != 225 && _action_r.graphics_indexes().front() != 252 || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 224, 226, 228, 230, 232, 234, 236, 238, 240, 242, 244, 246, 248, 250);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 225, 227, 229, 231, 233, 235, 237, 239, 241, 243, 245, 247, 249, 251);
            }
            //Float
            else if(_action_r.current_graphics_index() == 251)
            {
                _action_l = bn::create_sprite_animate_action_forever(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 252, 254, 256, 258);
                _action_r = bn::create_sprite_animate_action_forever(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 253, 255, 257, 259);
            }
            if(_action_r.current_graphics_index() == 271)
            {
                _jumping = false;
            }
        }
        else if(_dashing)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Dashing");
            if((_action_r.graphics_indexes().front() != 113 && _action_r.graphics_indexes().front() != 117) || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 112, 114);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 113, 115);
            }
            if(_action_r.current_graphics_index() == 115)
            {
                _action_l = bn::create_sprite_animate_action_forever(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 116, 118, 120, 122);
                _action_r = bn::create_sprite_animate_action_forever(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 117, 119, 121, 123);
            }
        }
        else if(_backdashing)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Backdashing");
            if(_action_r.graphics_indexes().front() != 153 || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 2, bn::sprite_items::ram_movement.tiles_item(), 152, 154, 156, 158, 160, 162, 164, 166, 168);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 2, bn::sprite_items::ram_movement.tiles_item(), 153, 155, 157, 159, 161, 163, 165, 167, 169);
            }
        }
        else if(_crouching)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Crouching");
            // Stand
            if(bn::keypad::down_released())
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 2, bn::sprite_items::ram_movement.tiles_item(), 212, 214, 216, 218, 220, 222);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 2, bn::sprite_items::ram_movement.tiles_item(), 213, 215, 217, 219, 221, 223);
            }
            // Crouch Startup
            else if((_action_r.graphics_indexes().front() != 173 && _action_r.graphics_indexes().front() != 189 && _action_r.graphics_indexes().front() != 213) || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 2, bn::sprite_items::ram_movement.tiles_item(), 172, 174, 176, 178, 180, 182, 184, 186);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 2, bn::sprite_items::ram_movement.tiles_item(), 173, 175, 177, 179, 181, 183, 185, 187);
            }
            // Crouch Loop
            if(_action_r.current_graphics_index() == 187)
            {
                _action_l = bn::create_sprite_animate_action_forever(
                    _sprite_l, 2, bn::sprite_items::ram_movement.tiles_item(), 188, 190, 192, 194, 196, 198, 200, 202, 204, 206, 208, 210);
                _action_r = bn::create_sprite_animate_action_forever(
                    _sprite_r, 2, bn::sprite_items::ram_movement.tiles_item(), 189, 191, 193, 195, 197, 199, 201, 203, 205, 207, 209, 211);
            }
        }
        else if(_walking_r){
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            // Walk Startup
            if((_action_r.graphics_indexes().front() != 33 && _action_r.graphics_indexes().front() != 41) || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 32, 34, 36, 38);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 33, 35, 37, 39);
            }
            //Walk Loop
            if(_action_r.current_graphics_index() == 39)
            {
                _action_l = bn::create_sprite_animate_action_forever(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 66, 68, 70);
                _action_r = bn::create_sprite_animate_action_forever(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 41, 43, 45, 47, 49, 51, 53, 55, 57, 59, 61, 63, 65, 67, 69, 71);
            }
        }
        else if(_walking_l){
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery
            // Walk Startup
            if((_action_r.graphics_indexes().front() != 73 && _action_r.graphics_indexes().front() != 81) || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                _action_l = bn::create_sprite_animate_action_once(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 72, 74, 76, 78);
                _action_r = bn::create_sprite_animate_action_once(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 73, 75, 77, 79);
            }
            //Walk Loop
            if(_action_r.current_graphics_index() == 79)
            {
                _action_l = bn::create_sprite_animate_action_forever(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 80, 82, 84, 86, 88, 90, 92, 94, 96, 98, 100, 102, 104, 106, 108, 110);
                _action_r = bn::create_sprite_animate_action_forever(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 81, 83, 85, 87, 89, 91, 93, 95, 97, 99, 101, 103, 105, 107, 109, 111);
            }
        }
        else if(!_attacking) {
            BN_LOG("Idling");
            if((_action_r.graphics_indexes().front() != 1) || (_action_r.tiles_item() != bn::sprite_items::ram_movement.tiles_item()))
            {
                BN_LOG("Idled.");
                //idle
                _action_l = bn::create_sprite_animate_action_forever(
                    _sprite_l, 3, bn::sprite_items::ram_movement.tiles_item(), 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30);
                _action_r = bn::create_sprite_animate_action_forever(
                    _sprite_r, 3, bn::sprite_items::ram_movement.tiles_item(), 1, 3, 5, 7, 9, 11, 13, 15, 17, 19, 21, 23, 25, 27, 29, 31);
            }
        }

        _sprite_l.set_horizontal_flip(_flip);
        _sprite_r.set_horizontal_flip(_flip);

        _action_l.update();
        _action_r.update();
    }

    void Fighter::lockout(int frames)
    {
        _lockout = true;
        _lockout_duration = frames;
        _lockout_timer = _elapsed_frames;
    }

    void Fighter::flip()
    {
        _flip = !_flip;
        if(_flip)
        {
            _flip_offset = 32;
        }
        else
        {
            _flip_offset = -32;
        }
    }

    void Fighter::check_inputs()
    {
        // Input Vector
        if(bn::keypad::left_held())
        {
            _input_vector.set_x(-1);
        }
        else if(bn::keypad::right_held())
        {
            _input_vector.set_x(1);
        }
        else
        {
            _input_vector.set_x(0);
        }
        if(bn::keypad::up_held())
        {
            _input_vector.set_y(-1);
        }
        else if(bn::keypad::down_held())
        {
            _input_vector.set_y(1);
        }
        else
        {
            _input_vector.set_y(0);
        }

        // Flip inputs
        if(_flip)
        {
            _input_vector.set_x(-_input_vector.x());
        }

        // Ground Check
        if(_position.y() >= 25)
        {
            _grounded = true;
        }
        else
        {
            _grounded = false;
        }

        // Crouch
        if(bn::keypad::down_held() && !_crouching && _grounded)
        {
            Fighter::crouch();
        }

        // Check backdash input
        if((bn::keypad::left_pressed() && !_flip) || (bn::keypad::right_pressed() && _flip))
        {
            if((_dash_ready == 1 && !_flip) || (_dash_ready == 2 && _flip))
            {
                BN_LOG("BACKDASH!");
                Fighter::backdash();
            }
            else
            {
                if(!_flip)
                {
                    _dash_ready = 1;
                }
                else
                {
                    _dash_ready = 2;
                }
                _dash_timer = _elapsed_frames;
            }
        }

        // Finish back dash
        if(_velocity_multiplier == _backdash_speed && (_elapsed_frames - _dash_timer) > _backdash_duration)
        {
            _velocity_multiplier = 1;
        }

        // Check dash input
        if((bn::keypad::right_pressed() && !_flip) || (bn::keypad::left_pressed() && _flip))
        {
            if((_dash_ready == 2 && !_flip) || (_dash_ready == 1 && _flip))
            {
                BN_LOG("DASH!");
                Fighter::dash();
            }
            else
            {
                if(!_flip)
                {
                    _dash_ready = 2;
                }
                else
                {
                    _dash_ready = 1;
                }
                _dash_timer = _elapsed_frames;
            }
        }

        // Dash Window Expire
        if(_dash_ready > 0 && (_elapsed_frames - _dash_timer) > _dash_window)
        {
            _dash_ready = 0;
            _dash_timer = 0;
        }

        // Release dash
        if(_dashing && _grounded)
        {
            if(_lockout || (!bn::keypad::right_held() && !_flip) || (!bn::keypad::left_held() && _flip))
            {
                _velocity_multiplier = 1;
                _dashing = false;
            }
        }

        // Jump
        if(bn::keypad::up_pressed() && _grounded && !_lockout)
        {
            Fighter::jump();
        }

        // Attack 1
        if(bn::keypad::a_pressed() && !bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(1);
            Fighter::lockout(8);
        }

        // Attack 2
        if(bn::keypad::b_pressed() && !bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(2);
            Fighter::lockout(8);
        }

        // Attack 3
        if(bn::keypad::a_pressed() && bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(3);
            Fighter::lockout(24);
        }

        // Attack 4
        if(bn::keypad::b_pressed() && bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(4);
            Fighter::lockout(32);
        }

        _elapsed_frames++;
    }

    void Fighter::move()
    {
        // Attack interrupt
        int target_multiplier;
        if(_attacking)
        {
            target_multiplier = 0;
        }
        else
        {
            target_multiplier = 1;
        }
        // Accelerate
        if(!_crouching && _grounded && !_attacking)
        {
            if(!_flip)
            {
                _velocity += bn::fixed_point(_acceleration*_input_vector.x()*target_multiplier, 0);
            }
            else
            {
                _velocity -= bn::fixed_point(_acceleration*_input_vector.x()*target_multiplier, 0);
            }
        }

        // Drag
        if(_grounded || !_jumping)
        {
            BN_LOG("grounded");
            if(_attacking)
            {
                BN_LOG("Attack drag");
                _velocity.set_x(_velocity.x()*_attacking_drag);
            }
            else
            {
                BN_LOG("Drag");
                _velocity.set_x(_velocity.x()*_drag);
            }
        }

        // Limit Speed
        if(_velocity.x() > _walk_speed)
        {
            _velocity.set_x(_walk_speed);
        }
        else if(_velocity.x() < -_walk_speed)
        {
            _velocity.set_x(-_walk_speed);
        }

        // Apply Gravity
        if(_grounded)
        {
            _velocity.set_y(0);
        }
        else
        {
            _velocity += _gravity;
        }

        // Dash velocity
        _velocity.set_x(_velocity.x() * _velocity_multiplier);
        _input_vector.set_x(_input_vector.x() * _velocity_multiplier);

        // Move Player
        _position += _velocity;

        // Ground player
        if(_position.y() > 25)
        {
            _position.set_y(25);
        }

        // Position Rollover
        if(_position.x() > 160)
        {
            _position.set_x(-160);
        }
        else if(_position.x() < -160)
        {
            _position.set_x(160);
        }

        // Move Sprites
        if(_flip)
        {
            _sprite_l.set_position(_position+bn::fixed_point(_flip_offset,0));
            _sprite_r.set_position(_position-bn::fixed_point(_flip_offset,0));
        }
        else
        {
            _sprite_l.set_position(_position-bn::fixed_point(_flip_offset,0));
            _sprite_r.set_position(_position+bn::fixed_point(_flip_offset,0));
        }
    }

    void Fighter::jump()
    {
        _velocity.set_y(_jump_impulse);
        _grounded = false;
        _jumping = true;
    }

    void Fighter::dash()
    {
        _velocity_multiplier = _dash_speed;
        _dash_ready = 0;
        _dashing = true;
    }

    void Fighter::backdash()
    {
        _backdashing = true;
        _velocity_multiplier = _backdash_speed;
        _dash_ready = 0;
        _dash_timer = _elapsed_frames;
    }

    void Fighter::crouch()
    {
        _crouching = true;
    }

    void Fighter::attack(int attack_id)
    {
        _attacking = true;
        _action = attack_id;
    }

}