#pragma once
#include "allegro5/allegro.h"
#include "Tilemap.h"

#include <vector>
#include <string>
#include <iostream>
#include <sstream>

#define draw_debug false

class Player;

//Basic class for enemies, players dont know about enemies on their code,
//enemies must handle the player jumping on them, etc...
//This does nothing at all
class Enemy
{
public:

	//You should spawn at x and y
	virtual void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites, 
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		printf("Oh no slicing!\n");
	}
	
	//Called every frame
	virtual void update(Player* player, Tilemap* map)
	{
		printf("Oh no slicing!\n");
	}

	//Draw your sprite to screen here
	virtual void draw(float off_x, float off_y, float scale)
	{
		printf("Oh no slicing!\n");
	}
};


//BasicEnemy defines an entity which moves left and right.
//If you hit it you lose health, if it dies it does nothing :P
//tile_size * 2 in size
class BasicEnemy : public Enemy
{
public:

	float x = 0.0f;
	float y = 0.0f;

	float vx = 0.0f;
	float vy = 0.0f;

	bool alive = true;

	int tile_size = 0;

	std::vector<ALLEGRO_BITMAP*>* sprites;
	std::vector<ALLEGRO_SAMPLE*>* samples;

	bool moving_left = true;

	bool spiky = false;

	bool on_air = false;

	int frame = 0;

	int t_frm = 0;

	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites,
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		this->x = x * tile_size - tile_size;
		this->y = y * tile_size - tile_size;
		this->tile_size = tile_size;

		this->sprites = sprites;
		this->samples = samples;

		if (args == "spiky")
		{
			spiky = true;
		}
		else
		{
			spiky = false;
		}
	}

	void update(Player* player, Tilemap* map)
	{
		if (alive)
		{
			t_frm++;


			Rect player_rect = Rect(player->x, player->y + 3.0f, tile_size * 2, tile_size * 2);
			Rect my_rect = Rect(x, y, tile_size * 2, tile_size * 2);

			if (player_rect.contains(Vector2(x + 8, y + 0.3f)))
			{
				player->add_speed(0.0f, -5.0f);
				al_play_sample(samples->at(0), 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
				alive = false;
			}

			if (player_rect.contains(Vector2(x, y + 8)))
			{
				player->add_speed(-2.8f, -4.0f);
				player->damage(10);
			}

			if (player_rect.contains(Vector2(x + 16, y + 8)))
			{
				player->add_speed(2.8f, -4.0f);
				player->damage(10);
			}

			if (t_frm >= 20)
			{
				frame++;
				if (frame == 2)
				{
					frame = 0;
				}
				t_frm = 0;
			}

			//Check if we have a block underneath and assign it to on_air
			on_air = !map->get_tile((x + 8.0f) / tile_size, (y + 16.5f) / tile_size).collides;

			if (!on_air)
			{
				vy = 0.0f;

				if (moving_left)
				{
					//Check collisions on our left with tilemap
					if (map->get_tile((x - 0.1f) / tile_size, (y + 8) / tile_size).collides)
					{
						//We are in front of a wall, rotate
						moving_left = false;
					}
					else
					{
						vx = -0.5f;
					}
				}
				else
				{
					//Check collisions on our right with tilemap
					if (map->get_tile((x + 16.1f) / tile_size, (y + 8) / tile_size).collides)
					{
						//We are in front of a wall, rotate
						moving_left = true;
					}
					else
					{
						vx = 0.5f;
					}
				}
			}
			else
			{
				if (vy <= 2.5f)
				{
					vy += 0.3f;
				}
				else
				{
					vy = 2.5f;
				}
			}


			x += vx;
			y += vy;
		}
		else
		{

		}
	}

	void draw(float off_x, float off_y, float scale)
	{
		if (alive)
		{
			if (draw_debug)
			{
				al_draw_rectangle((x - off_x) * scale, (y - off_y) * scale,
					((x + tile_size * 2) - off_x) * scale, ((y + tile_size * 2) - off_y) * scale,
					al_map_rgb(255, 0, 0), 1.0f);

				al_draw_circle((x + 8 - off_x) * scale, (y + 0.3f - off_y) * scale, 2.0f, al_map_rgb(0, 255, 0), 1.0f);

				al_draw_circle((x - off_x) * scale, (y + 8.0f) * scale, 2.0f, al_map_rgb(255, 0, 0), 1.0f);
				al_draw_circle((x + 16 - off_x) * scale, (y + 8.0f) * scale, 2.0f, al_map_rgb(255, 0, 0), 1.0f);
			}

			//printf("Drawing at %f, %f \n", x * scale - off_x, y * scale - off_y);
			if (moving_left)
			{
				al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 2, tile_size * 2,
					(x - off_x) * scale, (y * scale) - off_y * scale, tile_size * 2 * scale, tile_size * 2 * scale, 0);
			}
			else
			{
				al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 2, tile_size * 2,
					(x - off_x) * scale, (y * scale) - off_y * scale, tile_size * 2 * scale, tile_size * 2 * scale, ALLEGRO_FLIP_HORIZONTAL);
			}

		}
		else
		{

		}
	}

};


