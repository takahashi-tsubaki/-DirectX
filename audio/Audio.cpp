#include "Audio.h"
#include <cassert>
#include <fstream>

///リンクの設定
#pragma comment(lib,"xaudio2.lib")

void Audio::Initialize(const std::string& directoryPath)
{
	directoryPath_ = directoryPath;

	HRESULT result = S_FALSE;
	IXAudio2MasteringVoice* masterVoice;

	//xAudioエンジンのインスタンス生成
	result = XAudio2Create(&xAudio2_,0,XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));
	//マスターボイスを生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

}

void Audio::LoadWave(const std::string& filename)
{
	//重複読み込みを回避する
	if (soundData_.find(filename) != soundData_.end()) {
		//何もせず抜ける
		return;
	}

	//パスとファイル名を連結してフルパスを得る
	std::string fullpath = directoryPath_ + filename;

	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(fullpath,std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//wavデータ読み込み
	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)& riff, sizeof(riff));
	//ファイルがRIFFかどうかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0){
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0) {
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		assert(0);
	}
	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)& data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0) {
		//読み取り位置をJUNKの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}
	if (strncmp(data.id, "data", 4) != 0) {
		assert(0);
	}
	//Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	//waveファイルを閉じる
	file.close();

	//読み込んだ音声データをreturn
	//returnする為の音声データ
	SoundData soundData = {};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	//サウンドデータを連想配列に格納
	soundData_.insert(std::make_pair(filename,soundData));

}

void Audio::UnLoad(SoundData* soundData)
{
	//バッファのメモリを解放
	delete[] soundData->pBuffer;

	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}

void Audio::PlayWave(const std::string& filename)
{
	HRESULT result;
	
	std::map<std::string, SoundData>::iterator it = soundData_.find(filename);
	//未読み込みの検出
	assert(it != soundData_.end());
	//サウンドデータの参照を取得
	SoundData& soundData = it->second;

	//波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice,&soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer;
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = pSourceVoice->SubmitSourceBuffer(&buf);
	result = pSourceVoice->Start();


}

void Audio::Finalize()
{
	//XAudio2解放
	xAudio2_.Reset();
	//サウンドデータ解放
	std::map<std::string, SoundData>::iterator it = soundData_.begin();
	for (; it != soundData_.end(); it++)
	{
		UnLoad(&it->second);
	}
	soundData_.clear();
}
