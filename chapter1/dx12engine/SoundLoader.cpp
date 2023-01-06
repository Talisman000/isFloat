#include "SoundLoader.h"

std::shared_ptr<std::vector<short>> SoundLoader::Load(const std::string& filePath)
{
	// 既に読み込み済みの場合
	if (const auto it = m_dataCache.find(filePath); it != m_dataCache.end())
	{
		return it->second;
	}
	FILE* file = nullptr;
	// ファイル読み込み失敗
	if (fopen_s(&file, filePath.c_str(), "rb") != 0) {
		return nullptr;
	}


	// RIFFチャンクの読み込み
	WAVE::RIFF riff{};
	fread_s(&riff, sizeof(riff), sizeof(riff), 1, file);

	// IDとTYPEのチェック
	if (riff.id != WAVE::fccRIFF || riff.type != WAVE::fccWAVE)
	{
		fclose(file);
		return nullptr;
	}
	WAVE::DWORD buff;
	while (true)
	{
		fread_s(&buff, sizeof(buff), sizeof(buff), 1, file);
		//DATAチャンク発見
		if (buff == WAVE::fccFmt)
		{
			fseek(file, -static_cast<long>(sizeof(buff)), SEEK_CUR);
			break;
		}
		//その他のチャンク
		long size = 0;
		fread_s(&size, sizeof(size), sizeof(size), 1, file);
		fseek(file, size, SEEK_CUR);
	}

	// FMTの読み込み
	WAVE::FMT fmt{};
	fread_s(&fmt, sizeof(fmt), sizeof(fmt), 1, file);

	// FMTのチェック
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
		//DATAチャンク発見
		if (buff == WAVE::fccData)
		{
			break;
		}
		//その他のチャンク
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