//"Enemy" that jumps out of lava and falls back all with a nice 
//rotation
class JumpingLava : public Enemy
{
public:

	float x, y;

	float o_x, o_y;

	float vx = 0, vy = 0;

	int tile_size = 0;
	int jump_frequency = 0;

	float jump_force = 0;

	int frames = 0;

	bool in_jump = false;

	bool first_jump_frame = true;

	std::vector<ALLEGRO_BITMAP*>* sprites;
	std::vector<ALLEGRO_SAMPLE*>* samples;

	//You should spawn at x and y. Please give valid args :)
	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites,
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		this->samples = samples;

		bool first = true;

		std::string input = args;
		std::istringstream ss(input);
		std::string token;

		while (std::getline(ss, token, ',')) {
			//token contains the part of the string
			if (first)
			{
				jump_frequency = std::stoi(token);
				first = false;
			}
			else
			{
				jump_force = std::stof(token);
				break;
			}
		}

		this->sprites = sprites;

		this->x = x * tile_size;
		this->y = y * tile_size; 
		this->tile_size = tile_size;

		o_x = x * tile_size;
		o_y = y * tile_size;
	}

	//Called every frame
	void update(Player* player, Tilemap* map)
	{
		if (!in_jump)
		{
			frames++;

			x = 9999.0f;
			y = 9999.0f;

			if (frames >= jump_frequency)
			{
				in_jump = true;
				first_jump_frame = true;
			}
		}
		else
		{

			Rect player_rect = Rect(player->x, player->y, tile_size * 2, tile_size * 2);

			if (player_rect.contains(Vector2(x + 8, y + 8)))
			{
				player->add_speed(-player->vx, 20.0f);
				player->damage(100);
			}

			if (first_jump_frame)
			{
				x = o_x;
				y = o_y;
				vy = -jump_force;
				first_jump_frame = false;

				//Play fireball effect
				Vector2 pPos = Vector2(player->x, player->y);
				Vector2 mPos = Vector2(x, y);
				float dist = pPos.get_distance(mPos);

				if (dist < 130)
				{
					al_play_sample(samples->at(0), 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
				}
			}
			else
			{
				vy += 0.2f;
				if (vy >= 2.8f)
				{
					vy = 2.8f;
				}

				if (y > o_y)
				{
					//Get out into sleep state
					first_jump_frame = true;
					in_jump = false;
					frames = 0;
				}
				x += vx;
				y += vy;

			}
		}


	}

	//Draw your sprite to screen here
	void draw(float off_x, float off_y, float scale)
	{
		float angle = 0.0f;

		if (vy > 0.5f)
		{
			angle = 3.14f;
		}
		else if(vy < 0.5f)
		{
			angle = 0.0f;
		}
		else
		{
			angle = 45.0f;
		}

		al_draw_scaled_bitmap(sprites->at(1), 0, 0, 32, 32,
			(x - off_x - 16) * scale,
			(y - off_y - 16) * scale,
			32 * scale, 32 * scale, 0);

		al_draw_scaled_rotated_bitmap(sprites->at(0), tile_size, tile_size,
			(x - off_x) * scale, 
			(y - off_y) * scale,
			scale, scale, angle, 0);



	}
};


