#include "EnemyBullet.h"

void EnemyBullet::Initialize(DirectXCommon* dxcommon, ResourceManager* resource, XMFLOAT3 velocity_)
{
	bulletFbxModel = resource->LoadFbx("boneTest");
	bulletFbxObj = new FbxObject3D();
	bulletFbxObj->Initialize();
	bulletFbxObj->SetModel(bulletFbxModel);

	//3dオブジェクト
	bulletModel = resource->LoadObj("redcube");
	bulletObj = Object3d::Create();
	bulletObj->SetModel(bulletModel);


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

	velocity_ = XMFLOAT3(velocity_.x * speed, velocity_.y * speed, velocity_.z * speed);
	this->velocity = velocity_;
}

void EnemyBullet::Update()
{
	Move();

	////fbx
	//bulletfbxobj->SetPosition(position);
	//bulletfbxobj->SetScale(scale);
	//bulletfbxobj->SetRotation(rotation);
	//bulletfbxobj->Update();

	//時間経過でデス
	if (--deathTimer_ <= 0) {
		isDeath = true;
	}

	//オブジェクト
	bulletObj->SetPosition(position);
	bulletObj->SetScale(scale);
	bulletObj->SetRotation(rotation);
	bulletObj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void EnemyBullet::Draw()
{
	////fbx
	//bulletfbxobj->Draw(cmdList);

	//オブジェクト
	bulletObj->Draw();

}

void EnemyBullet::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	collisionBox->Draw(cmdList);
}

void EnemyBullet::Move()
{
	position.x += velocity.x;
	position.y += velocity.y;
	position.z += velocity.z;
}

void EnemyBullet::OnCollision()
{
	isDeath = true;
}
