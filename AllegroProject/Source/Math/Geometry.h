#pragma once
#include <iostream>


class Vector2
{
public:

	float x;
	float y;

	char buff[128];

	Vector2(float x, float y)
	{
		this->x = x;
		this->y = y;
	}

	Vector2()
	{
		this->x = 0;
		this->y = 0;
	}

	void set(float nx, float ny)
	{
		x = nx;
		y = ny;
	}

	char* get_display_string()
	{
		sprintf_s(buff, "{ float x = %f, float y = %f }", x, y);

		return buff;
	}

	float get_distance(Vector2 p)
	{
		return sqrt(pow(x - p.x, 2) + pow(y - p.y, 2));
	}

	float get_angle(Vector2 p)
	{
		atan2(p.y - y, p.x - x);
	}


	void operator+= (Vector2 p2)
	{
		x = x + p2.x; y = y + p2.y;
	}
};

struct Rect
{
	float x;
	float y;
	float w;
	float h;


	Rect(float x, float y, float w, float h)
	{
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}

	void set(float nx, float ny, float nw, float nh)
	{
		x = nx;
		y = ny;
		w = nw;
		h = nh;
	}

	bool contains(Vector2 point)
	{
		return (point.x > x && point.y > y && point.x < x + w && point.y < y + h);
	}

};