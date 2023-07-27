#include "PlayerBullet.h"

void PlayerBullet::Initialize(DirectXCommon* dxcommon, ResourceManager* resource)
{

	bulletfbxmodel = resource->LoadFbx("boneTest");
	bulletfbxobj = new FbxObject3D();
	bulletfbxobj->Initialize();
	bulletfbxobj->SetModel(bulletfbxmodel);

	//3dオブジェクト
	bulletmodel = resource->LoadObj("blackcube");
	bulletobj = Object3d::Create();
	bulletobj->SetModel(bulletmodel);


	//当たり判定キューブモデル
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

	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isdeath = true;
	}

	//オブジェクト
	bulletobj->SetPosition(position);
	bulletobj->SetScale(scale);
	bulletobj->SetRotation(rotation);
	bulletobj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void PlayerBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	////fbx
	//bulletfbxobj->Draw(cmdList);

	//オブジェクト
	bulletobj->Draw();

}

void PlayerBullet::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	collisionBox->Draw(cmdList);
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
