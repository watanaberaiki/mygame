#include "EnemyBullet.h"

void EnemyBullet::Initialize(DirectXCommon* dxcommon, ResourceManager* resource, XMFLOAT3 velocity)
{
	bulletfbxmodel = resource->LoadFbx("boneTest");
	bulletfbxobj = new FbxObject3D();
	bulletfbxobj->Initialize();
	bulletfbxobj->SetModel(bulletfbxmodel);

	//3d�I�u�W�F�N�g
	bulletmodel = resource->LoadObj("block");
	bulletobj = WireObject::Create();
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

	velocity = XMFLOAT3(velocity.x * speed, velocity.y * speed, velocity.z * speed);
	this->velocity = velocity;
}

void EnemyBullet::Update()
{
	Move();

	////fbx
	//bulletfbxobj->SetPosition(position);
	//bulletfbxobj->SetScale(scale);
	//bulletfbxobj->SetRotation(rotation);
	//bulletfbxobj->Update();

	//���Ԍo�߂Ńf�X
	if (--deathTimer_ <= 0) {
		isdeath = true;
	}

	//�I�u�W�F�N�g
	bulletobj->SetPosition(position);
	bulletobj->SetScale(scale);
	bulletobj->SetRotation(rotation);
	bulletobj->Update();

	//����
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void EnemyBullet::Draw(ID3D12GraphicsCommandList* cmdList)
{
	////fbx
	//bulletfbxobj->Draw(cmdList);

	//�I�u�W�F�N�g
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
