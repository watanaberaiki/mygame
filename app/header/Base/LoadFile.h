//jsonを読み込むクラス

#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>

// レベルデータ
namespace RaikiEngine {
	struct LevelData {

		struct ObjectData {
			// 平行移動
			DirectX::XMVECTOR translation;
			// 回転角
			DirectX::XMVECTOR rotation;
			// スケーリング
			DirectX::XMVECTOR scaling;
			// ファイル名
			std::string fileName;
		};

		// オブジェクト配列
		std::vector<ObjectData> objects;
	};

	// レベルデータのローダー
	class LoadFile {

	public:// 定数
		// デフォルトの読み込みディレクトリ
		static const std::string kDefaultBaseDirectory;
		// ファイル拡張子
		static const std::string kExtension;

	public:// メンバ関数

		/// <summary>
		/// レベルデータファイルの読み込み
		/// </summary>
		/// <param name="fileName">ファイル名</param>
		static LevelData* LoadFileData(const std::string& fileName);
	};
}
using namespace RaikiEngine;
