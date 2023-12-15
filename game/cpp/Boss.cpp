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
	posLineModel->Initialize(dxcommon->GetDevice(), 0.2f, -0.2f);
	posLineModel->SetImageData(XMFLOAT4(255, 0, 0, 1));
	for (int i = 0; i < 2; i++) {
		posLineObject[i] = new LineObject();
		posLineObject[i]->Initialize();
		posLineObject[i]->SetModel(posLineModel);
	}

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

		//動き
		moveTime++;
		if (moveTime > moveMaxTime) {
			//ランダム
			count = ram() % MOVE::move_end;
			//変換
			move = static_cast<MOVE>(count);
			moveTime = 0;
		}
		Move();

		//位置を横に表示
		for (int i = 0; i < 2; i++) {
			XMFLOAT3 pos = position;
			pos.x = 0;
			pos.y = -0.5;
			if (i == 0) {
				pos.x += widthSpace;
			}
			else if (i == 1) {
				pos.x -= widthSpace;
			}
			posLineObject[i]->SetStartPosition(pos);
			posLineObject[i]->Update();
		}
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
	for (int i = 0; i < 2; i++) {
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
	life = 10;
	isdead = false;
}
