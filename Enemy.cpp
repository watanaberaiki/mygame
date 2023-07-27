#include "Enemy.h"
DirectXCommon* Enemy::dxcommon = nullptr;

void Enemy::Initialize()
{
	resource=ResourceManager::Getinstance();

	//fbx
	enemyfbxmodel = resource->LoadFbx("boneTest");
	enemyfbxobj = new FbxObject3D();
	enemyfbxobj->Initialize();
	enemyfbxobj->SetModel(enemyfbxmodel);

	//3d�I�u�W�F�N�g
	enemymodel = resource->LoadObj("redcube");
	enemyobj = Object3d::Create();
	enemyobj->SetModel(enemymodel);

	//�����蔻��L���[�u���f��
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->Update();

}

void Enemy::Update()
{
	Move();

	//�f�X�t���O�̗����������폜
	bullets.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	//�e�̔���
	time++;
	if (time >= MaxTime) {
		Fire();
		time = 0;
	}
	//�e
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Update();
	}

	////fbx
	//enemyfbxobj->SetPosition(position);
	//enemyfbxobj->SetScale(scale);
	//enemyfbxobj->SetRotation(rotation);
	//enemyfbxobj->Update();


	//�I�u�W�F�N�g
	enemyobj->SetPosition(position);
	enemyobj->SetScale(scale);
	enemyobj->SetRotation(rotation);
	enemyobj->Update();

	//����
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////�I�u�W�F�N�g
	enemyobj->Draw();

	//�e
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Draw(cmdList);
	}

}

void Enemy::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	//�e
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->DebugDraw(cmdList);
	}

	collisionBox->Draw(cmdList);
}

void Enemy::Move()
{
	position.z -= speed;
}

void Enemy::Fire()
{
	std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
	newObject->Initialize(dxcommon,resource);
	newObject->SetPosition(position);
	bullets.push_back(std::move(newObject));
}

void Enemy::OnCollision()
{

}
