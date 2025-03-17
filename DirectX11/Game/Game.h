#pragma once
class Game
{
public:
	Game();
	~Game();

	void BeginPlay(HWND hwnd);
	void Tick();
	void Render();

private:
	HWND _hwnd;
	uint32 _width = 0;
	uint32 _height = 0;
};

