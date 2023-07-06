#include "PlayerBullet.h"

void PlayerBullet::Initialize()
{
	bulletfbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	bulletfbxobj = new FbxObject3D();
	bulletfbxobj->Initialize();
	bulletfbxobj->SetModel(bulletfbxmodel);

	//3d�I�u�W�F�N�g
	bulletmodel = Model::LoadFromObj("blackcube");
	bulletobj = Object3d::Create();
	bulletobj->SetModel(bulletmodel);

}

void PlayerBullet::Update()
{
	if (isfire) {
		Move();
	}
	////fbx
	//bulletfbxobj->SetPosition(position);
	//bulletfbxobj->SetScale(scale);
	//bulletfbxobj->SetRotation(rotation);
	//bulletfbxobj->Update();

	//�I�u�W�F�N�g
	bulletobj->SetPosition(position);
	bulletobj->SetScale(scale);
	bulletobj->SetRotation(rotation);
	bulletobj->Update();
}

void PlayerBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	////fbx
	//bulletfbxobj->Draw(cmdList);

	//�I�u�W�F�N�g
	bulletobj->Draw();
}

void PlayerBullet::Move()
{
	position.z += speed;
	if (position.z>=20) {
		isfire = false;
	}
}
