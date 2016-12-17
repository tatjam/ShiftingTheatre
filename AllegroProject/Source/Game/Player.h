#include "allegro5\allegro.h"
#include <iostream>
#include "PlatformManager.h"

#define draw_debug false

class Tilemap;

class Player
{
public:

	ALLEGRO_BITMAP* walljump_bm_f1;
	ALLEGRO_BITMAP* walljump_bm_f2;
	ALLEGRO_BITMAP* walljump_bm_f3;
	ALLEGRO_BITMAP* walljump_bm_f4;
	ALLEGRO_BITMAP* walljump_bm_f5;

	ALLEGRO_BITMAP* player_run_bm_f1;
	ALLEGRO_BITMAP* player_run_bm_f2;
	ALLEGRO_BITMAP* player_run_bm_f3;
	ALLEGRO_BITMAP* player_run_bm_f4;
	ALLEGRO_BITMAP* player_run_bm_f5;

	ALLEGRO_BITMAP* player_hang;

	float x = 2.0f;
	float y = 2.0f;

	float vx = 0.0f;
	float vy = 0.0f;

	float tile_size = 8.0f;

	float gainSpeed = 0.0f;

	bool on_air = false;

	int run_frame = 0;
	int run_cc_frame = 0;
	bool frames_going_down = false;

	int time_on_ground = 0;

	int health = 100;

	bool damage_last_frame = false;

	float jump_impulse = 1.0f;

	float jump_initial_impulse = 3.5f;

	bool jump_released = true;

	int wrframe = 0;
	int wframe = 0;

	//0 = NO; 1 = LEFT; 2 = RIGHT
	int walljumpH = 0;

	float walljumpX = 0.0f;
	float walljumpY = 0.0f;

	//TODO
	ALLEGRO_BITMAP* player_bit;


	ALLEGRO_SAMPLE* jump;
	ALLEGRO_SAMPLE* wjump;

	ALLEGRO_SAMPLE* hurt;



	//0 = NO; 1 = LEFT; 2 = RIGHT
	int hanging = 0;

	bool finished = false;

	bool died = false;

	bool on_platform = false;

	bool finished_f = false;

	bool received_damage_this_frame = false;

