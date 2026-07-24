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
#include "animation.h"

#include "bn_sprite_items_ram_movement.h"
// #include "bn_affine_bg_items_hitbox.h"
// #include "bn_affine_bg_items_hurtbox.h"

namespace db
{
    Fighter::Fighter(const CharacterData& character, bn::fixed_point spawn_position) :
    _position(spawn_position),
    _sprite_l(character.movement_sprite->create_sprite(spawn_position+bn::point(32,0))),
    _sprite_r(character.movement_sprite->create_sprite(spawn_position+bn::point(-32,0))),
    _character(character)
    {
        _sprite_l.put_above();
        _sprite_r.put_above();

        _animation_l = animation::play(
            _sprite_l,
            _character.motionset[0].animation_l);
        _animation_r = animation::play(
            _sprite_r,
            _character.motionset[0].animation_r);

        set_motion(Motion::Idle);

        // // DEBUG
        // _hurtbox_debug.put_above();
        // BN_LOG("height: ", _hurtbox.height/32, "width: ", _hurtbox.width/32);
        // _hurtbox_debug.set_scale((bn::fixed(_hurtbox.width)/256), (bn::fixed(_hurtbox.height)/256));
        // _hurtbox_debug.set_blending_enabled(true);
        // _hurtbox_debug.set_priority(2);
        // _hurtbox_debug.set_wrapping_enabled(false);
        // _hurtbox_debug.set_z_order(-2);
        // _hitbox_debug.set_wrapping_enabled(false);
        // _hitbox_debug.set_blending_enabled(true);
        // _hitbox_debug.set_priority(3);
        // _hitbox_debug.set_z_order(-4);
        // _hitbox_debug.put_above();
        // _hitbox_debug.set_visible(false);
        // bn::blending::set_transparency_alpha(0.5);
    }

    void Fighter::state_machine()
    {
        switch(_motion)
        {
        case Motion::Idle:

            break;

        case Motion::WalkForwardStartup:

            if(!animation::playing(_animation_l))
                set_motion(Motion::WalkForward);

            break;

        case Motion::WalkBackwardStartup:

            if(!animation::playing(_animation_l))
                set_motion(Motion::WalkBackward);

            break;

        case Motion::DashStartup:

            if(!animation::playing(_animation_l))
                set_motion(Motion::DashLoop);

            break;

        case Motion::Backdash:
            if(!animation::playing(_animation_l))
                _backdashing = false;

        case Motion::JumpStartup:

            if(!animation::playing(_animation_l))
                set_motion(Motion::JumpLoop);

            break;

        case Motion::JumpLoop:

            if(_grounded)
                set_motion(Motion::JumpEnd);

            break;

        case Motion::JumpEnd:

            if(!animation::playing(_animation_l))
                _jumping = false;

            break;
        case Motion::CrouchStartup:

            if(!animation::playing(_animation_l))
                set_motion(Motion::CrouchLoop);

            break;

        case Motion::CrouchLoop:
            if(!bn::keypad::down_held())
                set_motion(Motion::CrouchEnd);

            break;

        case Motion::CrouchEnd:

            if(!animation::playing(_animation_l))
            {
                _crouching = false;
                BN_LOG("Stop Crouching");
            }

            break;
        }
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
            if(!animation::playing(_animation_l))
            {
                BN_LOG("ATTACK FINISHED");
                _attacking = false;
                set_motion(Motion::Idle);
            }
        }
        else if(_jumping)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Jumping");
            //Jump
            if(!_grounded && _motion != Motion::JumpStartup && _motion != Motion::JumpLoop)
            {
                set_motion(Motion::JumpStartup);
            }
        }
        else if(_dashing)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Dashing");
            if(_motion != Motion::DashStartup && _motion != Motion::DashLoop)
            {
                set_motion(Motion::DashStartup);
            }
        }
        else if(_backdashing)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Backdashing");
            if(_motion != Motion::Backdash)
            {
                set_motion(Motion::Backdash);
            }
        }
        else if(_crouching)
        {
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            BN_LOG("Crouching");
            // Crouch Startup
            if(_motion != Motion::CrouchStartup && _motion != Motion::CrouchLoop && _motion != Motion::CrouchEnd)
            {
                set_motion(Motion::CrouchStartup);
            }
        }
        else if(_walking_r){
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery

            // Walk Startup
            if(_motion != Motion::WalkForwardStartup && _motion != Motion::WalkForward)
            {
                set_motion(Motion::WalkForwardStartup);
            }
        }
        else if(_walking_l){
            if(_attacking && !_lockout) { _attacking = false; } // Cancel attack recovery
            // Walk Startup
            if(_motion != Motion::WalkBackwardStartup && _motion != Motion::WalkBackward)
            {
                set_motion(Motion::WalkBackwardStartup);
            }
        }
        else if(!_attacking) {
            BN_LOG("Idling");
            if(_motion != Motion::Idle)
            {
                BN_LOG("Idled.");
                //idle
                set_motion(Motion::Idle);
            }
        }

        animation::update(_animation_l);
        animation::update(_animation_r);
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
            if((_dash_ready == 1 && _grounded && !_flip) || (_dash_ready == 2 && _grounded && _flip))
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
            _backdashing = false;
        }

        // Check dash input
        if((bn::keypad::right_pressed() && !_flip) || (bn::keypad::left_pressed() && _flip))
        {
            if((_dash_ready == 2 && _grounded && !_flip) || (_dash_ready == 1 && _grounded && _flip))
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
            Fighter::attack(Motion::Punch);
        }

        // Attack 2
        if(bn::keypad::b_pressed() && !bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(Motion::Kick);
        }

        // Attack 3
        if(bn::keypad::a_pressed() && bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(Motion::Slash);
        }

        // Attack 4
        if(bn::keypad::b_pressed() && bn::keypad::r_held() && !_lockout)
        {
            Fighter::attack(Motion::Heavy);
        }

        _elapsed_frames++;
    }

    void Fighter::set_motion(Motion motion)
    {
        if(motion == _motion)
            return;

        _motion = motion;
        _motion_frame = 0;

        const MoveData& data =
            _character.motionset[int(motion)];

        _animation_l = animation::play(_sprite_l, data.animation_l);
        _animation_r = animation::play(_sprite_r, data.animation_r);
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
            if(_attacking)
            {
                _velocity.set_x(_velocity.x()*_attacking_drag);
            }
            else
            {
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
        set_motion(Motion::JumpStartup);
    }

    void Fighter::dash()
    {
        _velocity_multiplier = _dash_speed;
        _dash_ready = 0;
        _dashing = true;
        set_motion(Motion::DashStartup);
    }

    void Fighter::backdash()
    {
        _backdashing = true;
        _velocity_multiplier = _backdash_speed;
        _dash_ready = 0;
        _dash_timer = _elapsed_frames;
        set_motion(Motion::Backdash);
    }

    void Fighter::crouch()
    {
        _crouching = true;
        set_motion(Motion::CrouchStartup);
    }

    void Fighter::attack(Motion attackID)
    {
        _attacking = true;
        set_motion(attackID);
        Fighter::lockout(_character.motionset[int(attackID)].recovery);
        // _action = attackID;
    }

}