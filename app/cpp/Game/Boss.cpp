#include "Boss.h"
DirectXCommon* Boss::dxcommon = nullptr;
Player* Boss::player = nullptr;

void Boss::Initialize()
{
	resource = ResourceManager::Getinstance();

	//fbx
	bossfbxmodel = resource->LoadFbx("boneTest");
	bossfbxobj = new FbxObject3D();
	bossfbxobj->Initialize();
	bossfbxobj->SetModel(bossfbxmodel);

	//3dオブジェクト
	bossmodel = resource->LoadObj("redcube");
	bossobj = WireObject::Create();
	bossobj->SetModel(bossmodel);

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
	bossobj->SetPosition(position);
	bossobj->SetScale(scale);
	bossobj->SetRotation(rotation);
	bossobj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();

	if (life <= 0) {
		isdead = true;
	}

}

void Boss::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////オブジェクト
	bossobj->Draw();

	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Draw(cmdList);
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
	velocityvec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
	velocityvec.normalize();

	velocity = XMFLOAT3(velocityvec.x, velocityvec.y, velocityvec.z);
	std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
	newObject->Initialize(dxcommon, resource, velocity);
	newObject->SetPosition(position);
	bullets.push_back(std::move(newObject));
}

void Boss::OnCollision()
{
	life--;
}
