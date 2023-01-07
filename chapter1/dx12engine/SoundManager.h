#pragma once

#include <map>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <string>
#include <vector>
#include "ComPtr.h"
#include <xaudio2.h>


class SoundManager
{
	//ALCdevice* m_device = nullptr;
	//ALCcontext* m_context = nullptr;
	//ALuint buffers[10];
	//ALuint sources[10];
	ComPtr<IXAudio2> m_xAudio2;
	IXAudio2MasteringVoice* m_masteringVoice;
	std::shared_ptr<std::vector<BYTE>> datas[10];
	XAUDIO2_BUFFER buffers[10];
	std::map<std::string, int> m_data = {};
	IXAudio2SourceVoice* sources[10];
	bool isPlaying[10];
public:
	SoundManager();
	~SoundManager();
	void Register(std::string name, int index, TCHAR* filePath);
	void Play(std::string name, bool replay = true, bool isLoop = false);
	void Stop(std::string name);
};

