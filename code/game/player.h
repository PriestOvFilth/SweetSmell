#pragma once

#include "common/common.h"
#include "common/math3d.h"
#include "input.h"
#include "entity.h"

class CameraComponent;

class PlayerEntity : public Entity
{
public:

	PlayerEntity() {}

	virtual bool Init() { return true; }
	virtual void Update(float DeltaTime) {}

	virtual void HandleKeyboardEvents(KeyboardEvent Event) {}
	virtual void HandleMouseEvents(MouseEvent Event) {}
};

class EditorPlayerEntity : public PlayerEntity
{
public:

	EditorPlayerEntity();

	virtual bool Init();
	virtual void Update(float DeltaTime);

	virtual void HandleKeyboardEvents(KeyboardEvent Event);
	virtual void HandleMouseEvents(MouseEvent Event);

	class Entity* MousePick(Vector2 MousePos);

	class Entity* SelectedEntity;

	class CameraComponent* CamComp;
	bool bRightMouseDown;
	bool bLeftMouseDown;
};