//An enemy that when close to you stops moving and after a little
//bit explodes
class BombEnemy : public Enemy
{
public:

	float x = 0.0f;
	float y = 0.0f;

	float vx = 0.0f;
	float vy = 0.0f;

	bool alive = true;

	int tile_size = 0;

	std::vector<ALLEGRO_BITMAP*>* sprites;
	std::vector<ALLEGRO_SAMPLE*>* samples;

	bool moving_left = true;

	bool on_air = false;

	bool exploded = false;

	bool about_to_explode = false;
	int explode_frames = 0;

	int frame = 0;

	int t_frm = 0;

	bool fuse_sound_playing = false;

	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites,
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		this->x = x * tile_size - tile_size;
		this->y = y * tile_size - tile_size;
		this->tile_size = tile_size;

		this->sprites = sprites;
		this->samples = samples;
	}

	void update(Player* player, Tilemap* map)
	{
		if (exploded && alive)
		{
			Vector2 player_v2 = Vector2(player->x, player->y);
			Vector2 my_v2 = Vector2(x, y);

			if (my_v2.get_distance(player_v2) < 40)
			{
				player->damage(50);
				player->add_speed(0.0f, -3.0f);
			}

			explode_frames++;
			if (explode_frames >= 12)
			{
				alive = false;
			}
		}
		if (about_to_explode && !exploded && alive)
		{
			ALLEGRO_SAMPLE_ID id;

			if (!fuse_sound_playing)
			{
				fuse_sound_playing = true;
				al_play_sample(samples->at(0), 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, &id);
			}

			explode_frames++;

			if (explode_frames >= 36)
			{
				//Play explosion sfx
				al_play_sample(samples->at(1), 3.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);

				//Do explosion and damage and stuff :P

				frame = 4;

				explode_frames = 0;

				exploded = true;
			}
			else
			{
				frame = 3;
			}

			vx = 0;
			
			

			//Stop from anti gravity if triggered
			on_air = !map->get_tile((x + 8.0f) / tile_size, (y + 16.5f) / tile_size).collides;

			if (on_air)
			{
				if (vy <= 2.5f)
				{
					vy += 0.3f;
				}
				else
				{
					vy = 2.5f;
				}
			}
			else
			{
				vy = 0.0f;
			}

			x += vx;
			y += vy;
		}
		else
		{ 
			if (alive)
			{
				t_frm++;


				Rect player_rect = Rect(player->x, player->y + 3.0f, tile_size * 2, tile_size * 2);
				Vector2 player_v2 = Vector2(player->x, player->y);
				Vector2 my_v2 = Vector2(x, y);
				Rect my_rect = Rect(x, y, tile_size * 2, tile_size * 2);

				if (player_rect.contains(Vector2(x + 8, y + 0.3f)))
				{
					player->add_speed(0.0f, -5.0f);
					alive = false;
					al_play_sample(samples->at(2), 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
				}

				if (my_v2.get_distance(player_v2) <= 60.0f)
				{
					about_to_explode = true;
					//Explode
				}


				if (t_frm >= 20)
				{
					frame++;
					if (frame == 2)
					{
						frame = 0;
					}
					t_frm = 0;
				}

				//Check if we have a block underneath and assign it to on_air
				on_air = !map->get_tile((x + 8.0f) / tile_size, (y + 16.5f) / tile_size).collides;

				if (!on_air)
				{
					vy = 0.0f;

					if (moving_left)
					{
						//Check collisions on our left with tilemap
						if (map->get_tile((x - 0.1f) / tile_size, (y + 8) / tile_size).collides)
						{
							//We are in front of a wall, rotate
							moving_left = false;
						}
						else
						{
							vx = -0.5f;
						}
					}
					else
					{
						//Check collisions on our right with tilemap
						if (map->get_tile((x + 16.1f) / tile_size, (y + 8) / tile_size).collides)
						{
							//We are in front of a wall, rotate
							moving_left = true;
						}
						else
						{
							vx = 0.5f;
						}
					}
				}
				else
				{
					if (vy <= 2.5f)
					{
						vy += 0.3f;
					}
					else
					{
						vy = 2.5f;
					}
				}


				x += vx;
				y += vy;
			}
		}
	}

	void draw(float off_x, float off_y, float scale)
	{
		if (alive)
		{

			if (draw_debug)
			{
				al_draw_rectangle((x - off_x) * scale, (y - off_y) * scale,
					((x + tile_size * 2) - off_x) * scale, ((y + tile_size * 2) - off_y) * scale,
					al_map_rgb(255, 0, 0), 1.0f);

				al_draw_circle((x + 8 - off_x) * scale, (y + 0.3f - off_y) * scale, 2.0f, al_map_rgb(0, 255, 0), 1.0f);

				al_draw_circle((x - off_x) * scale, (y + 8.0f) * scale, 2.0f, al_map_rgb(255, 0, 0), 1.0f);
				al_draw_circle((x + 16 - off_x) * scale, (y + 8.0f) * scale, 2.0f, al_map_rgb(255, 0, 0), 1.0f);

			}

			//printf("Drawing at %f, %f \n", x * scale - off_x, y * scale - off_y);
			if (frame > 4)
			{
				frame = 4;
			}

			if (frame != 4)
			{
				if (moving_left)
				{
					al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 2, tile_size * 2,
						(x - off_x) * scale, (y * scale) - off_y * scale, tile_size * 2 * scale, tile_size * 2 * scale, 0);
				}
				else
				{
					al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 2, tile_size * 2,
						(x - off_x) * scale, (y * scale) - off_y * scale, tile_size * 2 * scale, tile_size * 2 * scale, ALLEGRO_FLIP_HORIZONTAL);
				}
			}
			else
			{
				al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 5, tile_size * 5,
					(x - off_x - tile_size * 2) * scale, (y - off_y - tile_size * 2) * scale, tile_size * 5 * scale, tile_size * 5 * scale, 0);
			}

		}
		else
		{

		}
	}

};


