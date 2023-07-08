#include "Player.h"
Input* Player::input = nullptr;
DirectXCommon* Player::dxcommon = nullptr;

void Player::Initialize()
{
	//fbx
	playerfbxmodel = FbxLoader::GetInstance()->LoadModelFromFile("bonetest");
	playerfbxobj = new FbxObject3D();
	playerfbxobj->Initialize();
	playerfbxobj->SetModel(playerfbxmodel);

	//�e
	for (int i = 0; i < bulletsize; i++) {
		std::unique_ptr<PlayerBullet>newObject = std::make_unique<PlayerBullet>();
		newObject->Initialize();
		bullets.push_back(std::move(newObject));
	}

	//3d�I�u�W�F�N�g
	playermodel= Model::LoadFromObj("block");
	playerobj = Object3d::Create();
	playerobj->SetModel(playermodel);

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

void Player::Update()
{
	if (input->TriggerKey(DIK_SPACE)) {
		Fire();
	}

	Move();
	////fbx
	//playerfbxobj->SetPosition(position);
	//playerfbxobj->SetScale(scale);
	//playerfbxobj->SetRotation(rotation);
	//playerfbxobj->Update();
	

	//�I�u�W�F�N�g
	playerobj->SetPosition(position);
	playerobj->SetScale(scale);
	playerobj->SetRotation(rotation);
	playerobj->Update();

	//����
	collisionBox->SetPosition(position);
	collisionBox->SetScale(scale);
	collisionBox->SetRotation(rotation);
	collisionBox->Update();

	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		if (bullet->GetIsFIre() == false) 
		{
			bullet->SetPosition(position);
		}
		bullet->Update();
	}
}

void Player::Draw(ID3D12GraphicsCommandList* cmdList)
{
	////fbx
	//playerfbxobj->Draw(cmdList);

	//�I�u�W�F�N�g
	playerobj->Draw();

	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		if (bullet->GetIsFIre())
		{
			bullet->Draw(cmdList);
		}
	}

}

void Player::Move()
{
	//�L�[�{�[�h�ł̈ړ�
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S)) {
		if (input->PushKey(DIK_W)) {
			position.y += 0.1f;
		}
		else if (input->PushKey(DIK_S)) {
			position.y -= 0.1f;
		}
	}

	if (input->PushKey(DIK_A) || input->PushKey(DIK_D)) {
		if (input->PushKey(DIK_A)) {
			position.x -= 0.1f;
		}
		else if (input->PushKey(DIK_D)) {
			position.x += 0.1f;
		}
	}
}

void Player::Fire()
{
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		if (bullet->GetIsFIre()==false) {
			bullet->SetIsFire(true);
			break;
		}
	}
}
