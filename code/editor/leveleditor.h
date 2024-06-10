#include <SDL.h>

class ImGui_Impl;

class LevelEditor
{
public:

	LevelEditor();

	bool Init(SDL_Window* Window);
	void Update(float DeltaTime);
	void Render();
	void Destroy();

	class ImGui_Impl* GUI;
};