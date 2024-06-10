#include <SDL_syswm.h>
#include "imgui_impl.h"
#include "leveleditor.h"

LevelEditor::LevelEditor()
{
	GUI = new ImGui_Impl();
}

bool LevelEditor::Init(SDL_Window* Window)
{
	GDisablePostEffects = true;
	GUI->Init(Window);
	return true;
}

void LevelEditor::Update(float DeltaTime)
{
	GUI->NewFrame();
	
	if (GShowImGuiDemo)
	{
		ImGui::ShowDemoWindow();
	}
}

void LevelEditor::Render()
{
	ImGui::Render();
}

void LevelEditor::Destroy()
{
	GUI->Destroy();
	SAFE_DELETE(GUI);
}