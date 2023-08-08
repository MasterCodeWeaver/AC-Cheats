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

	inline double Dot(Vector3 other) { return x * other.x + y * other.y + z * other.z; }

	inline Vector3 Normalized() { return *this / this->Magnitude(); }

	inline double Magnitude() { return sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2)); }

	friend std::ostream& operator<<(std::ostream& os, const Vector3& vec) {
		os << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
		return os;
	}

	bool operator==(const Vector3& other) {
		return (this->x == other.x && this->y == other.y && this->z == other.z);
	}

	Vector3 operator-(const Vector3& other) {
		return { this->x - other.x, this->y - other.y, this->z - other.z };
	}

	Vector3 operator/(const int value) { return { this->x / value, this->y / value, this->z / value }; }
};

struct Vector2
{
	float x, y;

	friend std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
		os << "(" << vec.x << ", " << vec.y << ")";
		return os;
	}
};
double Distance(const Vector3& a, const Vector3& b);

class Player
{
public:
	int health = 0;
	Vector3 position{ 0, 0, 0 };
	Vector2 viewDirection{ 0, 0 };
};


void DrawCircle(SDL_Renderer* renderer, int cx, int cy, int r);