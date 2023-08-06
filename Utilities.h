#pragma once
#include <string>
#include <iostream>
#include <SDL.h>

class Vector3;

class Vector3
{
public:
	static Vector3 zero;

	float x, y, z;

	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
		os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
		return os;
	}

	bool operator==(const Vector3& other) {
		return (this->x == other.x && this->y == other.y && this->z == other.z);
	}

};

struct Vector2
{
	float x, y;
};

class Player
{
public:
	int health = 0;
	Vector3 position{ 0, 0, 0 };
	Vector2 viewDirection{ 0, 0 };
};


void DrawCircle(SDL_Renderer* renderer, int cx, int cy, int r);