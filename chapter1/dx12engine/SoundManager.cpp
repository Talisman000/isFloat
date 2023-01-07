
#include "Windows.h"
#include "SoundManager.h"

#include <stdexcept>
#include <tchar.h>

#include "SoundLoader.h"

SoundManager::SoundManager()
{
	//m_device = alcOpenDevice(nullptr);
	//m_context = alcCreateContext(m_device, nullptr);
	//alcMakeContextCurrent(m_context);
	//alGenBuffers(10, buffers);
	//alGenSources(10, sources);

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	m_xAudio2 = nullptr;
	XAudio2Create(&m_xAudio2, 0);
#if defined(_DEBUG)
	XAUDIO2_DEBUG_CONFIGURATION debug{ 0 };
	debug.TraceMask = XAUDIO2_LOG_ERRORS | XAUDIO2_LOG_WARNINGS;
	debug.BreakMask = XAUDIO2_LOG_ERRORS;
	pXAudio2->SetDebugConfiguration(&debug, 0);
#endif
	m_masteringVoice = nullptr;
	m_xAudio2->CreateMasteringVoice(&m_masteringVoice);

	//const auto sound = SoundLoader::LoadB("Assets/Audio/damage.wav");
	//WAVEFORMATEX fmt;
	//std::shared_ptr<std::vector<BYTE>> pWaveData = nullptr;
	//SoundLoader::OpenWave((TCHAR*)_T("Assets/Audio/bgm.wav"), fmt, pWaveData);

	////const auto fmt = SoundLoader::LoadFormat("Assets/Audio/damage.wav");
	//IXAudio2SourceVoice* pSourceVoice{ nullptr };
	//if (HRESULT hr = m_xAudio2->CreateSourceVoice(&pSourceVoice, &fmt); FAILED(hr))
	//{
	//	throw std::runtime_error("CreateSourceVoice Failed");
	//}
	for (int i = 0; i < 10; i++)
	{
		isPlaying[i] = false;
	}

	//HRESULT hr = pSourceVoice->Start(0);
	//if (FAILED(hr))
	//{
	//	throw std::runtime_error("PlayVoice Failed");
	//}
	//while (true)
	//{
	//	//XAUDIO2_VOICE_STATE state{ 0 };
	//	//pSourceVoice->GetState(&state);
	//	//if (state.BuffersQueued == 0)
	//	//{
	//	//	break;
	//	//}
	//	Sleep(10);
	//}
}

SoundManager::~SoundManager()
{
	//for (int i = 0; i < 10; i++)
	//{
	//	alSourcei(sources[i], AL_BUFFER, NULL);
	//}
	//alDeleteSources(10, sources);
	//alDeleteBuffers(10, buffers);
	//alcMakeContextCurrent(nullptr);
	//alcDestroyContext(m_context);
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
	for (const auto& data : m_data)
	{
		auto index = data.second;
		sources[index]->Stop();
		sources[index]->FlushSourceBuffers();
		sources[index]->DestroyVoice();
	}
	m_masteringVoice->DestroyVoice();
	CoUninitialize();
}


void SoundManager::Register(std::string name, int index, TCHAR* filePath)
{
	//const auto sound = SoundLoader::Load(filePath);
	//alBufferData(buffers[index], AL_FORMAT_STEREO16, sound->data(), sound->size(), 44100);
	//m_data[name] = index;

	WAVEFORMATEX fmt;
	std::shared_ptr<std::vector<BYTE>> pWaveData = nullptr;
	SoundLoader::OpenWave(filePath, fmt, pWaveData);

	//const auto fmt = SoundLoader::LoadFormat("Assets/Audio/damage.wav");
	IXAudio2SourceVoice* pSourceVoice{ nullptr };
	if (HRESULT hr = m_xAudio2->CreateSourceVoice(&pSourceVoice, &fmt); FAILED(hr))
	{
		throw std::runtime_error("CreateSourceVoice Failed");
	}
	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = pWaveData->data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = pWaveData->size();
	//buffer.LoopBegin = 0;
	//buffer.LoopLength = pWaveData->size() / fmt.nBlockAlign;
	//buffer.PlayLength = pWaveData->size() / fmt.nBlockAlign;
	//buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	pSourceVoice->SubmitSourceBuffer(&buffer);

	buffers[index] = buffer;
	sources[index] = pSourceVoice;
	datas[index] = pWaveData;
	m_data[name] = index;
	//HRESULT hr = sources[index]->Start(0);
	//if (FAILED(hr))
	//{
	//	throw std::runtime_error("PlayVoice Failed");
	//}
	Sleep(10);
}

void SoundManager::Play(const std::string name, bool replay, bool isLoop)
{
	const auto index = m_data[name];
	if (!replay)
	{
		if (isPlaying[index]) return;
	}
	XAUDIO2_BUFFER buffer{ 0 };
	buffer.pAudioData = datas[index]->data();
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	buffer.AudioBytes = datas[index]->size();
	if (isLoop)
	{
		buffer.LoopBegin = 0;
		buffer.LoopLength = datas[index]->size() / 4;
		buffer.PlayLength = datas[index]->size() / 4;
		buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
	}
	sources[index]->Stop(0);
	sources[index]->FlushSourceBuffers();
	sources[index]->SubmitSourceBuffer(&buffer);
	sources[index]->Start(0);
	HRESULT hr = sources[0]->Start(0);
	if (FAILED(hr))
	{
		throw std::runtime_error("PlayVoice Failed");
	}
	isPlaying[index] = true;
	//alSourcei(sources[index], AL_BUFFER, buffers[index]);
	//if (isLoop)
	//	alSourcei(sources[index], AL_LOOPING, AL_TRUE);
	//alSourcePlay(sources[index]);
}
void SoundManager::Stop(const std::string name)
{
	const auto index = m_data[name];
	sources[index]->Stop(0);
	sources[index]->FlushSourceBuffers();
	isPlaying[index] = false;
	//alSourceStop(sources[index]);
}

