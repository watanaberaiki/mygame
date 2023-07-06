#include "Enemy.h"

void Enemy::Initialize()
{
	//fbx
	enemyfbxmodel= FbxLoader::GetInstance()->LoadModelFromFile("boneTest");
	enemyfbxobj = new FbxObject3D();
	enemyfbxobj->Initialize();
	enemyfbxobj->SetModel(enemyfbxmodel);

	//3dオブジェクト
	enemymodel = Model::LoadFromObj("redcube");
	enemyobj = Object3d::Create();
	enemyobj->SetModel(enemymodel);

}

void Enemy::Update()
{
	Move();
	////fbx
	//enemyfbxobj->SetPosition(position);
	//enemyfbxobj->SetScale(scale);
	//enemyfbxobj->SetRotation(rotation);
	//enemyfbxobj->Update();

	//オブジェクト
	enemyobj->SetPosition(position);
	enemyobj->SetScale(scale);
	enemyobj->SetRotation(rotation);
	enemyobj->Update();

}

void Enemy::Draw(ID3D12GraphicsCommandList* cmdList)
{
	////fbx
	//enemyfbxobj->Draw(cmdList);
	
	//オブジェクト
	enemyobj->Draw();
}

void Enemy::Move()
{
	position.z -= speed;
}
