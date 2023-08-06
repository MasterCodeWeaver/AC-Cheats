#include "Utilities.h"

Vector3 Vector3::zero = { 0.0, 0.0, 0.0 };
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
