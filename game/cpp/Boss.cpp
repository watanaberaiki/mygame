#include "Boss.h"
DirectXCommon* Boss::dxcommon = nullptr;
Player* Boss::player = nullptr;

void Boss::Initialize()
{
	resource = ResourceManager::Getinstance();

	//fbx
	bossFbxModel = resource->LoadFbx("boneTest");
	bossFbxObj = new FbxObject3D();
	bossFbxObj->Initialize();
	bossFbxObj->SetModel(bossFbxModel);

	//3dオブジェクト
	bossModel = resource->LoadObj("redcube");
	bossObj = WireObject::Create();
	bossObj->SetModel(bossModel);

	//当たり判定キューブモデル
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->Update();
}

void Boss::Update()
{
	//デスフラグの立った球を削除
	bullets.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	//弾の発射
	time++;
	if (time >= MaxTime) {
		Fire();
		time = 0;
	}
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Update();
	}

	////fbx
	//enemyfbxobj->SetPosition(position);
	//enemyfbxobj->SetScale(scale);
	//enemyfbxobj->SetRotation(rotation);
	//enemyfbxobj->Update();


	//オブジェクト
	bossObj->SetPosition(position);
	bossObj->SetScale(scale);
	bossObj->SetRotation(rotation);
	bossObj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();

	if (life <= 0) {
		isdead = true;
	}

}

void Boss::Draw()
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////オブジェクト
	bossObj->Draw();

	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Draw();
	}
}

void Boss::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->DebugDraw(cmdList);
	}

	collisionBox->Draw(cmdList);
}

void Boss::Move()
{
}

void Boss::Fire()
{
	velocityVec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
	velocityVec.normalize();

	velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
	std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
	newObject->Initialize(dxcommon, resource, velocity);
	newObject->SetPosition(position);
	bullets.push_back(std::move(newObject));
}

void Boss::OnCollision()
{
	life--;
}

void Boss::Reset()
{
	life = 10;
	isdead = false;
}
