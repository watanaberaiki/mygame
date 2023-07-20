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


	//3d�I�u�W�F�N�g
	playermodel = Model::LoadFromObj("block");
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
	//�f�X�t���O�̗����������폜
	bullets.remove_if([](std::unique_ptr<PlayerBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	Move();

	Fire();

	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		bullet->Update();
	}
	
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
		bullet->Draw(cmdList);
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
	if (input->TriggerKey(DIK_SPACE)) {
		std::unique_ptr<PlayerBullet>newObject = std::make_unique<PlayerBullet>();
		newObject->Initialize(dxcommon);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}
}
