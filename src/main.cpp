#include "bn_math.h"
#include "bn_core.h"
#include "bn_keypad.h"
#include "bn_regular_bg_builder.h"
#include "bn_sprite_builder.h"
#include "bn_regular_bg_animate_actions.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_text_generator.h"
#include "bn_random.h"
#include "bn_forward_list.h"
#include "bn_pool.h"
#include "bn_log.h"
#include "bn_rect.h"
#include "bn_sound_items.h"
#include "bn_sprite_animate_actions.h"
#include "bn_sprite_builder.h"
#include "bn_timer.h"

#include "bn_regular_bg_items_bg_anim.h"
#include "bn_regular_bg_items_crow.h"
#include "bn_regular_bg_items_strive.h"
#include "bn_regular_bg_items_simple_bg.h"
#include "bn_sprite_items_logo.h"
#include "bn_sprite_items_ship.h"
#include "bn_sprite_items_asteroid.h"
#include "bn_sprite_items_bullet.h"

#include "main.h"
#include "fighter.h"
#include "character.h"
#include "char_ramlethal.h"

#include "bn_sprite_items_ramidle.h"
#include "bn_sprite_items_ram_movement.h"


struct Bullet {
    bn::fixed_point position;
    bn::fixed_point velocity;
    int lifetime;
    bn::optional<bn::sprite_ptr> sprite;
    bn::rect coll;
    bool is_dead;
};

struct Asteroid {
    bn::fixed_point position;
    bn::fixed_point velocity;
    bn::fixed scale;
    bn::fixed rotation;
    bn::optional<bn::sprite_ptr> sprite;
    bn::rect coll;
    bool is_dead;
};

namespace db
{
    [[nodiscard]] constexpr bn::fixed_point normalize(bn::fixed_point vector)
    {
        bn::fixed magnitude = bn::sqrt((vector.x() * vector.x()) + (vector.y() * vector.y()));
        if(magnitude == 0)
        {
            return bn::fixed_point(0,0);
        }
        return bn::fixed_point(vector.x(), vector.y()) / magnitude;
    }
}

namespace games
{

    void title_scene()
    {
        bn::regular_bg_ptr anim_bg = bn::regular_bg_items::bg_anim.create_bg(0, 0);
        bn::regular_bg_cached_animate_action<11> action_bg = bn::create_regular_bg_cached_animate_action_forever(
            anim_bg, 6, bn::regular_bg_items::bg_anim.map_item(), 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10);

        bn::sprite_ptr logo_sprite_l = bn::sprite_items::logo.create_sprite(-32, -42);
        bn::sprite_animate_action<2> action_l = bn::create_sprite_animate_action_forever(
            logo_sprite_l, 32, bn::sprite_items::logo.tiles_item(), 0, 2);

        bn::sprite_ptr logo_sprite_r = bn::sprite_items::logo.create_sprite(32, -42);
        bn::sprite_animate_action<2> action_r = bn::create_sprite_animate_action_forever(
            logo_sprite_r, 32, bn::sprite_items::logo.tiles_item(), 1, 3);

        while(! bn::keypad::start_pressed())
        {
            action_l.update();
            action_r.update();
            action_bg.update();

            bn::core::update();
        }
    }

    void title_scene2()
    {
        bn::regular_bg_ptr strive_bg = bn::regular_bg_items::strive.create_bg(0, 0);

        while(! bn::keypad::start_pressed())
        {
            bn::core::update();
        }
    }

    void title_scene3()
    {
        bn::regular_bg_ptr crow_bg = bn::regular_bg_items::crow.create_bg(0, 0);

        while(! bn::keypad::start_pressed())
        {
            bn::core::update();
        }
    }


    ///
    /// \brief FIGHTER
    ///
    void fighter_scene()
    {
        // Variables
        db::Fighter fighter = db::Fighter(ramlethal::character, bn::fixed_point(-32, 25));

        // db::Fighter enemy = db::Fighter(ramlethal::character, bn::fixed_point(32, 25));
        // enemy.flip();

        // bn::regular_bg_ptr simple_bg = bn::regular_bg_items::simple_bg.create_bg(0, 0);

        fighter.animate();

        while(! bn::keypad::start_pressed())
        {
            // Update enemy
            // enemy.update();

            // Update Player
            fighter.update();

            bn::core::update();
        }
    }

