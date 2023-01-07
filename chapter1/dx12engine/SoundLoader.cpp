#include "SoundLoader.h"

#pragma comment ( lib, "dxguid.lib" )
#pragma comment ( lib, "dsound.lib" )
#pragma comment ( lib, "winmm.lib" )

#include <atlstr.h>
#include <windows.h>
#include <dsound.h>
#include <mmsystem.h>

// Wave�t�@�C���I�[�v���֐�
bool SoundLoader::OpenWave(TCHAR* filepath, WAVEFORMATEX& outFmt, std::shared_ptr<std::vector<BYTE>>& ppData) {
	if (filepath == 0)
		return false;

	MMIOINFO mmioInfo = {};

	HMMIO hMmio = mmioOpen(filepath, &mmioInfo, MMIO_READ);
	if (!hMmio)
		return false; // �t�@�C���I�[�v�����s

	MMCKINFO riffChunk;
	riffChunk.fccType = mmioFOURCC('W', 'A', 'V', 'E');
	auto mmRes = mmioDescend(hMmio, &riffChunk, nullptr, MMIO_FINDRIFF);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(hMmio, 0);
		return false;
	}

	// �t�H�[�}�b�g�`�����N����
	MMCKINFO formatChunk;
	formatChunk.ckid = mmioFOURCC('f', 'm', 't', ' ');
	mmRes = mmioDescend(hMmio, &formatChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(hMmio, 0);
		return false;
	}
	DWORD fmsize = formatChunk.cksize;
	DWORD size = mmioRead(hMmio, reinterpret_cast<HPSTR>(&outFmt), fmsize);
	if (size != fmsize) {
		mmioClose(hMmio, 0);
		return false;
	}

	mmioAscend(hMmio, &formatChunk, 0);

	// �f�[�^�`�����N����
	MMCKINFO dataChunk;
	dataChunk.ckid = mmioFOURCC('d', 'a', 't', 'a');
	mmRes = mmioDescend(hMmio, &dataChunk, &riffChunk, MMIO_FINDCHUNK);
	if (mmRes != MMSYSERR_NOERROR) {
		mmioClose(hMmio, 0);
		return false;
	}
	const auto outData = std::make_shared<std::vector<BYTE>>(dataChunk.cksize);
	std::vector<BYTE> tmp(outData->size());
	size = mmioRead(hMmio, reinterpret_cast<HPSTR>(tmp.data()), dataChunk.cksize);
	if (size != dataChunk.cksize) {
		//delete[] * ppData;
		return false;
	}

	// �n���h���N���[�Y
	mmioClose(hMmio, 0);

	outData->assign(tmp.begin(), tmp.end());
	ppData = outData;
	return true;
}
//std::shared_ptr<std::vector<short>> SoundLoader::Load(const std::string& filePath)
//{
//	// ���ɓǂݍ��ݍς݂̏ꍇ
//	if (const auto it = m_dataCache.find(filePath); it != m_dataCache.end())
//	{
//		return it->second;
//	}
//	FILE* file = nullptr;
//	// �t�@�C���ǂݍ��ݎ��s
//	if (fopen_s(&file, filePath.c_str(), "rb") != 0) {
//		return nullptr;
//	}
//
//
//	// RIFF�`�����N�̓ǂݍ���
//	WAVE::RIFF riff{};
//	fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);
//
//	// ID��TYPE�̃`�F�b�N
//	if (riff.id != WAVE::fccRIFF || riff.type != WAVE::fccWAVE)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	WAVE::DWORD buff;
//	while (true)
//	{
//		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
//		//DATA�`�����N����
//		if (buff == WAVE::fccFmt)
//		{
//			fseek(file, -static_cast<long>(sizeof(buff)), SEEK_CUR);
//			break;
//		}
//		//���̑��̃`�����N
//		long size = 0;
//		fread_s(&size, sizeof(size), sizeof(size), 1, file);
//		fseek(file, size, SEEK_CUR);
//	}
//
//	// FMT�̓ǂݍ���
//	WAVE::FMT fmt{};
//	fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);
//
//	// FMT�̃`�F�b�N
//	if (fmt.id != WAVE::fccFmt)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	if (fmt.sample != 44100)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	if (fmt.bit != 16)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	if (fmt.channel != 1 && fmt.channel != 2)
//	{
//		fclose(file);
//		return nullptr;
//	}
//
//	WAVEFORMATEX waveFmt{
//		fmt.type,
//		fmt.channel,
//		fmt.sample,
//		fmt.byte,
//		fmt.block,
//		fmt.bit,
//		0
//	};
//
//	buff = 0;
//	while (true)
//	{
//		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
//		//DATA�`�����N����
//		if (buff == WAVE::fccData)
//		{
//			break;
//		}
//		//���̑��̃`�����N
//		long size = 0;
//		fread_s(&size, sizeof(size), sizeof(size), 1, file);
//		fseek(file, size, SEEK_CUR);
//	}
//	long size = 0;
//	fread_s(&size, sizeof(size), sizeof(size), 1, file);
//	auto outData = std::make_shared<std::vector<short>>(size);
//	std::vector<short>tmp(outData->size());
//	fread_s(tmp.data(), sizeof(short) * tmp.size(), sizeof(short) * tmp.size(), 1, file);
//
//	outData->assign(tmp.begin(), tmp.end());
//	m_dataCache[filePath] = outData;
//	fclose(file);
//	return outData;
//}
//
//std::shared_ptr<std::vector<BYTE>> SoundLoader::LoadB(const std::string& filePath)
//{
//	// ���ɓǂݍ��ݍς݂̏ꍇ
//	//if (const auto it = m_dataCache.find(filePath); it != m_dataCache.end())
//	//{
//	//	return it->second;
//	//}
//	FILE* file = nullptr;
//	// �t�@�C���ǂݍ��ݎ��s
//	if (fopen_s(&file, filePath.c_str(), "rb") != 0) {
//		return nullptr;
//	}
//
//
//	// RIFF�`�����N�̓ǂݍ���
//	WAVE::RIFF riff{};
//	fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);
//
//	// ID��TYPE�̃`�F�b�N
//	if (riff.id != WAVE::fccRIFF || riff.type != WAVE::fccWAVE)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	WAVE::DWORD buff;
//	while (true)
//	{
//		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
//		//DATA�`�����N����
//		if (buff == WAVE::fccFmt)
//		{
//			fseek(file, -static_cast<long>(sizeof(buff)), SEEK_CUR);
//			break;
//		}
//		//���̑��̃`�����N
//		long size = 0;
//		fread_s(&size, sizeof(size), sizeof(size), 1, file);
//		fseek(file, size, SEEK_CUR);
//	}
//
//	// FMT�̓ǂݍ���
//	WAVE::FMT fmt{};
//	fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);
//
//	// FMT�̃`�F�b�N
//	if (fmt.id != WAVE::fccFmt)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	if (fmt.sample != 44100)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	if (fmt.bit != 16)
//	{
//		fclose(file);
//		return nullptr;
//	}
//	if (fmt.channel != 1 && fmt.channel != 2)
//	{
//		fclose(file);
//		return nullptr;
//	}
//
//	WAVEFORMATEX waveFmt{
//		fmt.type,
//		fmt.channel,
//		fmt.sample,
//		fmt.byte,
//		fmt.block,
//		fmt.bit,
//		0
//	};
//
//	buff = 0;
//	while (true)
//	{
//		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
//		//DATA�`�����N����
//		if (buff == WAVE::fccData)
//		{
//			break;
//		}
//		//���̑��̃`�����N
//		long size = 0;
//		fread_s(&size, sizeof(size), sizeof(size), 1, file);
//		fseek(file, size, SEEK_CUR);
//	}
//	long size = 0;
//	fread_s(&size, sizeof(size), sizeof(size), 1, file);
//	auto outData = std::make_shared<std::vector<BYTE>>(size);
//	std::vector<BYTE>tmp(outData->size());
//	fread_s(tmp.data(), sizeof(short) * tmp.size(), sizeof(short) * tmp.size(), 1, file);
//
//	outData->assign(tmp.begin(), tmp.end());
//	fclose(file);
//	return outData;
//}
//
//WAVEFORMATEX SoundLoader::LoadFormat(const std::string& filePath)
//{
//	FILE* file = nullptr;
//	// �t�@�C���ǂݍ��ݎ��s
//	if (fopen_s(&file, filePath.c_str(), "rb") != 0) {
//		return {};
//	}
//
//
//	// RIFF�`�����N�̓ǂݍ���
//	WAVE::RIFF riff{};
//	fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);
//
//	// ID��TYPE�̃`�F�b�N
//	if (riff.id != WAVE::fccRIFF || riff.type != WAVE::fccWAVE)
//	{
//		fclose(file);
//		return {};
//	}
//	WAVE::DWORD buff;
//	while (true)
//	{
//		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
//		//DATA�`�����N����
//		if (buff == WAVE::fccFmt)
//		{
//			fseek(file, -static_cast<long>(sizeof(buff)), SEEK_CUR);
//			break;
//		}
//		//���̑��̃`�����N
//		long size = 0;
//		fread_s(&size, sizeof(size), sizeof(size), 1, file);
//		fseek(file, size, SEEK_CUR);
//	}
//
//	// FMT�̓ǂݍ���
//	WAVE::FMT fmt{};
//	fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);
//
//	// FMT�̃`�F�b�N
//	if (fmt.id != WAVE::fccFmt)
//	{
//		fclose(file);
//		return {};
//	}
//	if (fmt.sample != 44100)
//	{
//		fclose(file);
//		return {};
//	}
//	if (fmt.bit != 16)
//	{
//		fclose(file);
//		return {};
//	}
//	if (fmt.channel != 1 && fmt.channel != 2)
//	{
//		fclose(file);
//		return {};
//	}
//	WAVEFORMATEX waveFmt{
//		fmt.type,
//		fmt.channel,
//		fmt.sample,
//		fmt.byte,
//		fmt.block,
//		fmt.bit,
//		0
//	};
//	fclose(file);
//	return waveFmt;
//}
