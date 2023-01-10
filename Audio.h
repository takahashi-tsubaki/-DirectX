#pragma once
#include<xaudio2.h>
#include<cstdint>
#include <wrl.h>
#include <map>
#include <string>


class Audio
{
public://構造体 
	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];//チャンク毎のID
		int32_t size;//チャンクのサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;
		char type[4];
	};

	//FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk; //"fmt"
		WAVEFORMATEX fmt; // 波形フォーマット
	};

	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		BYTE* pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;

	};

public:

	//初期化処理
	void Initialize(const std::string& directoryPath = "Resources/");

	//音声読み込み
	void LoadWave(const std::string& filename);

	void PlayWave(const std::string& filename);

	void UnLoad(SoundData* soundData);

	//解放処理
	void Finalize();

	static Audio* GetInstance();

private:
	// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	

	ComPtr<IXAudio2> xAudio2_;
	//サウンドデータの連想配列
	std::map<std::string,SoundData> soundData_;
	//サウンド格納ディレクトリ
	std::string directoryPath_;

};

