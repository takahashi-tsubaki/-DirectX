#pragma once
#include "Core/ErrorException.h"
///リンクの設定
#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

class AudioData
{
public:
	std::string filePass;
	IMFSourceReader* pMFSourceReader = nullptr;
	WAVEFORMATEX* waveFormat = nullptr;
	std::vector<BYTE> mediaData;
	char* pBuffer = nullptr;
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	float volume = 1.0f;
	bool loop = false;
	bool playTrigger = false;
	char PADDING2[2]{};
	AudioData(std::string FilePass);
	void Unload();
};

class XAudio2VoiceCallback : public IXAudio2VoiceCallback
{
public:

	virtual ~XAudio2VoiceCallback()
	{
	};

	//ボイス処理パスの開始時
	STDMETHOD_(void, OnVoiceProcessingPassStart)(THIS_ UINT32 BytesRequired)
	{
		static_cast<void>(BytesRequired);
	};

	//ボイス処理パスの終了時
	STDMETHOD_(void, OnVoiceProcessingPassEnd) (THIS)
	{
	};

	//バッファストリームの再生が終了したとき
	STDMETHOD_(void, OnStreamEnd) (THIS)
	{
	};

	//バッファの使用開始時
	STDMETHOD_(void, OnBufferStart) (THIS_ void* pBufferContext)
	{
		static_cast<void>(pBufferContext);

	};

	//バッファの末尾に達した時
	STDMETHOD_(void, OnBufferEnd) (THIS_ void* pBufferContext)
	{
		static_cast<void>(pBufferContext);
	};

	//再生がループ位置に達した時
	STDMETHOD_(void, OnLoopEnd) (THIS_ void* pBufferContext)
	{
		static_cast<void>(pBufferContext);
	};

	//ボイスの実行エラー時
	STDMETHOD_(void, OnVoiceError) (THIS_ void* pBufferContext, HRESULT Error)
	{
		static_cast<void>(pBufferContext);
		static_cast<void>(Error);
	};
};

struct PlayAudioArray
{
	std::vector<uint32_t>handles;
	int32_t nowIdx = 0;
	char PADDING[4]{};
	PlayAudioArray(const std::vector<uint32_t>& Handles);
};

class Audio
{
private:
	static Audio* instance;
	Microsoft::WRL::ComPtr<IXAudio2>xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;
	XAudio2VoiceCallback voiceCallback;
	std::list<AudioData>audios;
	std::vector<PlayAudioArray>playHandleArray;

public:

	static Audio* GetInstance();
	void Destroy();

	void Initialize();
	void Update();
	bool NowPlay(const uint32_t& handle);
	void ChangeVolume(const uint32_t& handle, float volume);
	float GetVolume(const uint32_t& handle);
	uint32_t LoadAudio(std::string fileName, const float& volume = 1.0f);
	int32_t PlayWave(const uint32_t& handle, bool loopFlag = false);
	int32_t PlayWaveArray(const std::vector<uint32_t>& handles);
	void StopWave(const uint32_t& handle);

private:
	Audio();
	~Audio();

	//コピーコンストラクタ・代入演算子削除
	Audio& operator=(const Audio&) = delete;
	Audio(const Audio&) = delete;
};

