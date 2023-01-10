#pragma once
#include<xaudio2.h>
#include<cstdint>
#include <wrl.h>
#include <map>
#include <string>


class Audio
{
public://�\���� 
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];//�`�����N����ID
		int32_t size;//�`�����N�̃T�C�Y
	};

	//RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;
		char type[4];
	};

	//FMT�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk; //"fmt"
		WAVEFORMATEX fmt; // �g�`�t�H�[�}�b�g
	};

	//�����f�[�^
	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		BYTE* pBuffer;
		//�o�b�t�@�̃T�C�Y
		unsigned int bufferSize;

	};

public:

	//����������
	void Initialize(const std::string& directoryPath = "Resources/");

	//�����ǂݍ���
	void LoadWave(const std::string& filename);

	void PlayWave(const std::string& filename);

	void UnLoad(SoundData* soundData);

	//�������
	void Finalize();

	static Audio* GetInstance();

private:
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	

	ComPtr<IXAudio2> xAudio2_;
	//�T�E���h�f�[�^�̘A�z�z��
	std::map<std::string,SoundData> soundData_;
	//�T�E���h�i�[�f�B���N�g��
	std::string directoryPath_;

};

