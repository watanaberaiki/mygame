#include "PlayerBullet.h"

void PlayerBullet::Initialize(DirectXCommon* dxcommon)
{
	bulletfbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	bulletfbxobj = new FbxObject3D();
	bulletfbxobj->Initialize();
	bulletfbxobj->SetModel(bulletfbxmodel);

	//3d�I�u�W�F�N�g
	bulletmodel = Model::LoadFromObj("blackcube");
	bulletobj = Object3d::Create();
	bulletobj->SetModel(bulletmodel);


	//�����蔻��L���[�u���f��
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->SetPosition(position);
	collisionBox->SetScale(scale);
	collisionBox->SetRotation(rotation);
	collisionBox->Update();

}

void PlayerBullet::Update()
{
	Move();
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

	//����
	collisionBox->SetPosition(position);
	collisionBox->SetScale(scale);
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
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
		OnCollision();
	}
}

void PlayerBullet::OnCollision()
{
	isdeath = true;
}
