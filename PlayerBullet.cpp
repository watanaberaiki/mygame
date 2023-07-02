#include "PlayerBullet.h"

void PlayerBullet::Initialize()
{
	bulletmodel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	bulletobj = new FbxObject3D();
	bulletobj->Initialize();
	bulletobj->SetModel(bulletmodel);
}

void PlayerBullet::Update()
{
	if (isfire) {
		Move();
	}
	bulletobj->SetPosition(position);
	bulletobj->SetScale(scale);
	bulletobj->SetRotation(rotation);
	bulletobj->Update();
}

void PlayerBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	bulletobj->Draw(cmdList);
}

void PlayerBullet::Move()
{
	position.z += speed;
	if (position.z>=20) {
		isfire = false;
	}
}
