#include "soundclass.h"

SoundClass::SoundClass()
{
	m_DirectSound = 0;
	m_primaryBuffer = 0;
	m_Volume = DSBVOLUME_MAX;
	m_listener = 0;
	m_secondaryBuffer1 = 0;
	m_secondary3DBuffer1 = 0;
	b_is3D = false;
	m_Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_PositionSound = 0;
}


SoundClass::SoundClass(const SoundClass& other)
{
}

SoundClass::~SoundClass()
{
}


bool SoundClass::Initialize(HWND hwnd, char* soundFilename)
{
	bool result;

	// Initialize direct sound and the primary sound buffer.
	result = InitializeDirectSound(hwnd);
	if (!result) {
		return false;
	}

	// Load a wave audio file onto a secondary buffer.
	result = Load(soundFilename);
	if (!result) {
		return false;
	}

	return true;
}

void SoundClass::Shutdown()
{
	// Release the secondary buffer.
	ShutdownWaveFile(&m_secondaryBuffer1, &m_secondary3DBuffer1);

	// Shutdown the Direct Sound API.
	ShutdownDirectSound();
}

bool SoundClass::InitializeDirectSound(HWND hwnd)
{
	HRESULT result;
	DSBUFFERDESC bufferDesc;
	WAVEFORMATEX waveFormat;


	// Initialize the direct sound interface pointer for the default sound device.
	result = DirectSoundCreate8(NULL, &m_DirectSound, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified.
	result = m_DirectSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY);
	if (FAILED(result)) {
		return false;
	}

	// Setup the primary buffer description.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D;
	bufferDesc.dwBufferBytes = 0;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = NULL;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &m_primaryBuffer, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format).
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified.
	result = m_primaryBuffer->SetFormat(&waveFormat);
	if (FAILED(result)) {
		return false;
	}

	// Obtain a listener interface.
	result = m_primaryBuffer->QueryInterface(IID_IDirectSound3DListener8, (LPVOID*)&m_listener);
	if (FAILED(result)) {
		return false;
	}

	// Set the initial position of the listener to be in the middle of the scene.
	m_listener->SetPosition(m_Position.x, m_Position.y, m_Position.z, DS3D_IMMEDIATE);
	m_listener->SetRolloffFactor(0.075f, DS3D_IMMEDIATE);

	return true;
}

void SoundClass::ShutdownDirectSound()
{
	// Release the listener interface.
	if (m_listener) {
		m_listener->Release();
		m_listener = 0;
	}

	// Release the primary sound buffer pointer.
	if (m_primaryBuffer) {
		m_primaryBuffer->Release();
		m_primaryBuffer = 0;
	}

	// Release the direct sound interface pointer.
	if (m_DirectSound) {
		m_DirectSound->Release();
		m_DirectSound = 0;
	}
}

bool SoundClass::Load(char* filename)
{
	std::string string(filename);

	if (string.rfind(".wav") != std::string::npos) {
		return LoadWaveFile(filename, &m_secondaryBuffer1, &m_secondary3DBuffer1);
	}
	
	return false;
}