//A pair of sprites rotating around the actual enemy,
//if any of these touch you it damages. No way to kill it
//Looks like a normal ground tile
class SpinEnemy : public Enemy
{
public:

	std::vector<ALLEGRO_BITMAP*>* sprites;
	std::vector<ALLEGRO_SAMPLE*>* samples;

	float x, y;
	int tile_size;

	float angle = 0.0f;

	float radius = 3.0f;

	//Points which kill
	float k1_pos_x, k1_pos_y;
	float k2_pos_x, k2_pos_y;


	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites,
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		this->x = x;
		this->y = y;
		this->tile_size = tile_size;
		
		this->sprites = sprites;
		this->samples = samples;

		this->angle = std::stoi(args);
	}

	void update(Player* player, Tilemap* map)
	{

		Rect p_rect = Rect(player->x, player->y, 14, 14);

		angle += 0.05f;

		k1_pos_x = x + radius * 1.2 * cos(angle);
		k1_pos_y = y + radius * 1.2 * sin(angle);

		// Inverse position
		k2_pos_x = x + radius / 2 * 1.4 * cos(angle);
		k2_pos_y = y + radius / 2 * 1.4 * sin(angle);

		Vector2 k1 = Vector2(k1_pos_x * 8, k1_pos_y * 8);
		Vector2 k2 = Vector2(k2_pos_x * 8, k2_pos_y * 8);

		if (p_rect.contains(k1) || p_rect.contains(k2))
		{
			player->add_speed(0.0f, -3.0f, true);
			player->damage(50);
		}

	}

	void draw(float off_x, float off_y, float scale)
	{
		al_draw_scaled_bitmap(sprites->at(0), 0, 0, 8, 8, (x * scale * 8) - off_x * scale,
			(y * scale * 8) - off_y * scale, 8 * scale, 8 * scale, 0);

		al_draw_scaled_bitmap(sprites->at(1), 0, 0, 8, 8, k1_pos_x * scale * 8 - off_x * scale,
			(k1_pos_y * scale * 8) - off_y * scale, 8 * scale, 8 * scale, 0);

		al_draw_scaled_bitmap(sprites->at(1), 0, 0, 8, 8, k2_pos_x * scale * 8 - off_x * scale,
			(k2_pos_y * scale * 8) - off_y * scale, 8 * scale, 8 * scale, 0);
	}
};

