
#include "Windows.h"
#include "SoundManager.h"
#include "SoundLoader.h"

SoundManager::SoundManager()
{
	m_device = alcOpenDevice(nullptr);
	m_context = alcCreateContext(m_device, nullptr);
	alcMakeContextCurrent(m_context);
	alGenBuffers(10, buffers);
	alGenSources(10, sources);

}

SoundManager::~SoundManager()
{
	for (int i = 0; i < 10; i++)
	{
		alSourcei(sources[i], AL_BUFFER, NULL);
	}
	alDeleteSources(10, sources);
	alDeleteBuffers(10, buffers);
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(m_context);
	// alcCloseDeviceをするとなぜかスタックする
	//ALCboolean  a = alcCloseDevice(m_device);
	//if (a == ALC_FALSE)
	//{
	//	OutputDebugString(L"[OpenAL]close failed\n");
	//}
	//else
	//{
	//	OutputDebugString(L"[OpenAL]close succeeded\n");
	//}
}


void SoundManager::Register(std::string name, int index, std::string filePath)
{
	SoundLoader soundLoader{};
	const auto sound = soundLoader.Load(filePath);
	alBufferData(buffers[index], AL_FORMAT_STEREO16, sound->data(), sound->size(), 44100);
	m_data[name] = index;
}

void SoundManager::Play(const std::string name, bool replay, bool isLoop)
{
	const auto index = m_data[name];
	if (!replay)
	{
		ALint state;
		alGetSourcei(sources[index], AL_SOURCE_STATE, &state);
		if (state == AL_PLAYING)
		{
			return;
		}
	}
	alSourcei(sources[index], AL_BUFFER, buffers[index]);
	if (isLoop)
		alSourcei(sources[index], AL_LOOPING, AL_TRUE);
	alSourcePlay(sources[index]);
}
void SoundManager::Stop(const std::string name)
{
	const auto index = m_data[name];
	alSourceStop(sources[index]);
}

