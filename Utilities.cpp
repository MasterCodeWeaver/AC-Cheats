#include "Utilities.h"
#include <math.h>

Vector3 Vector3::zero = { 0.0, 0.0, 0.0 };

double Distance(const Vector3& a, const Vector3& b)
{
	return sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2) + pow(b.z - a.z, 2));
}

void DrawCircle(SDL_Renderer* renderer, int cx, int cy, int r)
{
	for (int i = r; i >= 0; i--) 
	{
		for (float angle = 0; angle < 2 * M_PI; angle += 0.01)
		{
			int x = cx + (cos(angle) * i);
			int y = cy + (sin(angle) * i);

			SDL_RenderDrawPoint(renderer, x, y);
		}
	}
	
}
