#pragma once
#include <iostream>
#include <allegro5\allegro.h>
#include <vector>
#include <map>
#include "allegro5/allegro_native_dialog.h"
#include "PlatformManager.h"

using std::string;


struct Tile
{
	ALLEGRO_COLOR c;
	ALLEGRO_BITMAP* tex;
	bool real_tile = false;
	bool collides = false;


	//Stupid stuff to work with std::map
	unsigned int get_color()
	{
		unsigned char r, g, b;
		al_unmap_rgb(c, &r, &g, &b);
		unsigned int out = 0;
		out += r;
		out = out << 8;
		out += g;
		out = out << 8;
		out += b;
		return out;
	}

	Tile(ALLEGRO_BITMAP* b, bool collides, ALLEGRO_COLOR rc)
	{
		tex = b;
		this->collides = collides;
		c = rc;
	}

	Tile()
	{
		tex = NULL;
		this->collides = false;
		c = al_map_rgb(0, 0, 0);
	}
};

//Is actually an image, not more
class Tilemap
{
private:

	unsigned int break_color(ALLEGRO_COLOR c)
	{
		unsigned char r, g, b;
		al_unmap_rgb(c, &r, &g, &b);
		unsigned int out = 0;
		out += r;
		out = out << 8;
		out += g;
		out = out << 8;
		out += b;
		return out;
	}

	std::map<unsigned int, Tile> tiles;



	ALLEGRO_DISPLAY* display;

	ALLEGRO_BITMAP* buff = NULL;

	int tile_size = 8;

public:

	PlatformManager platform_manager = PlatformManager();

	ALLEGRO_BITMAP* map = NULL;

	void add_tile(Tile t)
	{
		tiles[t.get_color()] = t;
		tiles[t.get_color()].real_tile = true;
	}

	bool load_file(string path)
	{
		printf("Loading tilemap: %s ", path.c_str());
		map = al_load_bitmap(path.c_str());


		if (map == NULL)
		{
			/*al_show_native_message_box(display, "Error", "Error", "Failed to load tilemap!",
			NULL, ALLEGRO_MESSAGEBOX_ERROR);*/

			abort();
			return false;
		}
		else
		{
			return true;
		}
	}

	ALLEGRO_COLOR get_tile_raw(int x, int y)
	{
		return al_get_pixel(map, x, y);
	}

	Tile get_tile(int x, int y)
	{

		Tile out = Tile();



		ALLEGRO_COLOR t_c = al_get_pixel(map, x, y);
		unsigned int t_c_compound = break_color(t_c);
		std::map<unsigned int, Tile>::iterator it = tiles.find(t_c_compound);

		if (it != tiles.end())
		{
			out = tiles[t_c_compound];
		}

		return out;
	}


	//Renders and generates platforms
	void render(float scale, ALLEGRO_DISPLAY* disp, ALLEGRO_BITMAP* b)
	{
		platform_manager.clear();
		platform_manager.find_platforms(map);

		if (map != NULL)
		{
			buff = al_create_bitmap(al_get_bitmap_width(map) * tile_size, al_get_bitmap_height(map) * tile_size);

			al_set_target_bitmap(buff);

			for (int x = 0; x < al_get_bitmap_width(buff); x++)
			{
				for (int y = 0; y < al_get_bitmap_height(buff); y++)
				{
					Tile t = get_tile(x, y);
					//We can have tiles without texture
					if (t.tex != NULL)
					{
						//int or_size = al_get_bitmap_width(t.tex);

						al_draw_bitmap(t.tex, x * tile_size, y * tile_size, 0);

					}
				}
			}

			al_set_target_bitmap(al_get_backbuffer(disp));

		}


		printf("Should render now!");
		//platform_manager->render(display);
	}

	//Required for platforms
	void update()
	{
		platform_manager.update();
	}

	void draw(float x_off, float y_off, float scale, int disp_x, int disp_y)
	{

		//platform_manager->draw(scale, x_off, y_off);
		al_draw_scaled_bitmap(buff, 0, 0, al_get_bitmap_width(buff), al_get_bitmap_height(buff),
			-x_off * scale, -y_off * scale, al_get_bitmap_width(buff) * scale, al_get_bitmap_height(buff) * scale, 0);

		platform_manager.draw(x_off, y_off, scale);
	}

	Tilemap(ALLEGRO_DISPLAY* display, int tile_size)
	{
		this->display = display;
		this->tile_size = tile_size;
	}

	~Tilemap()
	{
		//delete &map;
	}
};

