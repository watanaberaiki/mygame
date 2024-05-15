#include "Boss.h"
#include <random>
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

	//レティクルの位置を横の壁でわかりやすく
	posLineModel = new LineModel();
	posLineModel->Initialize(dxcommon->GetDevice(), 0.0f, 0.0f);
	posLineModel->SetImageData(XMFLOAT4(255, 0, 0, 1));
	for (int i = 0; i < 4; i++) {
		posLineObject[i] = new LineObject();
		posLineObject[i]->Initialize();
		posLineObject[i]->SetModel(posLineModel);
	}
	//Z移動先
	front = distanceZ;
	frontBack = distanceZ*2;
	backFront = distanceZ*3;
	back = distanceZ*4;
}

void Boss::Update()
{
	//ランダム
	std::random_device ram_dev;
	std::mt19937 ram(ram_dev());

	//カウント
	int count = 0;

	//デスフラグの立った球を削除
	bullets.remove_if([](std::unique_ptr<EnemyBullet>& bullet) {
		return bullet->GetIsDeath();
		});

	if (!isdead) {

		//弾の発射
		targetShotTime++;
		if (targetShotTime >= targetShotMaxTime) {
			//弾の発射パターンランダム
			count = ram() % TARGETSHOT::targetShot_end;
			//変換
			targetShot = static_cast<TARGETSHOT>(count);
			TargetShot();
			targetShotTime = 0;
		}

		//固定撃ち
		fixedShotTime++;
		if (fixedShotTime >= fixedShotMaxTime) {
			//パターンランダム
			count = ram() % FIXEDSHOT::fixedShot_end;
			//変換
			fixedShot = static_cast<FIXEDSHOT>(count);
			//縦
			FixedShotHeight();
			//横
			FixedShotWidth();
			fixedShotTime = 0;
		}

		//弾更新処理
		for (std::unique_ptr<EnemyBullet>& bullet : bullets)
		{
			bullet->Update();
		}
		//最初の登場用
		if (firstDirection==false) {
			int moveZ = 0;
			moveZ = ram() % 4;
			//前(1)
			if (moveZ == 0) {
				position.z = player->GetPosition().z + front;
			}
			//前の後ろ(2)
			else if (moveZ == 1) {
				position.z = player->GetPosition().z + frontBack;
			}
			//後ろの前(3)
			else if (moveZ == 2) {
				position.z = player->GetPosition().z + backFront;
			}
			//後ろ(4)
			else if (moveZ == 3) {
				position.z = player->GetPosition().z + back;
			}
			isMoveZ = true;
			isReduction = false;
			firstDirection = true;
		}
		//動き
		if (isMoveZ == false) {
			moveTime++;
			moveZTime++;
			//Z軸の移動タイマー
			if (moveZTime >= moveZMaxTime) {
				isMoveZ = true;
				isReduction = true;
			}
			//XY軸の移動
			if (moveTime > moveMaxTime) {
				//ランダム
				count = ram() % MOVE::move_end;
				//変換
				move = static_cast<MOVE>(count);
				moveTime = 0;
			}
			Move();
		}
		//Z軸の移動
		else {
			int moveZ = 0;
			directionTime++;
			//縮小(最初)
			if (isReduction) {
				//敵のスケール
				directionScale.x = (float)easeOutQuad(directionMaxTime, startScale.x, endScale - startScale.x, directionTime);
				directionScale.y = (float)easeOutQuad(directionMaxTime, startScale.y, endScale - startScale.y, directionTime);
				directionScale.z = (float)easeOutQuad(directionMaxTime, startScale.z, endScale - startScale.z, directionTime);
				//縮小の終わり
				if (directionTime>=directionMaxTime) {
					isReduction = false;
					directionTime = 0;
					moveZ=ram() % 4;
					//前(1)
					if (moveZ == 0) {
						position.z = player->GetPosition().z + front;
					}
					//前の後ろ(2)
					else if (moveZ == 1) {
						position.z = player->GetPosition().z + frontBack;
					}
					//後ろの前(3)
					else if (moveZ==2) {
						position.z = player->GetPosition().z + backFront;
					}
					//後ろ(4)
					else if (moveZ == 3) {
						position.z = player->GetPosition().z + back;
					}
				}

			}
			//拡大(後半)
			else {
				//敵のスケール
				directionScale.x = (float)easeOutQuad(directionMaxTime, endScale, startScale.x - endScale, directionTime);
				directionScale.y = (float)easeOutQuad(directionMaxTime, endScale, startScale.y - endScale, directionTime);
				directionScale.z = (float)easeOutQuad(directionMaxTime, endScale, startScale.z - endScale, directionTime);
				//拡大の終わり
				if (directionTime >= directionMaxTime) {
					isMoveZ = false;
					directionTime = 0;
					moveZTime = 0;
				}
			}
		}
	}
	scale = directionScale;
	////fbx
	//enemyfbxobj->SetPosition(position);
	//enemyfbxobj->SetScale(scale);
	//enemyfbxobj->SetRotation(rotation);
	//enemyfbxobj->Update();
	
	//位置を下に表示
	for (int i = 0; i < 4; i++) {
		XMFLOAT3 pos = position;
		XMFLOAT3 startpos = {};
		XMFLOAT3 endpos = {};

		pos.y = -downLinePosY;
		////簡易的な影
		if (i == 0) {
			pos.x += endSpaceX;
			startpos = { pos.x,pos.y,pos.z - endSpaceZ };
			endpos = { pos.x,pos.y,pos.z + endSpaceZ };

		}
		else if (i == 1) {
			pos.x -= endSpaceX;
			startpos = { pos.x,pos.y,pos.z - endSpaceZ };
			endpos = { pos.x,pos.y,pos.z + endSpaceZ };
		}
		else if (i == 2) {
			pos.z += endSpaceZ;
			startpos = { pos.x - endSpaceX,pos.y,pos.z };
			endpos = { pos.x + endSpaceX,pos.y,pos.z };
		}
		else if (i == 3) {
			pos.z -= endSpaceZ;
			startpos = { pos.x - endSpaceX,pos.y,pos.z };
			endpos = { pos.x + endSpaceX,pos.y,pos.z };
		}
		posLineObject[i]->SetStartPosition(startpos);
		posLineObject[i]->SetEndPosition(endpos);
		posLineObject[i]->SetModel(posLineModel);
		posLineObject[i]->Update();
	}
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

void Boss::WireDraw()
{
	//fbx
	//enemyfbxobj->Draw(cmdList);

	////オブジェクト
	bossObj->Draw();


}

void Boss::Draw()
{
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

void Boss::LineDraw(ID3D12GraphicsCommandList* cmdList)
{
	for (int i = 0; i < 4; i++) {
		posLineObject[i]->Draw(cmdList);
	}
}

void Boss::Move()
{
	switch (move)
	{
	case x:
		//X方向
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
	case y:
		//Y方向
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
	case xy:
		//X方向
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
		//Y方向
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
	}


}

double Boss::easeOutQuad(double time_, double start_, double difference, double totaltime_)
{
	double x = totaltime_ / time_;
	double v = 1 - (1 - x) * (1 - x);
	double ret = difference * v + start_;
	return ret;
}

void Boss::TargetShot()
{
	//自機狙い弾
	if (TARGETSHOT::playerTarget == targetShot) {
		velocityVec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));
	}
	//自機狙い弾+縦
	else if (TARGETSHOT::playerTargetHeight == targetShot) {
		//自機狙い弾
		velocityVec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//縦方向にずらす(+方向)
		velocityVec = { player->GetPosition().x - position.x,(player->GetPosition().y + space) - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//縦方向にずらす(-方向)
		velocityVec = { player->GetPosition().x - position.x,(player->GetPosition().y - space) - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//自機狙い弾+横
	else if (TARGETSHOT::playerTargetWidth == targetShot) {
		//自機狙い弾
		velocityVec = { player->GetPosition().x - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//横方向にずらす(+方向)
		velocityVec = { (player->GetPosition().x + space) - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//横方向にずらす(-方向)
		velocityVec = { (player->GetPosition().x - space) - position.x,player->GetPosition().y - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//何も通らなかった場合(バグ)
	else {

	}

}

void Boss::FixedShotHeight()
{
	//左側
	if (FIXEDSHOT::leftHeight == fixedShot) {
		//真ん中
		velocityVec = { -spaceX - position.x,0 - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//上側
		velocityVec = { -spaceX - position.x,spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//下側
		velocityVec = { -spaceX - position.x,-spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//真ん中
	else if (FIXEDSHOT::centerHeight == fixedShot) {
		//真ん中
		velocityVec = { 0 - position.x,0 - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//上側
		velocityVec = { 0 - position.x,spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//下側
		velocityVec = { 0 - position.x,-spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//右側
	else if (FIXEDSHOT::rightHeight == fixedShot) {
		//真ん中
		velocityVec = { spaceX - position.x,0 - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//上側
		velocityVec = { spaceX - position.x,spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//下側
		velocityVec = { spaceX - position.x,-spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//横方向かバグ
	else {

	}
}

void Boss::FixedShotWidth()
{
	//上側
	if (FIXEDSHOT::upWidth == fixedShot) {
		//左側
		velocityVec = { -spaceX - position.x,spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//真ん中
		velocityVec = { 0 - position.x,spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//右側
		velocityVec = { spaceX - position.x,spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//真ん中
	else if (FIXEDSHOT::centerWidth == fixedShot) {
		//左側
		velocityVec = { -spaceX - position.x,0 - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//真ん中
		velocityVec = { 0 - position.x,0 - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//右側
		velocityVec = { spaceX - position.x,0 - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//下側
	else if (FIXEDSHOT::downWidth == fixedShot) {
		//左側
		velocityVec = { -spaceX - position.x,-spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObject = std::make_unique<EnemyBullet>();
		newObject->Initialize(dxcommon, resource, velocity);
		newObject->SetPosition(position);
		bullets.push_back(std::move(newObject));

		//真ん中
		velocityVec = { 0 - position.x,-spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectPlus = std::make_unique<EnemyBullet>();
		newObjectPlus->Initialize(dxcommon, resource, velocity);
		newObjectPlus->SetPosition(position);
		bullets.push_back(std::move(newObjectPlus));

		//右側
		velocityVec = { spaceX - position.x,-spaceX - position.y, player->GetPosition().z - position.z };
		velocityVec.normalize();
		velocity = XMFLOAT3(velocityVec.x, velocityVec.y, velocityVec.z);
		std::unique_ptr<EnemyBullet>newObjectMinus = std::make_unique<EnemyBullet>();
		newObjectMinus->Initialize(dxcommon, resource, velocity);
		newObjectMinus->SetPosition(position);
		bullets.push_back(std::move(newObjectMinus));
	}
	//縦方向かバグ
	else {

	}
}

void Boss::OnCollision()
{
	life--;
}

void Boss::Reset()
{
	life = maxLife;
	isdead = false;
	firstDirection = false;
}
