#include "Enemy.h"
DirectXCommon* Enemy::dxcommon = nullptr;
Player* Enemy::player = nullptr;

void Enemy::Initialize()
{
	resource = ResourceManager::Getinstance();

	//fbx
	enemyFbxModel = resource->LoadFbx("boneTest");
	enemyFbxObj = new FbxObject3D();
	enemyFbxObj->Initialize();
	enemyFbxObj->SetModel(enemyFbxModel);

	//3dオブジェクト
	enemyModel = resource->LoadObj("redcube");
	enemyObj = WireObject::Create();
	enemyObj->SetModel(enemyModel);

	//当たり判定キューブモデル
	cubeModel = new CubeModel();
	cubeModel->CreateBuffers(dxcommon->GetDevice());
	cubeModel->SetImageData(XMFLOAT4(255, 0, 0, 1));

	collisionBox = new CubeObject3D();
	collisionBox->Initialize();
	collisionBox->SetModel(cubeModel);
	collisionBox->Update();

	//レティクルの位置を横の壁でわかりやすく
	posLineModel = new LineModel();
	posLineModel->Initialize(dxcommon->GetDevice(), 0, 0);
	posLineModel->SetImageData(XMFLOAT4(255, 0, 0, 1));
	for (int i = 0; i < 2; i++) {
		posLineObject[i] = new LineObject();
		posLineObject[i]->Initialize();
		posLineObject[i]->SetModel(posLineModel);
	}

}

void Enemy::Update()
{
	//登場演出
	//プレイヤー
	if (isAppearanceDirection) {
		if (directionMaxTime == directionTime) {
			isAppearanceDirection = false;
		}
		else {
			directionTime++;
			//敵のスケール
			directionScale = (float)easeOutQuad(directionMaxTime, startScale, endScale - startScale, directionTime);

			//ライン
			linePosY = (float)easeOutQuad(directionMaxTime, startLinePosY, endLinePosY - startLinePosY, directionTime);
		}
	}
	scale = XMFLOAT3(directionScale, directionScale, directionScale);
	//演出の入る条件
	if (!isAppearance) {
		if (position.z - player->GetPosition().z <= 35.0f) {
			isAppearance = true;
			isAppearanceDirection = true;
		}
	}

	//動き
	Move();

	if (player->GetPosition().z - position.z >= 10.0f) {
		isdead = true;
	}


	//デスフラグの立った球を削除
	bullets.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	//弾の発射
	time++;
	if (time >= MaxTime) {
		//プレイヤーの裏に行ったら弾を撃たなくなる
		if (player->GetPosition().z < position.z) {
			//距離の差が特定の値を越したら撃ち始める
			if (position.z - player->GetPosition().z <= 30.0f) {
				Fire();
				time = 0;
			}
		}
	}
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Update();
	}

	//位置を横に表示
	for (int i = 0; i < 2; i++) {
		XMFLOAT3 pos = position;
		pos.x = 0;
		pos.y = -0.5;
		if (i == 0) {
			pos.x += widthSpace;
		}
		else if (i==1) {
			pos.x -= widthSpace;
		}
		posLineObject[i]->SetStartPosition(pos);
		posLineObject[i]->SetStartPositionY(pos.y-linePosY);
		posLineObject[i]->SetEndPosition(pos);
		posLineObject[i]->SetEndPositionY(pos.y + linePosY);
		posLineObject[i]->Update();
	}


	////fbx
	//enemyfbxobj->SetPosition(position);
	//enemyfbxobj->SetScale(scale);
	//enemyfbxobj->SetRotation(rotation);
	//enemyfbxobj->Update();


	//オブジェクト
	enemyObj->SetPosition(position);
	enemyObj->SetScale(scale);
	enemyObj->SetRotation(rotation);
	enemyObj->Update();

	//判定
	collisionBox->SetPosition(position);
	collisionBox->SetScale(XMFLOAT3(scale.x * 2, scale.y * 2, scale.z * 2));
	collisionBox->SetRotation(rotation);
	collisionBox->Update();
}

void Enemy::WireDraw()
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////オブジェクト
	enemyObj->Draw();

}

void Enemy::Draw()
{
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->Draw();
	}
}

void Enemy::DebugDraw(ID3D12GraphicsCommandList* cmdList)
{
	//弾
	for (std::unique_ptr<EnemyBullet>& bullet : bullets)
	{
		bullet->DebugDraw(cmdList);
	}

	collisionBox->Draw(cmdList);

	for (int i = 0; i < 2; i++) {
		posLineObject[i]->Draw(cmdList);
	}
}

void Enemy::Move()
{
	switch (type)
	{
		//z方向のみ
	case Move_z:
		//position.z -= speedZ;
		break;
		//xz方向
	case Move_xz:
		//position.z -= speedZ;

		if (plusX) {
			if (position.x < MoveX) {
				position.x += speedX;
			}
			else {
				plusX = false;
			}
		}
		else {
			if (position.x > -MoveX) {
				position.x -= speedX;
			}
			else {
				plusX = true;
			}
		}
		break;
		//yz方向
	case Move_yz:
		//position.z -= speedZ;
		if (plusY) {
			if (position.y < MoveY) {
				position.y += speedY;
			}
			else {
				plusY = false;
			}
		}
		else {
			if (position.y > -MoveY) {
				position.y -= speedY;
			}
			else {
				plusY = true;
			}
		}



		break;
		//xyz方向
	case Move_xyz:
		//position.z -= speedZ;

		if (plusX) {
			if (position.x < MoveX) {
				position.x += speedX;
			}
			else {
				plusX = false;
			}
		}
		else {
			if (position.x > -MoveX) {
				position.x -= speedX;
			}
			else {
				plusX = true;
			}
		}

		if (plusY) {
			if (position.y < MoveY) {
				position.y += speedY;
			}
			else {
				plusY = false;
			}
		}
		else {
			if (position.y > -MoveY) {
				position.y -= speedY;
			}
			else {
				plusY = true;
			}
		}

		break;
	default:
		break;
	}
}

void Enemy::Fire()
{
	velocityVec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
	velocityVec.normalize();

	velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
	std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
	newObject->Initialize(dxcommon, resource, velocity);
	newObject->SetPosition(position);
	bullets.push_back(std::move(newObject));
}

void Enemy::OnCollision()
{
	isdead = true;
	isAppearance = false;
}

double Enemy::easeOutQuad(double time_, double start_, double difference, double totaltime_)
{
	double x = totaltime_ / time_;
	double v = 1 - (1 - x) * (1 - x);
	double ret = difference * v + start_;
	return ret;
}