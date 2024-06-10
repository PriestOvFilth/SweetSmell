#pragma once

#include <vector>

#include <soloud.h>
#include <soloud_wav.h>
#include <soloud_wavstream.h>

#include "common/common.h"
#include "common/math3d.h"

using namespace SoLoud;

class SoundBase
{
public:

	virtual void Play() {}

	int32 Handle;

	bool bIs3D;
	bool bLooping;
	bool bIsLoaded;

	// this is only used when this sound is in AudioSystem's Sounds vector
	bool bShouldBeRemoved;

	float Panning;

	Vector3 Position;
	Vector3 Velocity;
	float MaxDistance;

	float Length;

	float Volume;
};

class SoundWave : public SoundBase
{
public:

	void Preload(const char* FileName);

	virtual void Play() override;

	SoLoud::Wav Wave;
};

class SoundStream : public SoundBase
{
public:

	virtual void Play() override;

	SoLoud::WavStream Stream;

	const char* Filename;
};

class AudioSystem
{
public:

	bool Init();

	void PlaySound(class SoundBase* Snd);

	void Update();

	void Destroy();

	Soloud* SL;

	std::vector<class SoundBase*> Sounds;

	Vector3 ListenerPos;
	Vector3 ListenerTarget;
	Vector3 ListenerUp;
};