struct CannonBall
{
	float x, y;
	bool left;

	bool exploding = false;
	int explode_frame;
	bool exploded = false;
};

class Cannon : public Enemy
{
public:

	std::vector<CannonBall> cannon_balls = std::vector<CannonBall>();

	bool looking_left = false;

	int fire_frames = 0;

	int tile_size;

	float x, y;

	std::vector<ALLEGRO_BITMAP*>* sprites;
	std::vector<ALLEGRO_SAMPLE*>* samples;

	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites,
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		this->x = x;
		this->y = y;
		this->tile_size = tile_size;

		this->sprites = sprites;
		this->samples = samples;

		if (args == "LEFT")
		{
			looking_left = true;
		}
	}

	//Called every frame
	void update(Player* player, Tilemap* map)
	{
		fire_frames++; 

		Vector2 player_pos = Vector2(player->x + 1, player->y + 1);

		Vector2 c_pos = Vector2(x * 8, y * 8);

		for (int i = 0; i < cannon_balls.size(); i++)
		{
			CannonBall*  c = &cannon_balls[i];

			if (!c->exploded)
			{

				Vector2 m_pos = Vector2(c->x * 8, c->y * 8);

				float dist = player_pos.get_distance(m_pos);

				if (!c->exploding)
				{

					if (c->left)
					{
						c->x -= 0.2f;

						if (map->get_tile((c->x - 1), c->y).collides)
						{
							c->exploding = true;
							c->explode_frame = 0;
						}

					}
					else
					{
						c->x += 0.2f;
						if (map->get_tile((c->x + 1), c->y).collides)
						{
							c->exploding = true;
							c->explode_frame = 0;

						}
					}

					if (dist <= 8.0f)
					{
						c->exploding = true;
						c->explode_frame = 0;
					}


					

				}
				else
				{
					if (c->explode_frame == 0)
					{
						//Check if we can damage player
						

						if (dist <= 50)
						{
							//Play sound
							al_play_sample(samples->at(1), 2.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
							if (dist <= 50)
							{
								//Damage
								player->damage(50);
							}
						}
					}

					c->explode_frame++;

					if (c->explode_frame >= 10)
					{
						c->exploded = true;
					}
				}
			}

			
		}

		if (fire_frames >= 180)
		{
			fire_frames = 0;

			//Fire a cannonball

			if (c_pos.get_distance(player_pos) <= 140.0f)
			{
				al_play_sample(samples->at(0), 2.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
			}

			CannonBall n = CannonBall();
			
			n.left = looking_left;

			if (looking_left)
			{
				n.x = x - 1.0f;
				n.y = y;
			}
			else
			{
				n.x = x + 1.0f;
				n.y = y;
			}

			cannon_balls.push_back(n);
		}
	}

	//Draw your sprite to screen here
	void draw(float off_x, float off_y, float scale)
	{
		
		if (looking_left)
		{
			al_draw_scaled_bitmap(sprites->at(0), 0, 0, 8, 8,
				(x * scale * 8) - off_x * scale,
				(y * scale * 8) - off_y * scale,
				8 * scale, 8 * scale, ALLEGRO_FLIP_HORIZONTAL);
		}
		else
		{
			al_draw_scaled_bitmap(sprites->at(0), 0, 0, 8, 8,
				(x * scale * 8) - off_x * scale,
				(y * scale * 8) - off_y * scale,
				8 * scale, 8 * scale, 0);
		}
		

		for (int i = 0; i < cannon_balls.size(); i++)
		{
			if (!cannon_balls[i].exploded)
			{
				if (cannon_balls[i].exploding)
				{
					al_draw_scaled_bitmap(sprites->at(2), 0, 0, 40, 40,
						(cannon_balls[i].x * scale * 8 - 16 * scale) - off_x * scale,
						(cannon_balls[i].y * scale * 8 - 16 * scale) - off_y * scale,
						40 * scale, 40 * scale, 0);
				}
				else
				{
					al_draw_scaled_bitmap(sprites->at(1), 0, 0, 8, 8,
						(cannon_balls[i].x * scale * 8) - off_x * scale,
						(cannon_balls[i].y * scale * 8) - off_y * scale,
						8 * scale, 8 * scale, 0);
				}
			}
		}
	}

};

//Enemy that flies following you, and after a while sticks to a tile on top of it and disappears
class FlyingEnemy : public Enemy
{
public:
	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites, std::string args)
	{

	}

	void update(Player* player, Tilemap* map)
	{

	}

	void draw(float off_x, float off_y, float scale)
	{

	}
};


