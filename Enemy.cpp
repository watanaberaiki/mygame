#include "Enemy.h"
DirectXCommon* Enemy::dxcommon = nullptr;

void Enemy::Initialize()
{
	//fbx
	enemyfbxmodel= FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	enemyfbxobj = new FbxObject3D();
	enemyfbxobj->Initialize();
	enemyfbxobj->SetModel(enemyfbxmodel);

	//3d�I�u�W�F�N�g
	enemymodel = Model::LoadFromObj("redcube");
	enemyobj = Object3d::Create();
	enemyobj->SetModel(enemymodel);

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

void Enemy::Update()
{
	/*Move();*/
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
	collisionBox->SetScale(scale);
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	////fbx
	//enemyfbxobj->Draw(cmdList);
	
	//�I�u�W�F�N�g
	enemyobj->Draw();
}

void Enemy::Move()
{
	position.z -= speed;
}