    void asteroid_scene()
    {
        // Bullet Pool
        constexpr int BULLET_POOL_SIZE = 32;

        bn::pool<Bullet, BULLET_POOL_SIZE> bullet_pool;
        bn::forward_list<Bullet*, BULLET_POOL_SIZE> bullet_list;

        // Asteroid Pool
        constexpr int ASTEROID_POOL_SIZE = 32;

        bn::pool<Asteroid, ASTEROID_POOL_SIZE> asteroid_pool;
        bn::forward_list<Asteroid*, ASTEROID_POOL_SIZE> asteroid_list;

        // Config Variables
        const bn::fixed ship_scale = 0.4f;

        const int turn_speed = 4;

        const bn::fixed acceleration = 0.03;

        const bn::fixed drag = 0.99;

        const bn::fixed max_speed = 3;

        const bn::fixed bullet_speed = 3;

        const bn::fixed asteroid_speed = 0.03;

        const bn::fixed recoil_multiplier = 0.5;

        // Sprites
        bn::sprite_ptr ship_sprite = bn::sprite_items::ship.create_sprite(0, 0);
        ship_sprite.set_scale(ship_scale);

        bn::sprite_ptr ship_sprite_clone_x = bn::sprite_items::ship.create_sprite(0, 0);
        ship_sprite_clone_x.set_scale(ship_scale);

        bn::sprite_ptr ship_sprite_clone_y = bn::sprite_items::ship.create_sprite(0, 0);
        ship_sprite_clone_y.set_scale(ship_scale);

        bn::optional<bn::sprite_ptr> asteroid_sprite;

        // Variables
        bn::fixed_point ship_pos;
        bn::fixed x_velocity = 0;
        bn::fixed y_velocity = 0;

        bn::fixed_point aim_dir;

        bn::fixed_point asteroid_pos;
        bn::point asteroid_angle;

        bn::random random;
        bn::random random2;

        int asteroid_timer = 0;

        // Start Loop
        while(!bn::keypad::start_pressed())
        {
            // Ship Rotation
            if(bn::keypad::l_held())
            {
                ship_sprite.set_rotation_angle_safe(ship_sprite.rotation_angle()+turn_speed);
            }
            else if(bn::keypad::r_held())
            {
                ship_sprite.set_rotation_angle_safe(ship_sprite.rotation_angle()-turn_speed);
            }

            // Ship Movement Input
            if(bn::keypad::left_held())
            {
                x_velocity -= acceleration;
            }
            else if(bn::keypad::right_held())
            {
                x_velocity += acceleration;
            }
            else
            {
                x_velocity *= drag;
            }
            if(bn::keypad::up_held())
            {
                y_velocity -= acceleration;
            }
            else if(bn::keypad::down_held())
            {
                y_velocity += acceleration;
            }
            else
            {
                y_velocity *= drag;
            }

            // Limit Speed
            if(x_velocity > max_speed)
            {
                x_velocity = max_speed;
            }
            else if(x_velocity < -max_speed)
            {
                x_velocity = -max_speed;
            }

            if(y_velocity > max_speed)
            {
                y_velocity = max_speed;
            }
            else if(y_velocity < -max_speed)
            {
                y_velocity = -max_speed;
            }

            // Ship Mover
            ship_pos.set_x(ship_pos.x()+x_velocity);
            ship_pos.set_y(ship_pos.y()+y_velocity);

            // Position Rollover
            if(bn::abs(ship_pos.x()) > 120)
            {
                ship_pos.set_x(-ship_pos.x());
            }
            if(bn::abs(ship_pos.y()) > 80)
            {
                ship_pos.set_y(-ship_pos.y());
            }

            ship_sprite.set_position(ship_pos);

            // Clone Movement X
            if(ship_sprite.x() < 0)
            {
                ship_sprite_clone_x.set_x(ship_sprite.x()+240);
            }
            else
            {
                ship_sprite_clone_x.set_x(ship_sprite.x()-240);
            }
            ship_sprite_clone_x.set_y(ship_sprite.y());
            ship_sprite_clone_x.set_rotation_angle(ship_sprite.rotation_angle());

            // Clone Movement Y
            if(ship_sprite.y() < 0)
            {
                ship_sprite_clone_y.set_y(ship_sprite.y()+160);
            }
            else
            {
                ship_sprite_clone_y.set_y(ship_sprite.y()-160);
            }
            ship_sprite_clone_y.set_x(ship_sprite.x());
            ship_sprite_clone_y.set_rotation_angle(ship_sprite.rotation_angle());


            // Fire Bullet
            if(bn::keypad::a_pressed())
            {
                // Get ship heading vector
                aim_dir.set_x(bn::degrees_lut_sin(ship_sprite.rotation_angle()));
                aim_dir.set_y(bn::degrees_lut_cos(ship_sprite.rotation_angle()));

                // Spawn Bullet
                auto bullet = bn::sprite_items::bullet.create_sprite_optional(ship_pos.x(), ship_pos.y());
                bullet->set_z_order(1);
                bullet_list.push_front(&bullet_pool.create(ship_sprite.position(), aim_dir*bullet_speed, 240, bullet, bn::rect(bullet->position().x().floor_integer(),bullet->position().y().floor_integer(),2,2)));

                // Recoil
                x_velocity += aim_dir.x() * recoil_multiplier;
                y_velocity += aim_dir.y() * recoil_multiplier;

                // Shoot Sound
                bn::sound_items::kill.play(0.5);
            }

            // Update Bullets
            for (auto before_iter = bullet_list.before_begin(),
                 iter = bullet_list.begin();
                 iter != bullet_list.end();) {
                Bullet* bullet = *iter;

                // Destroy expired bullets
                if (bullet->lifetime <= 0 && !bullet->is_dead) {
                    bullet->is_dead = true;
                }
                // Update Bullets
                else {
                    --bullet->lifetime;

                    // Move Bullets
                    bullet->position = bullet->position - bullet->velocity;
                    bullet->sprite->set_position(bullet->position);
                    bullet->coll.set_x(bullet->position.x().floor_integer());
                    bullet->coll.set_y(bullet->position.y().floor_integer());
                }
                before_iter = iter;
                ++iter;
            }

            // Bullet Destruction
            bullet_list.remove_if([&bullet_pool](Bullet* bullet) -> bool {
                if (bullet->is_dead) {
                    bullet_pool.destroy(*bullet);
                    return true;
                }
                return false;
            });

            // Spawn Asteroids
            if(asteroid_timer == 0)
            {
                // Get Spawn Position
                if(random.get_bool())
                {
                    if(random2.get_bool())
                    {
                        asteroid_pos.set_x(120);
                    }
                    else
                    {
                        asteroid_pos.set_x(-120);
                    }
                    asteroid_pos.set_y(random.get_int(160)-80);
                }
                else
                {
                    if(random2.get_bool())
                    {
                        asteroid_pos.set_y(80);
                    }
                    else
                    {
                        asteroid_pos.set_y(-80);
                    }
                    asteroid_pos.set_x(random.get_int(240)-120);
                }

                // Spawn Asteroid
                auto asteroid_sprite = bn::sprite_items::asteroid.create_sprite_optional(asteroid_pos.x(), asteroid_pos.y());
                bn::fixed scale_modifier = random.get_fixed(0.75) + bn::fixed(0.5);
                asteroid_sprite->set_rotation_angle(random.get_int(360));
                asteroid_sprite->set_scale(0.5*scale_modifier);
                asteroid_sprite->set_z_order(-1);
                asteroid_list.push_front(&asteroid_pool.create(asteroid_pos, asteroid_pos, 1, random.get_int(8)-4, asteroid_sprite, bn::rect(asteroid_pos.x().floor_integer(), asteroid_pos.y().floor_integer(), (16*scale_modifier).floor_integer(),(16*scale_modifier).floor_integer())));
            }

            // Update Asteroids
            for (auto before_iter = asteroid_list.before_begin(),
                 iter = asteroid_list.begin();
                 iter != asteroid_list.end();) {
                Asteroid* asteroid = *iter;

                // Destroy OOB Asteroids
                if (abs(asteroid->position.x()) > 160) {
                    asteroid->is_dead = true;
                }
                else if(abs(asteroid->position.y()) > 120) {
                    asteroid->is_dead = true;
                }

                else {
                    // Move Asteroids
                    asteroid->position = asteroid->position - asteroid->velocity*asteroid_speed;
                    asteroid->sprite->set_position(asteroid->position);
                    asteroid->coll.set_x(asteroid->position.x().floor_integer());
                    asteroid->coll.set_y(asteroid->position.y().floor_integer());

                    // Rotate Asteroids
                    asteroid->sprite->set_rotation_angle_safe(asteroid->sprite->rotation_angle()+asteroid->rotation);

                    // Collision Check
                    for (auto bullet_before_iter = bullet_list.before_begin(),
                         bullet_iter = bullet_list.begin();
                         bullet_iter != bullet_list.end();) {
                        Bullet* bullet = *bullet_iter;

                        if(bullet->coll.intersects(asteroid->coll) && !bullet->is_dead)
                        {
                            // Play Explosion Sound
                            bn::sound_items::explode.play(0.5);

                            // Kill Asteroid & Bullet
                            bullet->is_dead = true;
                            asteroid->is_dead = true;
                        }
                        else
                        {
                            bullet_before_iter = bullet_iter;
                            if(bullet_list.size() > 0)
                            {
                                ++bullet_iter;
                            }
                        }
                    }
                }

                before_iter = iter;
                if(asteroid_list.size() > 0)
                {
                    ++iter;
                }
            }

            // Destroy Asteroids
            asteroid_list.remove_if([&asteroid_pool](Asteroid* asteroid) -> bool {
                if (asteroid->is_dead) {
                    asteroid_pool.destroy(*asteroid);
                    return true;
                }
                return false;
            });

            // Asteroid Timer
            asteroid_timer += 1;

            if(asteroid_timer > 80)
            {
                asteroid_timer = 0;
            }

            bn::core::update();
        }

        /// EXIT SCENE

        // Clear all pools before exiting
        bullet_list.remove_if([&bullet_pool](Bullet* bullet) -> bool {
            bullet_pool.destroy(*bullet);
            return true;
        });
        asteroid_list.remove_if([&asteroid_pool](Asteroid* asteroid) -> bool {
            asteroid_pool.destroy(*asteroid);
            return true;
        });
        bullet_list.clear();
        asteroid_list.clear();
    }
}

int main()
{
    bn::core::init();

    while(true)
    {
        games::title_scene();
        bn::core::update();

        games::fighter_scene();
        bn::core::update();

        // games::title_scene2();
        // bn::core::update();

        // games::title_scene3();
        // bn::core::update();

        games::asteroid_scene();
        bn::core::update();
    }
}
