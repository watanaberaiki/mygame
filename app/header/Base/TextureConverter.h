#pragma once
#include "DirectXCommon.h"
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include <array>
#include<string>
using namespace Microsoft::WRL;
using namespace DirectX;

class TextureConverter
{
public:
	//テクスチャをWICからDDSに変換する
	void ConvertTextureWICToDDS(const std::string& filePath);

private:
	//テクスチャファイルを読み込む
	void LoadWICTextureFromFile(const std::string& filePath);

	//ワイド文字列
	static std::wstring ConvertMultiByteStringToWideString(const std::string& mString);

	//フォルダパスとファイル名を分離する
	void SeparateFilePath(const std::wstring& filePath);

	//DDSテクスチャとしてファイル書き出し
	void SaveDDSTextureToFile();

private:
	//画像の情報
	DirectX::TexMetadata metadata_;
	//画像イメージのコンテナ
	DirectX::ScratchImage scratchImage_;
	//ディレクトリパス
	std::wstring directoryPath_;
	//ファイルパス
	std::wstring fileName_;
	//ファイル拡張子
	std::wstring fileExt_;
};

