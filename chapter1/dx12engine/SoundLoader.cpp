#include "SoundLoader.h"

std::shared_ptr<std::vector<short>> SoundLoader::Load(const std::string& filePath)
{
	// ���ɓǂݍ��ݍς݂̏ꍇ
	if (const auto it = m_dataCache.find(filePath); it != m_dataCache.end())
	{
		return it->second;
	}
	FILE* file = nullptr;
	// �t�@�C���ǂݍ��ݎ��s
	if (fopen_s(&file, filePath.c_str(), "rb") != 0) {
		return nullptr;
	}


	// RIFF�`�����N�̓ǂݍ���
	WAVE::RIFF riff{};
	fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);

	// ID��TYPE�̃`�F�b�N
	if (riff.id != WAVE::fccRIFF || riff.type != WAVE::fccWAVE)
	{
		fclose(file);
		return nullptr;
	}
	WAVE::DWORD buff;
	while (true)
	{
		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
		//DATA�`�����N����
		if (buff == WAVE::fccFmt)
		{
			fseek(file, -static_cast<long>(sizeof(buff)), SEEK_CUR);
			break;
		}
		//���̑��̃`�����N
		long size = 0;
		fread_s(&size, sizeof(size), sizeof(size), 1, file);
		fseek(file, size, SEEK_CUR);
	}

	// FMT�̓ǂݍ���
	WAVE::FMT fmt{};
	fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);

	// FMT�̃`�F�b�N
	if (fmt.id != WAVE::fccFmt)
	{
		fclose(file);
		return nullptr;
	}
	if (fmt.sample != 44100)
	{
		fclose(file);
		return nullptr;
	}
	if (fmt.bit != 16)
	{
		fclose(file);
		return nullptr;
	}
	if (fmt.channel != 1 && fmt.channel != 2)
	{
		fclose(file);
		return nullptr;
	}

	buff = 0;
	while (true)
	{
		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
		//DATA�`�����N����
		if (buff == WAVE::fccData)
		{
			break;
		}
		//���̑��̃`�����N
		long size = 0;
		fread_s(&size, sizeof(size), sizeof(size), 1, file);
		fseek(file, size, SEEK_CUR);
	}
	long size = 0;
	fread_s(&size, sizeof(size), sizeof(size), 1, file);
	auto outData = std::make_shared<std::vector<short>>(size);
	std::vector<short>tmp(outData->size());
	fread_s(tmp.data(), sizeof(short) * tmp.size(), sizeof(short) * tmp.size(), 1, file);

	outData->assign(tmp.begin(), tmp.end());
	m_dataCache[filePath] = outData;
	return outData;
}
