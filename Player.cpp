#include "Player.h"
Input* Player::input = nullptr;
DirectXCommon* Player::dxcommon = nullptr;

void Player::Initialize()
{
	resource = ResourceManager::Getinstance();
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
	collisionBox->Update();

	//���C��������
	linemodel = new LineModel();
	linemodel->Initialize(dxcommon->GetDevice(), 0.2f, -0.2f);
	linemodel->SetImageData(XMFLOAT4(255, 255, 255, 1));
	for (int i = 0; i < 4; i++) {
		lineobject[i] = new LineObject();
		lineobject[i]->Initialize();
		lineobject[i]->SetModel(linemodel);
	}
	//XMConvertToDegrees
	lineobject[0]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));
	lineobject[2]->SetRotation(XMFLOAT3(0.0f, 0.0f, XMConvertToRadians(90.0f)));

	//���e�B�N���̈ʒu
	frontdepth = 10;
	backdepth = 25;

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
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();

	//���e�B�N��
	frontReticlepos = position;
	frontReticlepos.z += frontdepth;

	backReticlepos = position;
	backReticlepos.z += backdepth;

	for (int i = 0; i < 4; i++) {
		if (i < 2) {
			lineobject[i]->SetPosition(frontReticlepos);
		}
		else {
			lineobject[i]->SetPosition(backReticlepos);
		}
		lineobject[i]->Update();
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
		bullet->Draw(cmdList);
	}


}

void Player::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	//�e
	for (std::unique_ptr<PlayerBullet>& bullet : bullets)
	{
		//bullet->DebugDraw(cmdList);
	}

	//collisionBox->Draw(cmdList);

	for (int i = 0; i < 4; i++) {
		lineobject[i]->Draw(cmdList);
	}
}

void Player::Move()
{
	//�L�[�{�[�h�ł̈ړ�
	if (input->PushKey(DIK_W) || input->PushKey(DIK_S)) {
		if (input->PushKey(DIK_W)) {
			if (position.y < 1.0) {
				position.y += 0.1f;
			}
		}
		else if (input->PushKey(DIK_S)) {
			if (position.y > -1.0) {
				position.y -= 0.1f;
			}
		}
	}

	if (input->PushKey(DIK_A) || input->PushKey(DIK_D)) {
		if (input->PushKey(DIK_A)) {
			if (position.x > -2.0) {
				position.x -= 0.1f;
			}
		}
		else if (input->PushKey(DIK_D)) {
			if (position.x < 2.0) {
				position.x += 0.1f;
			}
		}
	}
}

void Player::Fire()
{
	if (input->TriggerKey(DIK_SPACE)) {
		std::unique_ptr<PlayerBullet>newObject = std::make_unique<PlayerBullet>();
		newObject->Initialize(dxcommon, resource);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}
}

void Player::OnCollision()
{
}
