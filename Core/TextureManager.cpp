#include "TextureManager.h"
#include "DirectXCommon.h"
TextureManager* TextureManager::GetInstance()
{
	static TextureManager instance;
	return &instance;
}

uint32_t TextureManager::Load(const std::string& fileName)
{
	////�e�N�X�`��
	//TexMetadata metaData{};
	//ScratchImage scratchImg{};

	//result = LoadFromWICFile(
	//	L"Resource/mario.jpg",
	//	WIC_FLAGS_NONE,
	//	&metaData, scratchImg
	//);

	//ScratchImage mipChain{};
	//result = GenerateMipMaps(
	//	scratchImg.GetImages(), scratchImg.GetImageCount(), scratchImg.GetMetadata(),
	//	TEX_FILTER_DEFAULT, 0, mipChain
	//);
	//if (SUCCEEDED(result))
	//{
	//	scratchImg = std::move(mipChain);
	//	metaData = scratchImg.GetMetadata();
	//}

	////�ǂݍ��񂾃f�B�t���[�Y�e�N�X�`����SRGB�Ƃ��Ĉ���
	//metaData.format = MakeSRGB(metaData.format);
	return uint32_t();
}
