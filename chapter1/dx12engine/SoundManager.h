#pragma once

#pragma comment(lib, "OpenAL32.lib")
#include <al.h>
#include <alc.h>
#include <map>
#include <math.h>
#include <memory>
#include <stdlib.h>
#include <string>
#include <vector>


class SoundManager
{
	ALCdevice* m_device = nullptr;
	ALCcontext* m_context = nullptr;
	ALuint buffers[10];
	ALuint sources[10];
	std::map<std::string, int> m_data = {};
public:
	SoundManager();
	~SoundManager();
	void Register(std::string name, int index, std::string filePath);
	void Play(std::string name, bool replay = true, bool isLoop = false);
	void Stop(std::string name);
};

