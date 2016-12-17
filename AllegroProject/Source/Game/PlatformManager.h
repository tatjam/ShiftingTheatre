#pragma once
#include "allegro5/allegro5.h"
#include "allegro5/allegro_primitives.h"""
#include <algorithm>
#include "../Math/Geometry.h"
class Platform
{
public:

	float x, y;
	float sX, sY;
	float dX, dY;

	float vx, vy;

	bool going_to_end = true;
};

//Handles creation of platforms and realtime updating
class PlatformManager
{
private:

	bool more_or_less_equal(float a, float b, float sigma = 0.2f)
	{
		if (abs(a - b) <= sigma)
		{
			return true;
		}

		return false;
	}

public:

	ALLEGRO_BITMAP* platform_bm;

	std::vector<Platform> platforms = std::vector<Platform>();

	void find_platforms(ALLEGRO_BITMAP* map)
	{
		for (int x = 0; x < al_get_bitmap_width(map); x++)
		{
			for (int y = 0; y < al_get_bitmap_height(map); y++)
			{
				ALLEGRO_COLOR c = al_get_pixel(map, x, y);
				unsigned char r, g, b;

				al_unmap_rgb(c, &r, &g, &b);

				if (r == 0 && g == 255 & b == 0)
				{

					printf("FOUND PLATFORM. OUTCOME: ");

					bool horizontal = false;
					bool found = false;

					float off;

					//Found platform start:
					// Search horizontally and vertically for 
					// platform end. Stop if any other thing other than white is found

					bool h_done = false;
					bool backwards = false;
					int oX = 0;

					while (!h_done)
					{
						if (backwards)
						{
							oX--;
						}
						else
						{
							oX++;
						}

						ALLEGRO_COLOR c = al_get_pixel(map, x + oX, y);
						unsigned char r, g, b;

						al_unmap_rgb(c, &r, &g, &b);

						if (r == 255 && g == 0 && b == 255)
						{
							//Found end!
							h_done = true;
							off = oX;
							horizontal = true;
							found = true;
						}
						else
						{
							if (r == 255 & g == 255 & b == 255)
							{
								//Air, keep going 
								continue;
							}
							else
							{
								if (backwards)
								{
									//Nothing found, stop
									h_done = true;
								}
								else
								{
									//No +H found
									// Search -H
									backwards = true;
									oX = 0;
									continue;
								}
							}
						}
					}

					h_done = false;
					oX = 0;

					if (!found)
					{
						//Do vertical
						while (!h_done)
						{
							if (backwards)
							{
								oX--;
							}
							else
							{
								oX++;
							}

							ALLEGRO_COLOR c = al_get_pixel(map, x, y + oX);
							unsigned char r, g, b;

							al_unmap_rgb(c, &r, &g, &b);

							if (r == 255 && g == 0 && b == 255)
							{
								//Found end!
								h_done = true;
								found = true;
								off = oX;
								horizontal = false;
							}
							else
							{
								if (r == 255 & g == 255 & b == 255)
								{
									//Air, keep going 
									continue;
								}
								else
								{
									if (backwards)
									{
										//Nothing found, break
										h_done = true;
									}
									else
									{
										//No +H found
										// Search -H
										backwards = true;
										oX = 0;
										continue;
									}
								}
							}
						}

					}

					//After search is complete
					if(found == false)
					{
						printf("COULD NOT FIND PLATFORM END\n");
					}
					else
					{
						printf("FOUND END SUCCESSFULLY\n");
						Platform n = Platform();
						n.x = x;
						n.y = y;
						n.sX = x;
						n.sY = y;
						
						if (horizontal)
						{
							n.dX = x + off;
							n.dY = y;
						}
						else
						{
							n.dX = x;
							n.dY = y + off;
						}

						platforms.push_back(n);
					}
				}
			}
		}
	}

	void clear()
	{
		platforms = std::vector<Platform>();
	}


	//Returns the platform the point is inside of, if the point
	//is not inside any returns NULL
	Platform* point_collides(float x, float y)
	{
		Vector2 p = Vector2(x, y);
		for (int i = 0; i < platforms.size(); i++)
		{

			Rect p_rect = Rect(platforms[i].x, platforms[i].y, 3, 1);
			if (p_rect.contains(p))
			{
				return &platforms[i];
			}
		}
		return NULL;
	}

	void update()
	{
		for (int i = 0; i < platforms.size(); i++)
		{
			Platform* p = &platforms[i];

			if (p->going_to_end)
			{
				if (more_or_less_equal(p->x, p->dX) && more_or_less_equal(p->y, p->dY))
				{
					p->going_to_end = false;
				}
				else
				{
					
					//We are not at destination

					if (p->dX == p->x)
					{
						//Vertical movement
						if (p->dY < p->y)
						{
							//Destination is higher than we are, move up
							p->y -= 0.1f;
							p->vy = -0.1f;
						}
						else
						{
							//Destination is lower than we are
							p->y += 0.1f;
							p->vy = 0.1f;
						}
					}
					else
					{
						//Horizontal movement
						if (p->dX < p->x)
						{
							//Destination is to the left, move left
							p->x -= 0.1f;
							p->vx = -0.1f;
						}
						else
						{
							//Destination is to the right
							p->x += 0.1f;
							p->vx = 0.1f;
						}
					}
				}
					

			}
			else
			{
				if (more_or_less_equal(p->x, p->sX) && more_or_less_equal(p->y, p->sY))
				{
					p->going_to_end = true;
				}
				else
				{
					//We are not at destination

					if (p->sX == p->x)
					{
						//Vertical movement
						if (p->sY < p->y)
						{
							//Destination is higher than we are, move up
							p->y -= 0.1f;
							p->vy = -0.1f;
						}
						else
						{
							//Destination is lower than we are
							p->y += 0.1f;
							p->vy = 0.1f;
						}
					}
					else
					{
						//Horizontal movement
						if (p->sX < p->x)
						{
							//Destination is to the left, move left
							p->x -= 0.1f;
							p->vx = -0.1f;
						}
						else
						{
							//Destination is to the right
							p->x += 0.1f;
							p->vx = 0.1f;
						}
					}
				}
			}
		}
	}

	void draw(float off_x, float off_y, float scale)
	{
		for (int i = 0; i < platforms.size(); i++)
		{
			al_draw_scaled_bitmap(platform_bm, 0, 0, 24, 8,
				(platforms[i].x * 8 * scale) - off_x * scale,
				(platforms[i].y * 8 * scale) - off_y * scale,
				24 * scale, 8 * scale, 0);
		}
	}

	PlatformManager()
	{

	}

};