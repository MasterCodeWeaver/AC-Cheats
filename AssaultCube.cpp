#include <iostream>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <tchar.h>
#include <vector>
#include <stdlib.h>
#include <SDL.h>
#include "Utilities.h"
#include <format>


DWORD GetModuleBaseAddress(TCHAR* lpszModuleName, DWORD pID) {
	DWORD dwModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID); // make snapshot of all modules within process
	MODULEENTRY32 ModuleEntry32 = { 0 };
	ModuleEntry32.dwSize = sizeof(MODULEENTRY32);

	if (Module32First(hSnapshot, &ModuleEntry32)) //store first Module in ModuleEntry32
	{
		do {
			if (_tcscmp(ModuleEntry32.szModule, lpszModuleName) == 0) // if Found Module matches Module we look for -> done!
			{
				dwModuleBaseAddress = (DWORD)ModuleEntry32.modBaseAddr;
				break;
			}
		} while (Module32Next(hSnapshot, &ModuleEntry32)); // go through Module entries in Snapshot and store in ModuleEntry32


	}
	CloseHandle(hSnapshot);
	return dwModuleBaseAddress;
}

DWORD GetPointerAddress(HWND hwnd, DWORD gameBaseAddr, DWORD address, std::vector<DWORD> offsets)
{
	DWORD pID = NULL; // Game process ID
	GetWindowThreadProcessId(hwnd, &pID);
	HANDLE phandle = NULL;
	phandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pID);
	if (phandle == INVALID_HANDLE_VALUE || phandle == NULL);

	DWORD offset_null = NULL;
	ReadProcessMemory(phandle, (LPVOID*)(gameBaseAddr + address), &offset_null, sizeof(offset_null), 0);
	DWORD pointeraddress = offset_null; // the address we need
	for (int i = 0; i < offsets.size() - 1; i++) // we dont want to change the last offset value so we do -1
	{
		ReadProcessMemory(phandle, (LPVOID*)(pointeraddress + offsets.at(i)), &pointeraddress, sizeof(pointeraddress), 0);
	}
	return pointeraddress += offsets.at(offsets.size() - 1); // adding the last offset
}

int main(int argc, char* argv[])
{
	HWND hwnd = FindWindowA(nullptr, "AssaultCube");

	if (hwnd == FALSE) { std::cout << "Could Not Find Window!\n"; return -1; }

	DWORD processID = NULL;
	GetWindowThreadProcessId(hwnd, &processID);
	HANDLE pHandle = NULL;
	pHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);

	if (pHandle == INVALID_HANDLE_VALUE || pHandle == NULL) { std::cout << "Could Not Set Process Handle or ID"; return -1; }

	DWORD gameBaseAddress = GetModuleBaseAddress((TCHAR*)"ac_client.exe", processID);

	DWORD playerBaseAddress = 0x17E0A8;
	DWORD entityListBaseAddr = 0x00191FCC;

	std::vector<DWORD> healthOffsets = { 0xEC };

	DWORD healthPtrAddr = GetPointerAddress(hwnd, gameBaseAddress, playerBaseAddress, { 0xEC });
	DWORD positionPtrAddr = GetPointerAddress(hwnd, gameBaseAddress, playerBaseAddress, { 0x4 });
	DWORD namePtrAddr = GetPointerAddress(hwnd, gameBaseAddress, playerBaseAddress, { 0x205 });
	DWORD viewDirectionAddr = GetPointerAddress(hwnd, gameBaseAddress, playerBaseAddress, { 0x34 });


	DWORD entityPtrAddr = GetPointerAddress(hwnd, gameBaseAddress, entityListBaseAddr, { 0x0, 0x4 });

	//std::cout << std::showbase << std::hex << healthPtrAddr << std::endl;
	std::cout << std::format("{:#x}", healthPtrAddr);

	Player player;

	Vector3 entityPosition = { 0, 0, 0 };

	bool quit = false;
	SDL_Window* window = SDL_CreateWindow("Map",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, 500, 500, 0);

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	while (!quit)
	{
		SDL_Event event;
		SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
		SDL_RenderClear(renderer);

		ReadProcessMemory(pHandle, (LPCVOID*)(healthPtrAddr), &player.health, sizeof(int), 0);
		ReadProcessMemory(pHandle, (LPCVOID*)(positionPtrAddr), &player.position, sizeof(Vector3), 0);
		ReadProcessMemory(pHandle, (LPCVOID*)(viewDirectionAddr), &player.viewDirection, sizeof(Vector2), 0);

		//ReadProcessMemory(pHandle, (LPCVOID*)(entityPtrAddr), &entityPosition, sizeof(Vector3), 0);
		
		//std::cout << player.position << std::endl;

		//std::cout << std::dec << player.position.z << std::endl;
		//std::cout << std::dec << player.health << std::endl;

		player.health = 69420;

		WriteProcessMemory(pHandle, (LPVOID)healthPtrAddr, &player.health, sizeof(int), nullptr);

		SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
		DrawCircle(renderer, (int)player.position.x, (int)player.position.y, 5);
		SDL_RenderDrawLine(renderer, (int)player.position.x, (int)player.position.y, 
			(int)(player.position.x + cos((player.viewDirection.x - 90) * (M_PI / 180)) * 10), 
			(int)(player.position.y + sin((player.viewDirection.x - 90) * (M_PI / 180)) * 10));


		double closestDistance = INFINITY;
		Vector3 closestEntity;
		for (DWORD entityOffset = 0x4; entityOffset <= (0x0 + (0x4 * 8)); entityOffset += 0x4)
		{
			DWORD entityPtrAddr = GetPointerAddress(hwnd, gameBaseAddress, entityListBaseAddr, { entityOffset, 0x4 });
			ReadProcessMemory(pHandle, (LPCVOID*)(entityPtrAddr), &entityPosition, sizeof(Vector3), 0);

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			DrawCircle(renderer, entityPosition.x, entityPosition.y, 5);

			if ((player.position - entityPosition).Magnitude() < closestDistance) {
				closestDistance = (player.position - entityPosition).Magnitude();
				closestEntity = Vector3(entityPosition.x, entityPosition.y, entityPosition.z);
				std::cout << closestDistance << std::endl;
			}
		}

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				Vector3 delta = (entityPosition - player.position);
				float yaw = atan2f(delta.y, delta.x) * (180 / M_PI);

				float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
				float pitch = atan2f(delta.z, hyp) * (180 / M_PI);

				player.viewDirection.x = yaw + 90;
				player.viewDirection.y = pitch;

				WriteProcessMemory(pHandle, (LPVOID)(viewDirectionAddr), &player.viewDirection, sizeof(Vector2), 0);

			}
		}

		Vector3 delta = (entityPosition - player.position);
		float yaw = atan2f(delta.y, delta.x) * (180 / M_PI);

		float hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
		float pitch = atan2f(delta.z, hyp) * (180 / M_PI);

		player.viewDirection.x = yaw + 90;
		player.viewDirection.y = pitch;

		WriteProcessMemory(pHandle, (LPVOID)(viewDirectionAddr), &player.viewDirection, sizeof(Vector2), 0);
		
		//std::cout << (atan2(player.position.y, player.position.x) - atan2(closestEntity.y, closestEntity.x)) * (180 / M_PI) << std::endl;

		//WriteProcessMemory(pHandle, (LPVOID*)(viewDirectionAddr), &rotationAngle, sizeof(float), 0);

		SDL_RenderPresent(renderer);
		//SDL_Delay(10);
	}

	return 0;
}