struct FlyingBlade
{
	float x, y;

	//Is the blade going left
	bool left = false;

	int frame = 0;

	bool dying = false;

	bool dead = false;
};

//Throws stuff that damages
class ThrowingEnemy : public Enemy
{
public:

	float x = 0.0f;
	float y = 0.0f;

	float vx = 0.0f;
	float vy = 0.0f;

	bool alive = true;

	int tile_size = 0;

	std::vector<ALLEGRO_BITMAP*>* sprites;
	std::vector<ALLEGRO_SAMPLE*>* samples;

	//Flying blades!
	std::vector<FlyingBlade> blades = std::vector<FlyingBlade>();

	bool moving_left = true;

	bool on_air = false;

	int frame = 0;

	int t_frm = 0;

	int throw_frame = 0;

	void setup(int x, int y, int tile_size, std::vector<ALLEGRO_BITMAP*>* sprites,
		std::vector<ALLEGRO_SAMPLE*>* samples, std::string args)
	{
		this->x = x * tile_size - tile_size;
		this->y = y * tile_size - tile_size;
		this->tile_size = tile_size;

		this->sprites = sprites;
		this->samples = samples;

	}

	void update(Player* player, Tilemap* map)
	{
		if (alive)
		{
			throw_frame++;
			t_frm++;


			Rect player_rect = Rect(player->x, player->y + 3.0f, tile_size * 2, tile_size * 2);
			Rect my_rect = Rect(x, y, tile_size * 2, tile_size * 2);

			Vector2 m_v = Vector2(x, y);
			Vector2 p_v = Vector2(player->x, player->y);

			if (throw_frame >= 120 && m_v.get_distance(p_v) < 160.0f)
			{
				FlyingBlade n_blade = FlyingBlade();
				n_blade.left = moving_left;
				n_blade.x = x;
				n_blade.y = y;
				blades.push_back(n_blade);
				throw_frame = 0;
			}

			for (int i = 0; i < blades.size(); i++)
			{
				FlyingBlade* b = &blades[i];

				if (!b->dead)
				{
					if (b->dying)
					{
						if (b->frame == 2)
						{
							b->dead = true;
							b->dying = false;
						}

						b->frame++;
					}
					else
					{
						if (b->left)
						{
							b->x -= 1.3f;
						}
						else
						{
							b->x += 1.3f;
						}

						if (map->get_tile(b->x / tile_size, b->y / tile_size).collides)
						{
							b->dying = true;
							b->frame = 0;
						}
					}

					Vector2 b_v = Vector2(b->x + 4, b->y + 4);

					if (player_rect.contains(b_v) == true)
					{

						printf("Collided with player");

						if (b->left)
						{
							player->add_speed(-1.0f, -1.f, true);
							player->damage(20);
						}
						else
						{
							player->add_speed(1.0f, -1.f, true);
							player->damage(20);
						}

						b->dying = true;
					}

				}

			}

			if (player_rect.contains(Vector2(x + 8, y + 0.3f)))
			{
				player->add_speed(0.0f, -5.0f);
				al_play_sample(samples->at(0), 1.0f, 0.0f, 1.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
				alive = false;
			}

			if (player_rect.contains(Vector2(x, y + 8)))
			{
				player->add_speed(-2.8f, -4.0f);
				player->damage(5);
			}

			if (player_rect.contains(Vector2(x + 16, y + 8)))
			{
				player->add_speed(2.8f, -4.0f);
				player->damage(5);
			}

			if (t_frm >= 20)
			{
				frame++;
				if (frame == 2)
				{
					frame = 0;
				}
				t_frm = 0;
			}

			//Check if we have a block underneath and assign it to on_air
			on_air = !map->get_tile((x + 8.0f) / tile_size, (y + 16.5f) / tile_size).collides;

			if (!on_air)
			{
				vy = 0.0f;

				//Look to player
				/*if(m_v.get_distance(p_v) <= 100.0f)
				{
					if (m_v.x < p_v.x)
					{
						moving_left = false;
					}
					else
					{
						moving_left = true;
					}
				}
				*/


				if (moving_left)
				{
					//Check collisions on our left with tilemap
					if (map->get_tile((x - 0.1f) / tile_size, (y + 8) / tile_size).collides || rand() % 1000 >= 975)
					{
						//We are in front of a wall, rotate
						moving_left = false;
					}
					else
					{
						vx = -0.5f;
					}
				}
				else
				{
					//Check collisions on our right with tilemap
					if (map->get_tile((x + 16.1f) / tile_size, (y + 8) / tile_size).collides || rand() % 1000 >= 975)
					{
						//We are in front of a wall, rotate
						moving_left = true;
					}
					else
					{
						vx = 0.5f;
					}
				}
			}
			else
			{
				if (vy <= 2.5f)
				{
					vy += 0.3f;
				}
				else
				{
					vy = 2.5f;
				}
			}


			x += vx;
			y += vy;
		}
		else
		{
			for (int i = 0; i < blades.size(); i++)
			{
				blades[i].dead = true;
			}
		}
	}

	void draw(float off_x, float off_y, float scale)
	{

		for (int i = 0; i < blades.size(); i++)
		{
			FlyingBlade b = blades[i];
			if (!b.dead)
			{
				if (b.dying)
				{
					al_draw_scaled_bitmap(sprites->at(3 + b.frame), 0, 0, 6, 6,
						(b.x - off_x) * scale, (b.y - off_y) * scale, 6 * scale, 6 * scale, 0);
				}
				else
				{
					al_draw_scaled_bitmap(sprites->at(2), 0, 0, 6, 6,
						(b.x - off_x) * scale, (b.y - off_y) * scale, 6 * scale, 6 * scale, 0);
				}
			}
		}

		if (alive)
		{
			if (draw_debug)
			{
				al_draw_rectangle((x - off_x) * scale, (y - off_y) * scale,
					((x + tile_size * 2) - off_x) * scale, ((y + tile_size * 2) - off_y) * scale,
					al_map_rgb(255, 0, 0), 1.0f);

				al_draw_circle((x + 8 - off_x) * scale, (y + 0.3f - off_y) * scale, 2.0f, al_map_rgb(0, 255, 0), 1.0f);

				al_draw_circle((x - off_x) * scale, (y + 8.0f) * scale, 2.0f, al_map_rgb(255, 0, 0), 1.0f);
				al_draw_circle((x + 16 - off_x) * scale, (y + 8.0f) * scale, 2.0f, al_map_rgb(255, 0, 0), 1.0f);
			}

			//printf("Drawing at %f, %f \n", x * scale - off_x, y * scale - off_y);
			if (moving_left)
			{
				al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 2, tile_size * 2,
					(x - off_x) * scale, (y * scale) - off_y * scale, tile_size * 2 * scale, tile_size * 2 * scale, 0);
			}
			else
			{
				al_draw_scaled_bitmap(sprites->at(frame), 0, 0, tile_size * 2, tile_size * 2,
					(x - off_x) * scale, (y * scale) - off_y * scale, tile_size * 2 * scale, tile_size * 2 * scale, ALLEGRO_FLIP_HORIZONTAL);
			}

		}
		else
		{

		}
	}

};