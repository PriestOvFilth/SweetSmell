#include "common/common.h"
#include "sound.h"
#include "debug.h"

bool AudioSystem::Init()
{
	SL = new Soloud;

	if (SL->init(Soloud::LEFT_HANDED_3D, Soloud::SDL2) > 0)
	{
		return false;
	}

	SL->setGlobalVolume(1.0f);

	return true;
}

void AudioSystem::PlaySound(SoundBase* Snd)
{
	if (Snd->bIsLoaded)
	{
		Sounds.push_back(Snd);
		Snd->Play();
		GDebug->AddText(true, 60, "Playing sound %d\n", Snd->Handle);
	}
}

void AudioSystem::Update()
{
	// update listener (player) position
	SL->set3dListenerPosition(ListenerPos.x, ListenerPos.y, ListenerPos.z);
	SL->set3dListenerAt(ListenerTarget.x, ListenerTarget.y, ListenerTarget.z);
	SL->set3dListenerUp(ListenerUp.x, ListenerUp.y, ListenerUp.z);

	for (uint32 i = 0; i < Sounds.size(); i++)
	{
		// update sound positions
		if (Sounds[i]->bIs3D)
		{
			SL->set3dSourcePosition(Sounds[i]->Handle, Sounds[i]->Position.x, Sounds[i]->Position.y, Sounds[i]->Position.z);
			SL->set3dSourceVelocity(Sounds[i]->Handle, Sounds[i]->Velocity.x, Sounds[i]->Velocity.y, Sounds[i]->Velocity.z);
			SL->set3dSourceAttenuation(Sounds[i]->Handle, AudioSource::LINEAR_DISTANCE, 1.0f);
		}

		float StreamTime = (float)SL->getStreamTime(Sounds[i]->Handle);
		if (!Sounds[i]->bLooping && (StreamTime == 0.0f))
		{
			Sounds[i]->bShouldBeRemoved = true;
		}
	}

	// remove any non-looping sounds that have stopped playing from the vector
	for (auto It = Sounds.begin(); It != Sounds.end();)
	{
		SoundBase* Snd = dynamic_cast<SoundBase*>(*It._Ptr);
		if (Snd)
		{
			if (Snd->bShouldBeRemoved)
			{
				It = Sounds.erase(It);
				GDebug->AddText(true, 60, "Removed sound %d\n", Snd->Handle);
			}
			else
			{
				++It;
			}
		}
		else
		{
			++It;
		}
	}

	SL->update3dAudio();
}

void AudioSystem::Destroy()
{
	SL->stopAll();

	Sounds.clear();

	SL->deinit();
	SAFE_DELETE(SL);
}

void SoundWave::Play()
{
	Wave.setLooping(bLooping);

	if (!bIs3D)
	{
		Handle = GAudio->SL->play(Wave);
		GAudio->SL->setVolume(Handle, Volume);
		GAudio->SL->setPan(Handle, Panning);
	}
	else
	{
		Wave.set3dMinMaxDistance(1.0f, MaxDistance);
		Handle = GAudio->SL->play3d(Wave, Position.x, Position.y, Position.z, 0.0f, 0.0f, 0.0f, Volume);
	}
}

void SoundWave::Preload(const char* FileName)
{
	int Err = Wave.load(FileName);
	bIsLoaded = (Err != 0) ? false : true;

	if (!bIsLoaded)
	{
		WriteLog("Could not preload sound %s: %s\n", FileName, GAudio->SL->getErrorString(Err));
		return;
	}

	Length = (float)Wave.getLength();
}

void SoundStream::Play()
{
	int32 Err = Stream.load(Filename);
	bIsLoaded = (Err != 0) ? false : true;
	
	if (!bIsLoaded)
	{
		WriteLog("Unable to play streamed sound %s: %s\n", Filename, GAudio->SL->getErrorString(Err));
		return;
	}

	Length = (float)Stream.getLength();

	Stream.setLooping(bLooping);

	if (!bIs3D)
	{
		Handle = GAudio->SL->play(Stream);
		GAudio->SL->setVolume(Handle, Volume);
		GAudio->SL->setPan(Handle, Panning);
	}
	else
	{
		Stream.set3dMinMaxDistance(1.0f, MaxDistance);
		Handle = GAudio->SL->play3d(Stream, Position.x, Position.y, Position.z, 0.0f, 0.0f, 0.0f, Volume);
	}
}