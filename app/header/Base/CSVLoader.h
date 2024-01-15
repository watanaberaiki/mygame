//CSVを読み込むクラス

#pragma once
#include "array"
#include "DirectXMath.h"
#include "vector"
#include "string"

namespace RaikiEngine {
	class CSVLoader
	{
	private:	//エイリアス
		//DirectX::を省略
		using XMFLOAT2 = DirectX::XMFLOAT2;
		using XMFLOAT3 = DirectX::XMFLOAT3;
		using XMFLOAT4 = DirectX::XMFLOAT4;
		using XMMATRIX = DirectX::XMMATRIX;

	public:
		//csvファイルを読み込んで変数に代入する
		void LoadCSV(const std::string& fileName);


		//ゲッター
		XMFLOAT3 GetPosition(int num) { return position[num]; };
		XMFLOAT3 GetRotation(int num) { return rotation[num]; };
		XMFLOAT3 GetScale(int num) { return scale[num]; };
		int Getmove(int num) { return move[num]; }
		int GetShotType(int num) { return shotType[num]; }

	private:
		//読み込むオブジェクトの数
		int objectNum = 0;

		std::vector<XMFLOAT3> position;
		std::vector<XMFLOAT3> rotation;
		std::vector<XMFLOAT3> scale;
		std::vector<int> move;
		std::vector<int> shotType;
	};
}
using namespace RaikiEngine;