bool SoundClass::LoadWaveFile(char* filename, IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	int error;
	FILE* filePtr;
	unsigned int count;
	WaveHeaderType waveFileHeader;
	WAVEFORMATEX waveFormat;
	DSBUFFERDESC bufferDesc;
	HRESULT result;
	IDirectSoundBuffer* tempBuffer;
	unsigned char* waveData;
	unsigned char* bufferPtr;
	unsigned long bufferSize;

	// Open the wave file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0) {
		return false;
	}

	// Read in the wave file header.
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1) {
		return false;
	}

	// Check that the chunk ID is the RIFF format.
	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F')) {
		return false;
	}

	// Check that the file format is the WAVE format.
	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E')) {
		return false;
	}

	// Check that the sub chunk ID is the fmt format.
	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' ')) {
		return false;
	}

	// Check that the audio format is WAVE_FORMAT_PCM.
	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM) {
		return false;
	}

	// Check that the wave file was recorded in stereo format.
	if (b_is3D && waveFileHeader.numChannels != 1) {
		return false;
	}
	if (!b_is3D && waveFileHeader.numChannels != 2) {
		//return false;
	}

	// Check that the wave file was recorded at a sample rate of 44.1 KHz.
	if (waveFileHeader.sampleRate != 44100) {
		return false;
	}

	// Ensure that the wave file was recorded in 16 bit format.
	if (waveFileHeader.bitsPerSample != 16) {
		return false;
	}

	// Check for the data chunk header.
	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a')) {
		//return false;
	}

	// Set the wave format of secondary buffer that this wave file will be loaded onto.
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	if (b_is3D) {
		waveFormat.nChannels = 1;
	} else {
		waveFormat.nChannels = 2;
	}
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto.
	bufferDesc.dwSize = sizeof(DSBUFFERDESC);
	if (b_is3D) {
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME | DSBCAPS_CTRL3D | DSBCAPS_MUTE3DATMAXDISTANCE;
	}
	else {
		bufferDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	}
	bufferDesc.dwBufferBytes = waveFileHeader.dataSize;
	bufferDesc.dwReserved = 0;
	bufferDesc.lpwfxFormat = &waveFormat;
	bufferDesc.guid3DAlgorithm = GUID_NULL;

	// Create a temporary sound buffer with the specific buffer settings.
	result = m_DirectSound->CreateSoundBuffer(&bufferDesc, &tempBuffer, NULL);
	if (FAILED(result)) {
		return false;
	}

	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	result = tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer);
	if (FAILED(result)) {
		return false;
	}

	// Release the temporary buffer.
	tempBuffer->Release();
	tempBuffer = 0;

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Create a temporary buffer to hold the wave file data.
	waveData = new unsigned char[waveFileHeader.dataSize];
	if (!waveData) {
		return false;
	}

	// Read in the wave file data into the newly created buffer.
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);
	if (count != waveFileHeader.dataSize) {
		return false;
	}

	// Close the file once done reading.
	error = fclose(filePtr);
	if (error != 0) {
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	result = (*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0);
	if (FAILED(result)) {
		return false;
	}

	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	result = (*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0);
	if (FAILED(result)) {
		return false;
	}

	// Release the wave data since it was copied into the secondary buffer.
	delete[] waveData;
	waveData = 0;

	// Get the 3D interface to the secondary sound buffer.
	if (b_is3D) {
		result = (*secondaryBuffer)->QueryInterface(IID_IDirectSound3DBuffer8, (void**)&*secondary3DBuffer);
		if (FAILED(result)) {
			return false;
		}
	}

	m_secondary3DBuffer1->SetMaxDistance(100.0f, DS3D_IMMEDIATE);

	return true;
}

void SoundClass::ShutdownWaveFile(IDirectSoundBuffer8** secondaryBuffer, IDirectSound3DBuffer8** secondary3DBuffer)
{
	// Release the 3D interface to the secondary sound buffer.
	if (*secondary3DBuffer) {
		(*secondary3DBuffer)->Release();
		*secondary3DBuffer = 0;
	}

	// Release the secondary sound buffer.
	if (*secondaryBuffer) {
		(*secondaryBuffer)->Release();
		*secondaryBuffer = 0;
	}
}

bool SoundClass::Play()
{
	HRESULT result;

	// Set position at the beginning of the sound buffer.
	result = m_secondaryBuffer1->SetCurrentPosition(m_PositionSound);
	if (FAILED(result)) {
		return false;
	}

	// Set volume of the buffer
	result = m_secondaryBuffer1->SetVolume(m_Volume);
	if (FAILED(result)) {
		return false;
	}

	// Play the contents of the secondary sound buffer.
	result = m_secondaryBuffer1->Play(0, 0, 0);
	if (FAILED(result)) {
		return false;
	}

	return true;
}

void SoundClass::stop()
{
	m_secondaryBuffer1->Stop();
}

void SoundClass::setPosition(D3DXVECTOR3 position)
{
	m_Position = position;
	if (b_is3D) {
		m_secondary3DBuffer1->SetPosition(m_Position.x, m_Position.y, m_Position.z, DS3D_IMMEDIATE);
	}
}

void SoundClass::setPositionListener(D3DXVECTOR3 position)
{
	m_listener->SetPosition(position.x, position.y, position.z, DS3D_IMMEDIATE);
}

bool SoundClass::isPlaying()
{
	DWORD status = 0;

	if (m_secondaryBuffer1->GetStatus(&status) != DS_OK) {
		return false;
	}

	return (status & DSBSTATUS_PLAYING) != 0;
}

void SoundClass::setMaxDistance(float distance)
{
	m_secondary3DBuffer1->SetMaxDistance(distance, DS3D_IMMEDIATE);
}

void SoundClass::SetRolloffFactor(float rollof)
{
	m_listener->SetRolloffFactor(rollof, DS3D_IMMEDIATE);
}