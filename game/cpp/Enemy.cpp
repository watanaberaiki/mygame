#include "Enemy.h"
#include <random>

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
	enemyModel[0] = resource->LoadObj("redcube");
	enemyModel[1] = resource->LoadObj("greencube");
	enemyModel[2] = resource->LoadObj("bluecube");
	enemyObj = WireObject::Create();
	enemyObj->SetModel(enemyModel[0]);

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
	for (int i = 0; i < 4; i++) {
		posLineObject[i] = new LineObject();
		posLineObject[i]->Initialize();
		posLineObject[i]->SetModel(posLineModel);
	}
}

void Enemy::Update()
{
	//弾の打ち方で間隔変更
	if (shotType== Target) {
		MaxTime = targetShot;
	}
	else if (shotType == Straight) {
		MaxTime = straightShot;
	}
	else if (shotType==Random) {
		MaxTime = ramdomShot;
	}

	//弾の打ち方でモデル変更
	if (shotType == Target) {
		//赤
		enemyObj->SetModel(enemyModel[0]);
	}
	else if (shotType == Straight) {
		//緑
		enemyObj->SetModel(enemyModel[1]);
	}
	else if (shotType == Random) {
		//青
		enemyObj->SetModel(enemyModel[2]);
	}

	//弾の打ち方で影の色変更
	if (shotType == Target) {
		//赤
		posLineModel->SetImageData(XMFLOAT4(255, 0, 0, 1));
	}
	else if (shotType == Straight) {
		//緑
		posLineModel->SetImageData(XMFLOAT4(0, 255, 0, 1));
	}
	else if (shotType == Random) {
		//青
		posLineModel->SetImageData(XMFLOAT4(0, 0, 255, 1));
	}

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
			spaceX = (float)easeOutQuad(directionMaxTime, 0.0f, endSpaceX - 0.0f, directionTime);
			//ライン
			spaceZ = (float)easeOutQuad(directionMaxTime, 0.0f, endSpaceZ - 0.0f, directionTime);
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

	//プレイヤーの後ろに行ったら消える
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
	//位置を下に表示
	for (int i = 0; i < 4; i++) {
		XMFLOAT3 pos = position;
		XMFLOAT3 startpos = {};
		XMFLOAT3 endpos = {};

		pos.y = -downLinePosY;
		////簡易的な影
		if (i ==0) {
			pos.x += spaceX;
			startpos = { pos.x,pos.y,pos.z- spaceZ };
			endpos = { pos.x,pos.y,pos.z + spaceZ };

		}
		else if (i == 1) {
			pos.x -= spaceX;
			startpos = { pos.x,pos.y,pos.z - spaceZ };
			endpos = { pos.x,pos.y,pos.z + spaceZ };
		}
		else if (i==2) {
			pos.z += spaceZ;
			startpos = { pos.x- spaceX,pos.y,pos.z };
			endpos = { pos.x+ spaceX,pos.y,pos.z};
		}
		else if (i==3) {
			pos.z -= spaceZ;
			startpos = { pos.x - spaceX,pos.y,pos.z };
			endpos = { pos.x + spaceX,pos.y,pos.z };
		}
		posLineObject[i]->SetStartPosition(startpos);
		posLineObject[i]->SetEndPosition(endpos);
		posLineObject[i]->SetModel(posLineModel);
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

	//当たり判定
	//collisionBox->Draw(cmdList);

	for (int i = 0; i < 4; i++) {
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
	//自機狙い
	if (shotType == Shot::Target) {
		velocityVec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}
	//まっすぐ
	else if (shotType==Shot::Straight) {
		velocityVec = { position.x,position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}
	//ランダム
	else if (shotType==Shot::Random) {
		//ランダム
		std::random_device ram_dev;
		std::mt19937 ram(ram_dev());
		XMFLOAT3 pos;\
		pos.x= (ram() % 20  -10.0f)/10;
		pos.y= (ram() % 20 - 10.0f)/10;
		velocityVec = { pos.x - position.x,pos.y - position.y,player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}


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