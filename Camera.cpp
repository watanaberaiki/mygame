#include "Camera.h"
#include"WinApp.h"
void Camera::Initialize(Input* input) {
	//透視投影変換行列の計算
	matProjection = XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		(float)WinApp::window_width / WinApp::window_height,
		0.1f, 1000.0f
	);
	this->input = input;
}

void Camera::Update() {
	if (input->PushKey(DIK_LEFT) || input->PushKey(DIK_RIGHT)) {
		if (input->PushKey(DIK_LEFT)) {
			eye.x -= 1.0;
			target.x -= 1.0;
		}
		else if (input->PushKey(DIK_RIGHT)) {
			eye.x += 1.0;
			target.x += 1.0;
		}
	}

	if (input->PushKey(DIK_UP) || input->PushKey(DIK_DOWN)) {
		if (input->PushKey(DIK_UP)) {
			eye.y += 1.0;
			target.y += 1.0;
		}
		else if (input->PushKey(DIK_DOWN)) {
			eye.y -= 1.0;
			target.y -= 1.0;
		}
	}
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S)) {
		if (input->PushKey(DIK_W)) {
			eye.z += 1.0;
			target.z += 1.0;
		}
		else if (input->PushKey(DIK_S)) {
			eye.z -= 1.0;
			target.z -= 1.0;
		}
	}
	//ビュー変換行列の計算
	matView = XMMatrixLookAtLH(XMLoadFloat3(&eye), XMLoadFloat3(&target), XMLoadFloat3(&up));
}