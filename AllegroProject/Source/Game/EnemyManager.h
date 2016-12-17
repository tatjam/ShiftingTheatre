#pragma once

#include <allegro5/allegro5.h>
#include <vector>
#include "Enemy.h"

class EnemyManager
{

private:

	bool colors_equal(ALLEGRO_COLOR a, ALLEGRO_COLOR b)
	{
		unsigned char a_r, a_g, a_b, b_r, b_g, b_b;

		al_unmap_rgb(a, &a_r, &a_g, &a_b);
		al_unmap_rgb(b, &b_r, &b_g, &b_b);

		return a_r == b_r && a_g == b_g && a_b == b_b;
	}

public:


	std::vector<Enemy*> enemies = std::vector<Enemy*>();


	ALLEGRO_COLOR basic_color = al_map_rgb(255, 180, 0);
	ALLEGRO_COLOR lava_color = al_map_rgb(100, 0, 0);
	ALLEGRO_COLOR bomb_color = al_map_rgb(255, 110, 0);
	ALLEGRO_COLOR thrower_color = al_map_rgb(100, 50, 30);
	ALLEGRO_COLOR spinner_color = al_map_rgb(180, 180, 80);

	ALLEGRO_COLOR cannon_color_r = al_map_rgb(50, 50, 50);
	ALLEGRO_COLOR cannon_color_l = al_map_rgb(80, 80, 80);

	std::vector<ALLEGRO_BITMAP*>* basic_sprites;
	std::vector<ALLEGRO_BITMAP*>* lava_sprites;
	std::vector<ALLEGRO_BITMAP*>* bomb_sprites;
	std::vector<ALLEGRO_BITMAP*>* thrower_sprites;
	std::vector<ALLEGRO_BITMAP*>* spinner_sprites;

	std::vector<ALLEGRO_BITMAP*>* cannon_sprites;

	std::vector<ALLEGRO_SAMPLE*>* basic_samples;
	std::vector<ALLEGRO_SAMPLE*>* lava_samples;
	std::vector<ALLEGRO_SAMPLE*>* bomb_samples;
	std::vector<ALLEGRO_SAMPLE*>* thrower_samples;

	std::vector<ALLEGRO_SAMPLE*>* cannon_samples;

	void give_sprites(std::vector<ALLEGRO_BITMAP*>* basic_sprites,
		std::vector<ALLEGRO_BITMAP*>* lava_sprites,
		std::vector<ALLEGRO_BITMAP*>* bomb_sprites,
		std::vector<ALLEGRO_BITMAP*>* thrower_sprites,
		std::vector<ALLEGRO_BITMAP*>* spinner_sprites,
		std::vector<ALLEGRO_BITMAP*>* cannon_sprites)
	{
		this->basic_sprites = basic_sprites;
		this->lava_sprites = lava_sprites;
		this->bomb_sprites = bomb_sprites;
		this->thrower_sprites = thrower_sprites;
		this->spinner_sprites = spinner_sprites;
		this->cannon_sprites = cannon_sprites;
	}

	void give_samples(std::vector<ALLEGRO_SAMPLE*>* basic_samples,
		std::vector<ALLEGRO_SAMPLE*>* lava_samples,
		std::vector<ALLEGRO_SAMPLE*>* bomb_samples,
		std::vector<ALLEGRO_SAMPLE*>* thrower_samples,
		std::vector<ALLEGRO_SAMPLE*>* cannon_samples)
	{
		this->basic_samples = basic_samples;
		this->lava_samples = lava_samples;
		this->bomb_samples = bomb_samples;
		this->thrower_samples = thrower_samples;
		this->cannon_samples = cannon_samples;
	}

	//Creates all enemies from tilemap
	void setup(ALLEGRO_BITMAP* map, int tile_size)
	{
		for (int x = 0; x < al_get_bitmap_width(map); x++)
		{
			for (int y = 0; y < al_get_bitmap_height(map); y++)
			{
				ALLEGRO_COLOR c = al_get_pixel(map, x, y);

				if (colors_equal(c, basic_color))
				{
					//Spawn a BasicEnemy
					BasicEnemy* n = new BasicEnemy();
					n->setup(x, y, tile_size, basic_sprites, basic_samples, "NONE");
					enemies.push_back(n);
					printf("Found basic enemy at %i, %i\n", x, y);
				}
				else if (colors_equal(c, lava_color))
				{
					JumpingLava* n = new JumpingLava();
					n->setup(x, y, tile_size, lava_sprites, lava_samples, "60,5");
					enemies.push_back(n);
					printf("Found lava enemy at %i, %i\n", x, y);
				}
				else if (colors_equal(c, bomb_color))
				{
					BombEnemy* n = new BombEnemy();
					n->setup(x, y, tile_size, bomb_sprites, bomb_samples, "NONE");
					enemies.push_back(n);
					printf("Found bomb enemy at %i, %i\n", x, y);
				}
				else if (colors_equal(c, thrower_color))
				{
					ThrowingEnemy* n = new ThrowingEnemy();
					n->setup(x, y, tile_size, thrower_sprites, thrower_samples, "NONE");
					enemies.push_back(n);
					printf("Found thrower enemy at %i, %i\n", x, y);
				}
				else if (colors_equal(c, spinner_color))
				{
					SpinEnemy* n = new SpinEnemy();
					srand(x * y);
					n->setup(x, y, tile_size, spinner_sprites, NULL, std::to_string(rand() % 10));
					enemies.push_back(n);
					printf("Found spinner enemy at %i, %i\n", x, y);
				}
				else if (colors_equal(c, cannon_color_l))
				{
					Cannon* n = new Cannon();
					n->setup(x, y, tile_size, cannon_sprites, cannon_samples, "LEFT");
					enemies.push_back(n);
					printf("Found spinner enemy at %i, %i\n", x, y);
				}
				else if (colors_equal(c, cannon_color_r))
				{
					Cannon* n = new Cannon();
					srand(x * y);
					n->setup(x, y, tile_size, cannon_sprites, cannon_samples, "RIGHT");
					enemies.push_back(n);
					printf("Found spinner enemy at %i, %i\n", x, y);
				}
			}
		}
	}


	//Will destroy every enemy. To be used when changing levels
	void clear()
	{
		for (int i = 0; i < enemies.size(); i++)
		{
			delete enemies[i];
		}

		enemies = std::vector<Enemy*>();
	}

	void update(Player* player, Tilemap* tilemap, int ticks)
	{
		if (ticks >= 210)
		{
			for (int i = 0; i < enemies.size(); i++)
			{
				enemies[i]->update(player, tilemap);
			}
		}
	}

	void draw(float off_x, float off_y, float scale)
	{
		for (int i = 0; i < enemies.size(); i++)
		{
			enemies[i]->draw(off_x, off_y, scale);
		}
	}

	EnemyManager()
	{

	}

	~EnemyManager()
	{

	}
};