	void update(Tilemap t)
	{

		if (!finished)
		{
			ALLEGRO_COLOR c_tile = t.get_tile_raw((x + 8) / tile_size, (y + 8) / tile_size);

			unsigned char r, g, b;

			al_unmap_rgb(c_tile, &r, &g, &b);


			//If we are at exit tile
			if (r == 255 && g == 255 && b == 0)
			{
				finished = true;
				finished_f = true;
				//Reset variables as we have finished the level
				//Our x and y should be set in source.cpp
				vx = 0;
				vy = 0;
				health = 100;
				hanging = 0;
				time_on_ground = 0;
				on_air = false;

				died = false;

				//That's it, source.cpp will set finished back to false and load the new tilemap
				//all without us doing anything
			}

			//If we are at a lava tile
			if ((r == 200 && g == 0 && b == 0) || (r == 255 && g == 0 && b == 0) || health <= 0)
			{
				//We died :(
				finished = true;
				finished_f = true;
				//Reset variables as we have died
				//Our x and y should be set in source.cpp
				vx = 0;
				vy = 0;
				health = 100;
				hanging = 0;
				time_on_ground = 0;
				on_air = false;

				died = true;
			}

			if (vx <= -0.01f || vx >= 0.01f)
			{
				run_cc_frame++;
				if (run_cc_frame >= 5)
				{
					if (frames_going_down)
					{
						run_frame--;
						if (run_frame == 0)
						{
							frames_going_down = false;
						}
					}
					else
					{
						run_frame++;
						if (run_frame == 4)
						{
							frames_going_down = true;
						}
					}

					run_cc_frame = 0;

				}
			}
			else
			{
				run_frame = 0;
				run_cc_frame = 0;
				frames_going_down = false;
			}

			//I know i should not use the state, but i will do so for fast coding
			ALLEGRO_KEYBOARD_STATE state;
			al_get_keyboard_state(&state);

			on_air = !t.get_tile((int)(x + 8) / tile_size, (int)(y + 16 + 0.01f) / tile_size).collides;

			Platform* on_p = t.platform_manager.point_collides((x + 8) / tile_size, (y + 16.1f) / tile_size);
			if (on_p != NULL)
			{
				//We are on top of a platform, lock to it
				on_air = false;
				vx = on_p->vx * tile_size;
				vy = on_p->vy * tile_size;
				y = on_p->y * tile_size - 14;
				on_platform = true;
			}
			else
			{
				on_platform = false;
			}

			if (on_air)
			{
				time_on_ground = 0;
				//Simulate gravity
				vy += 0.3f;
				if (vy > 2.5f)
				{
					vy = 2.5f;
				}
			}
			else
			{
				if (on_platform)
				{
					vy = on_p->vy * tile_size;
					time_on_ground += 1;
				}
				else
				{
					vy = 0;
					time_on_ground += 1;
				}
			}

			if (al_key_down(&state, ALLEGRO_KEY_LEFT) || al_key_down(&state, ALLEGRO_KEY_A))
			{
				//Move left
				if (on_air)
				{
					if (vx > -1.0f)
					{
						vx -= 0.15f;
					}
				}
				else
				{
					if (vx > -1.0f)
					{
						vx -= 0.25f;
					}
				}

			}
			else if (al_key_down(&state, ALLEGRO_KEY_RIGHT) || al_key_down(&state, ALLEGRO_KEY_D))
			{
				//Move right
				if (on_air)
				{
					if (vx < 1.0f)
					{
						vx += 0.15f;
					}
				}
				else
				{
					if (vx < 1.0f)
					{
						vx += 0.25f;
					}
				}

			}
			else
			{
				if (!on_platform)
				{
					if (on_air)
					{
						if (vx > 0.05f)
						{
							vx -= 0.05f;
						}
						else if (vx < -0.05f)
						{
							vx += 0.05f;
						}
						else
						{
							vx = 0.0f;
						}
					}
					else
					{
						vx = 0;
					}
				}
				else
				{
					vx = on_p->vx * tile_size;
				}
			}

			//0 = NO; 1 = LEFT; 2 = RIGHT
			int walljump = 0;


			if (al_key_down(&state, ALLEGRO_KEY_Y));
			{
				//damage(100);
			}

			//Test sideways touch for walljump
			if (on_air)
			{
				if (t.get_tile((int)(x + 16.1f) / tile_size, (int)(y + 8) / tile_size).collides)
				{
					//Walljump to the left
					walljump = 1;
					vy /= 1.5f;
					vy -= 0.1f;
					hanging = 2;
				}
				else if (t.get_tile((int)(x - 0.1f) / tile_size, (int)(y + 8) / tile_size).collides)
				{
					//Walljump to the right
					walljump = 2;
					vy /= 1.5f;
					vy -= 0.1f;
					hanging = 1;
				}
				else
				{
					walljump = 0;
					hanging = 0;
				}
			}
			else
			{
				hanging = 0;
			}

			//printf("%i \n", walljump);

			if (al_key_down(&state, ALLEGRO_KEY_UP) || al_key_down(&state, ALLEGRO_KEY_W))
			{
				//Jump
				
				if (!on_air && time_on_ground >= 5 && jump_released)
				{
					if (on_platform)
					{
						y -= 2.0f;
					}

					vy = -jump_initial_impulse;
					jump_impulse = 3.4f;
					jump_released = false;

					al_play_sample(jump, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);

				}
				else
				{
					if (jump_impulse > 0.0f && !jump_released)
					{
						vy -= 0.17;
						jump_impulse -= 0.07;
					}

					if (jump_released)
					{
						if (walljump != 0)
						{
							al_play_sample(wjump, 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
						}

						if (walljump == 1)
						{
							vx = -2.0f;
							vy = -3.0f;
							walljumpH = 1;
							wframe = 0;
							walljumpX = x;
							walljumpY = y;
						}
						else if (walljump == 2)
						{
							vx = 2.0f;
							vy = -3.0f;
							walljumpH = 2;
							wframe = 0;
							walljumpX = x;
							walljumpY = y;
						}
					}

				}
			}
			else
			{
				jump_released = true;
			}

			wframe = wrframe / 4;

			if (walljumpH != 0)
			{
				if (wframe == 0)
				{
					wframe = 4;
				}
				wrframe++;
			}

			if (wrframe == 5)
			{
				wrframe = 0;
				walljumpH = 0;
			}

			if (al_key_down(&state, ALLEGRO_KEY_DOWN) || al_key_down(&state, ALLEGRO_KEY_S))
			{
				//Crouch
				//y += 0.3f;
			}


			//Check if we can do the move and end not stuck: TODO

			if (vy < 0)
			{
				//Check if our head collides next frame
				if (t.get_tile((int)(x + 8) / tile_size, (int)(y - 0.01f + vy / 2) / tile_size).collides)
				{
					vy = 0;
				}
			}

			if (vx > 0)
			{
				if (t.get_tile((int)(x + 14) / tile_size, (int)(y + 8) / tile_size).collides)
				{
					vx = 0;
				}
			}

			if (vx < 0)
			{
				if (t.get_tile((int)(x + 1) / tile_size, (int)(y + 8) / tile_size).collides)
				{
					vx = 0;
				}
			}


			//Test collisions with our sub 8x8 sprites and move out of the obstacle
			{
				if (t.get_tile((int)(x + 4) / tile_size, (int)(y + 4) / tile_size).collides)
				{
					//We are stuck in the upper left sprite
					x += 0.2f;
					y += 0.2f;
				}

				if (t.get_tile((int)(x + 12) / tile_size, (int)(y + 4) / tile_size).collides)
				{
					//We are stuck in the upper right sprite
					x -= 0.2f;
					y += 0.2f;
				}

				if (t.get_tile((int)(x + 4) / tile_size, (int)(y + 12) / tile_size).collides)
				{
					//We are stuck in the lower left sprite
					x += 0.2f;
					y -= 0.2f;
				}

				if (t.get_tile((int)(x + 12) / tile_size, (int)(y + 12) / tile_size).collides)
				{
					//We are stuck in the lower right sprite
					x -= 0.2f;
					y -= 0.2f;
				}

				//Also one extra a little bit over foot height
				if (t.get_tile((int)(x + 8) / tile_size, (int)(y + 15) / tile_size).collides)
				{
					//We are stuck in the feet
					y -= 0.5f;
				}
				else
				{
					//y = roundf(y);
				}
			}


			y += vy;
			x += vx;

			if (received_damage_this_frame)
			{
				received_damage_this_frame = false;
				damage_last_frame = true;
			}
			else
			{
				damage_last_frame = false;
			}
		}

	}

	void damage(int dmg)
	{
		if (damage_last_frame)
		{

		}
		else
		{
			health -= dmg;
			received_damage_this_frame = true;
			al_play_sample(hurt, 2.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
	}

	void add_speed(float x_s, float y_s, bool force_air = true)
	{
		vx += x_s;
		vy += y_s;
		if (force_air)
		{
			on_air = true;
			y -= 0.5f;
		}
	}

	void kill()
	{

	}

	void render(float scale, ALLEGRO_FONT* font, float off_x, float off_y)
	{

		ALLEGRO_BITMAP* p_to_draw = NULL;

		if (hanging == 0)
		{
			if (vx < -0.01f || vx > 0.01f && !on_air)
			{
				if (run_frame == 0) { p_to_draw = player_run_bm_f1; };
				if (run_frame == 1) { p_to_draw = player_run_bm_f2; };
				if (run_frame == 2) { p_to_draw = player_run_bm_f3; };
				if (run_frame == 3) { p_to_draw = player_run_bm_f4; };
				if (run_frame == 4) { p_to_draw = player_run_bm_f5; };

			}
			else
			{
				p_to_draw = player_bit;
			}
		}
		else
		{
			p_to_draw = player_hang;
		}
		

		if (vx > 0.3 || hanging == 1)
		{
			al_draw_scaled_bitmap(p_to_draw, 0, 0, 16, 16,
				(x - off_x) * scale, (y - off_y) * scale, 16 * scale, 16 * scale, 0);
		}
		else if (vx < -0.3 || hanging == 2)
		{
			al_draw_scaled_bitmap(p_to_draw, 0, 0, 16, 16,
				(x - off_x) * scale, (y - off_y) * scale, 16 * scale, 16 * scale, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{
			al_draw_scaled_bitmap(p_to_draw, 0, 0, 16, 16,
				(x - off_x) * scale, (y - off_y) * scale, 16 * scale, 16 * scale, 0);
		}



		ALLEGRO_BITMAP* w_to_draw = NULL;

		if (wframe == 1) { w_to_draw = walljump_bm_f1; }
		if (wframe == 2) { w_to_draw = walljump_bm_f2; }
		if (wframe == 3) { w_to_draw = walljump_bm_f3; }
		if (wframe == 4) { w_to_draw = walljump_bm_f4; }
		if (wframe == 5) { w_to_draw = walljump_bm_f5; }

		//printf("WFRAME: %f \n", wframe);

		if (wframe > 0 && wframe < 5)
		{
			if (walljumpH == 1)
			{
				al_draw_scaled_bitmap(w_to_draw, 0, 0, 8, 8,
					(walljumpX - off_x) * scale, (walljumpY - off_y) * scale, 8 * scale, 8 * scale, ALLEGRO_FLIP_HORIZONTAL);
			}
			else
			{
				al_draw_scaled_bitmap(w_to_draw, 0, 0, 8, 8,
					(walljumpX - off_x) * scale, (walljumpY - off_y) * scale, 8 * scale, 8 * scale, 0);
			}
		}

		if (draw_debug)
		{

			if (on_air)
			{
				al_draw_circle((x + 8 - off_x) * scale, (y + 16 - off_y) * scale, 1.0f, al_map_rgb(255, 0, 0), 2.0f);
			}
			else
			{
				al_draw_circle((x + 8 - off_x) * scale, (y + 16 - off_y) * scale, 2.0f, al_map_rgb(255, 128, 0), 2.0f);
			}

			al_draw_rectangle((x - off_x) * scale, (y - off_y) * scale,
				((x + tile_size * 2) - off_x) * scale, ((y + tile_size * 2) - off_y) * scale,
				al_map_rgb(255, 0, 0), 1.0f);


			al_draw_circle((int)(x + 16.1f - off_x) * scale, (int)(y + 8 - off_y) * scale, 1.0f, al_map_rgb(255, 0, 0), 1.0f);
			al_draw_circle((int)(x - 0.1f - off_x) * scale, (int)(y + 8 - off_y) * scale, 1.0f, al_map_rgb(255, 0, 0), 1.0f);

			al_draw_textf(font, al_map_rgb(255, 255, 255), 0.0f, 0.0f, 0, "Player pos: X: %f, Y: %f", x, y);

			al_draw_textf(font, al_map_rgb(255, 255, 255), 0.0f, 15.0f, 0, "Tile pos in tile: X: %f, Y: %f",
				(int)(x + 8) / tile_size, (int)(y + 16 + 0.001f) / tile_size);

			al_draw_textf(font, al_map_rgb(255, 255, 255), 0.0f, 30.0f, 0, "Player real pos: X: %f, Y: %f",
				((x * scale) - (off_x * scale)), ((y * scale) - (off_y * scale)));
		}
	}
};