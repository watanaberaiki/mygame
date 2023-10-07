#include "EnemyBullet.h"

void EnemyBullet::Initialize(DirectXCommon* dxcommon, ResourceManager* resource, XMFLOAT3 velocity_)
{
	bulletfbxmodel = resource->LoadFbx("boneTest");
	bulletfbxobj = new FbxObject3D();
	bulletfbxobj->Initialize();
	bulletfbxobj->SetModel(bulletfbxmodel);

	//3dオブジェクト
	bulletmodel = resource->LoadObj("block");
	bulletobj = WireObject::Create();
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

void EnemyBullet::Draw()
{
	////fbx
	//bulletfbxobj->Draw(cmdList);

	//オブジェクト
	bulletobj->Draw();

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
	isdeath = true;
}
