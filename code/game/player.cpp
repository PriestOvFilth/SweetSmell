#include <map>
#include <editor/imgui/imgui.h>
#include "player.h"
#include "component.h"
#include "scene.h"
#include "engine/camera.h"
#include "engine/sound.h"
#include "level.h"
#include "engine/render.h"
#include "engine/mesh.h"
#include "editor/leveleditor.h"
#include "engine/debug.h"

EditorPlayerEntity::EditorPlayerEntity()
{
	Name = "EditorPlayer";

	CamComp = new CameraComponent();
	AttachComponent(CamComp);
	CamComp->Name = "PlayerCamera";

	bLeftMouseDown = false;
	bRightMouseDown = false;
}

bool EditorPlayerEntity::Init()
{
	CamComp->Init();

	GInput->BindKey(KEY_W, "MoveForward");
	GInput->BindKey(KEY_S, "MoveBackward");
	GInput->BindKey(KEY_A, "MoveLeft");
	GInput->BindKey(KEY_D, "MoveRight");
	GInput->BindKey(KEY_F1, "TogglePause");
	GInput->BindKey(KEY_F2, "ReloadCurrentLevel");
	GInput->BindKey(KEY_F3, "ToggleImGuiDemo");
	GInput->BindKey(KEY_F4, "TogglePostEffects");

	return true;
}

void EditorPlayerEntity::Update(float DeltaTime)
{
	Entity::Update(DeltaTime);

	uint32 MouseMask = SDL_GetMouseState(NULL, NULL);
	bLeftMouseDown = GInput->ME.MousePressed[0] || (MouseMask & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
	bRightMouseDown = GInput->ME.MousePressed[1] || (MouseMask & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
	GInput->ME.MousePressed[0] = GInput->ME.MousePressed[1] = GInput->ME.MousePressed[2] = false;

	SDL_ShowCursor(bRightMouseDown ? 0 : 1);

	HandleKeyboardEvents(GInput->KE);
	HandleMouseEvents(GInput->ME);

	// update listener parameters for 3d audio
	GAudio->ListenerPos = CamComp->WorldPosition;
	GAudio->ListenerTarget = CamComp->Cam->Target;
	GAudio->ListenerUp = CamComp->Cam->Up;
}

void EditorPlayerEntity::HandleKeyboardEvents(KeyboardEvent Event)
{
	uint32 Key = Event.Key;
	bool bPressed = Event.bPressed;
	Camera* Cam = CamComp->Cam;

	if (bPressed)
	{
		if (Key == GInput->KeyBindings["TogglePause"])
		{
			// FIXME: we can't unpause the game right now
			// because this pauses the scene graph update loop
			// which is where we are.
			//WriteLog("Paused game\n");
			//GIsPaused = !GIsPaused;
		}
		if (Key == GInput->KeyBindings["ReloadCurrentLevel"])
		{
			GScene->LoadNewLevel(GScene->CurrentLevel->FileName);
		}
		if (Key == GInput->KeyBindings["ToggleImGuiDemo"])
		{
			GShowImGuiDemo = !GShowImGuiDemo;
		}
		if (Key == GInput->KeyBindings["TogglePostEffects"])
		{
			GDisablePostEffects = !GDisablePostEffects;
		}
	}

	if (Key == GInput->KeyBindings["MoveForward"])
	{
		Cam->Move(MoveState::MOVE_FORWARD, (bPressed && bRightMouseDown));
	}
	if (Key == GInput->KeyBindings["MoveBackward"])
	{
		Cam->Move(MoveState::MOVE_BACKWARD, (bPressed && bRightMouseDown));
	}
	if (Key == GInput->KeyBindings["MoveLeft"])
	{
		Cam->Move(MoveState::MOVE_LEFT, (bPressed && bRightMouseDown));
	}
	if (Key == GInput->KeyBindings["MoveRight"])
	{
		Cam->Move(MoveState::MOVE_RIGHT, (bPressed && bRightMouseDown));
	}
}

void EditorPlayerEntity::HandleMouseEvents(MouseEvent Event)
{
	Camera* Cam = CamComp->Cam;

	if (!GIsPaused)
	{
		if (bRightMouseDown == true)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);

			int32 XRel, YRel;
			SDL_GetRelativeMouseState(&XRel, &YRel);

			if (XRel == 0 || YRel == 0)
			{
				Event.XRel = XRel;
				Event.YRel = YRel;
			}

			Cam->Look(Event.XRel, Event.YRel);
		}
		else
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}

		if (bLeftMouseDown)
		{
			Vector2 MousePos = Vector2(ImGui::GetMousePos().x, ImGui::GetMousePos().y);
			SelectedEntity = MousePick(MousePos);

			if (SelectedEntity != NULL)
			{
				GDebug->AddText(false, 60 * 2, "SelectedEntity: %s\n", SelectedEntity->Name.c_str());
			}
		}
	}
}

Entity* EditorPlayerEntity::MousePick(Vector2 MousePos)
{
	std::map<float, class Entity*> Hits;

	Vector3 RayStart = Vector3::Zero;
	Vector3 RayDir = Vector3::Zero;
	
	CamComp->Cam->ScreenToWorldPoint(MousePos, RayStart, RayDir);

	Vector3 RayEnd = RayStart + RayDir * 1000.f;
	
	/*printf("RayStart: ");
	RayStart.Print();

	printf("RayDir: ");
	RayDir.Print();

	printf("RayEnd: ");
	RayEnd.Print();

	GDebug->AddLine(RayStart, RayEnd, Vector3(1.f, 0.f, 0.f), 1000000);*/

	std::vector<class MeshComponent*> Meshes = GScene->MeshComponents;
	for (int32 i = 0; i < Meshes.size(); i++)
	{
		AABB Box = Meshes[i]->RenderMesh->BoundingBox.Transform(Meshes[i]->Trans.GetWorldTrans());
		float HitDist = RaycastAABB(RayStart, RayEnd, Box);

		if (HitDist == 0.0f || HitDist == INFINITY)
		{
			continue;
		}

		Hits[HitDist] = Meshes[i]->Owner;
	}

	return !Hits.empty() ? Hits.begin()->second : NULL;
}