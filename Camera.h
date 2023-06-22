#pragma once
#include <DirectXTex.h>
#include"Input.h"

using namespace DirectX;

class Camera
{

public:
	void Initialize(Input* input);
	void Update();

	/*static void SetInput(Input* input) { Camera::input = input; }*/

public:
	/// <summary>
	/// 視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetEye() { return eye; }

	/// <summary>
	/// 視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetEye(XMFLOAT3 eye) { this->eye = eye; };

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetTarget() { return target; }

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetTarget(XMFLOAT3 target) { this->target = target; };

	/// <summary>
	/// 注視点座標の取得
	/// </summary>
	/// <returns>座標</returns>
	const XMFLOAT3& GetUp() { return up; }

	/// <summary>
	/// 注視点座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetUp(XMFLOAT3 up) { this->up = up; };

	const XMMATRIX& GetmatView() { return matView; }
	XMMATRIX GetmatViewProjection() { return (matProjection*matView); }

private:
	XMMATRIX matView;
	XMFLOAT3 eye = {};		//視点座標
	XMFLOAT3 target = {};	//注視点座標
	XMFLOAT3 up = {};		//上方向ベクトル
	XMMATRIX matProjection;

	//キーボード
	Input* input;
};

