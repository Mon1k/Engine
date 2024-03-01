#ifndef _SOUNDCLASS_H_
#define _SOUNDCLASS_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <d3dx10math.h>
#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>
#include <iostream>

#include "../resources/AbstractResource.h"

using namespace std;

class SoundClass: public AbstractResource
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	SoundClass();
	SoundClass(const SoundClass&);
	~SoundClass();

	bool Initialize(HWND, char*);
	void Shutdown();
	
	bool Play();
	void stop();
	bool isPlaying();

	void setVolume(int volume) {
		m_Volume = volume;
	}
	void set3D(bool is3D) {
		b_is3D = is3D;
	}

	void setPosition(D3DXVECTOR3);
	void setPositionListener(D3DXVECTOR3);
	void setMaxDistance(float);
	void SetRolloffFactor(float);

private:
	bool InitializeDirectSound(HWND);
	void ShutdownDirectSound();

	bool Load(char*);
	bool LoadWaveFile(char*, IDirectSoundBuffer8**, IDirectSound3DBuffer8**);
	void ShutdownWaveFile(IDirectSoundBuffer8**, IDirectSound3DBuffer8**);

private:
	IDirectSound8* m_DirectSound;
	IDirectSoundBuffer* m_primaryBuffer;

	IDirectSound3DListener8* m_listener;
	IDirectSoundBuffer8* m_secondaryBuffer1;
	IDirectSound3DBuffer8* m_secondary3DBuffer1;

	int m_Volume;
	bool b_is3D;
	D3DXVECTOR3 m_Position;
	unsigned long m_PositionSound;
};

#endif