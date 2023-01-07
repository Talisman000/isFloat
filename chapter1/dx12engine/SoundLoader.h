#pragma once
#include <map>
#include <memory>
#include <string>
#include <vector>

#include <xaudio2.h>

//namespace WAVE
//{
//	typedef unsigned char  BYTE;  // 1バイトデータ用
//	typedef unsigned short WORD;  // 2バイトデータ用
//	typedef unsigned long  DWORD; // 4バイトデータ用
//
//	// FourCC用に型を再定義
//	typedef DWORD FOURCC;
//#define FourCC(c1, c2, c3, c4) (c1 + (c2 << 8) + (c3 << 16) + (c4 << 24))
//	constexpr FOURCC fccRIFF = FourCC('R', 'I', 'F', 'F');
//	constexpr FOURCC fccLIST = FourCC('L', 'I', 'S', 'T');
//	constexpr FOURCC fccWAVE = FourCC('W', 'A', 'V', 'E');
//	constexpr FOURCC fccFmt = FourCC('f', 'm', 't', ' ');
//	constexpr FOURCC fccData = FourCC('d', 'a', 't', 'a');
//
//	struct RIFF
//	{
//		DWORD id;
//		DWORD size;
//		DWORD type;
//	};
//
//	struct FMT
//	{
//		DWORD id;
//		DWORD size;
//		WORD type;
//		WORD channel;
//		DWORD sample;
//		DWORD byte;
//		WORD block;
//		WORD bit;
//	};
//
//	struct DATA
//	{
//		DWORD id;
//		DWORD size;
//	};
//}

// 参考: https://qiita.com/okmonn/items/ab037ef11bf0e2ebbaca
class SoundLoader
{
	inline static std::map<std::string, std::shared_ptr<std::vector<short>>> m_dataCache = {};
public:
	SoundLoader() = default;
	// ファイルを読み込む
	// 規格は16bit, 44100hz, stereo(FLStudioの標準)
	//static std::shared_ptr<std::vector<short>> Load(
	//	const std::string& filePath
	//);
	//static std::shared_ptr<std::vector<BYTE>> LoadB(
	//	const std::string& filePath
	//);
	//static WAVEFORMATEX LoadFormat(const std::string& filePath);
	static bool OpenWave(WCHAR* filepath, WAVEFORMATEX& outFmt, std::shared_ptr<std::vector<BYTE>>& ppData);

};

