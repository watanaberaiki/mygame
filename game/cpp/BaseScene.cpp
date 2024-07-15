#include "BaseScene.h"
#include "ClearScene.h"
#include"GameOverScene.h"
#include"TitleScene.h"
#include"SceneManager.h"
void BaseScene::Initialize(DirectXCommon* dxCommon, ImguiManager* imgui)
{
	dxCommon_ = dxCommon;
	imgui_ = imgui;
}
void BaseScene::Finalize()
{
	delete spriteCommon;
	//3dオブジェクト解放
	delete sphereObj;
	delete blockObj;
	for (Object3d*& object : objects) {
		delete object;
	}
	for (WireObject*& wireobject : heightWireObjects) {
		delete wireobject;
	}
	//////レベルデータ解放
	//delete leveldata;
	//delete floorobj;
	//3Dモデル解放
	delete sphereModel;
	delete blockModel;
	FBX_SAFE_DELETE(boneTestModel);
	FBX_SAFE_DELETE(cube);
	for (int i = 0; i < bonetestsize; i++) {
		FBX_SAFE_DELETE(bonetest[i]);
	}
	delete enemycsv;
}
void BaseScene::Update()
{
}
void BaseScene::Draw()
{
}
//画面遷移
void BaseScene::Transition(Scene nextScene_)
{
	//遷移中
	if (isTransition) {
		//パーティクル座標
		partPos = eye;
		partPos.y = eye.y + 5;
		partPos.z = eye.z + 10;

		//パーティクル
		TransitionParticle(partPos);
		if (MaxDelay <= delayTime) {
			//白い画像の位置
			transitionWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, (float)easeOutQuad(MaxTransitionTime, transitionStartPosY, transitionEndposY - transitionStartPosY, totalTransitionTime)));
			transitionWhiteSprite->Update();

			//トータルのタイムを増やす
			if (MaxTransitionTime > totalTransitionTime) {
				totalTransitionTime++;
			}

			//MaxTimeを超えたら戻る演出を入れる
			if (MaxTransitionTime <= totalTransitionTime) {
				isTransition = false;
				isBackTransition = true;
				totalTransitionTime = 0;
				BaseScene* changeScene =nullptr;
				if (nextScene_ == Scene::Clear) {
					changeScene = new ClearScene();
				}
				else if (nextScene_ == Scene::Dead) {
					changeScene = new GameOverScene();
				}
				else if (nextScene_ == Scene::Title) {
					changeScene = new TitleScene();
					changeScene->isBackTransition = true;
				}
				//シーン切り替え
				sceneManager_->SetNextScene(changeScene);
				delayTime = 0;
			}
		}
		delayTime++;

	}
	//遷移戻り
	else if (isBackTransition) {
		transitionWhiteSprite->Update();
		if (MaxDelay <= delayTime) {
			//パーティクルの位置
			partPos = eye;
			partPos.y = eye.y - 5;
			partPos.z = eye.z + 10;
			//後半は出ないようにしておく
			if (totalTransitionTime > MaxTransitionTime - 40.0) {

			}
			else {
				//パーティクル
				TransitionBackParticle(partPos, (int)(MaxTransitionTime - totalTransitionTime * 2));
			}

			//白い画像の位置
			transitionWhiteSprite->SetPosition(XMFLOAT2(WinApp::window_width / 2, (float)easeOutQuad(MaxTransitionTime, transitionEndposY, transitionStartPosY - transitionEndposY, totalTransitionTime)));

			//トータルのタイムを増やす
			if (MaxTransitionTime > totalTransitionTime) {
				totalTransitionTime++;
			}
			//MaxTimeを超えたら戻る
			if (MaxTransitionTime <= totalTransitionTime) {
				isTransition = false;
				isBackTransition = false;
				totalTransitionTime = 0;
				delayTime = 0;
			}
		}
		delayTime++;
	}

	//particles->Update();

}
//画面遷移時に使うパーティクル
void BaseScene::TransitionParticle(XMFLOAT3 pos_)
{
	//パーティクル
	/*std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");*/
	particles->SetEmitterPos(pos_);
	for (int i = 0; i < 30; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 20.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos / 2.0f;
		//pos.z = (float)rand() / RAND_MAX * rnd_pos / 2.0f;

		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		//vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = -rnd_vel;
		//vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		//const float rnd_acc = 0.001f;
		XMFLOAT3 acc{};
		//acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		//色
		const float rnd_color = 1.0f;
		XMFLOAT4 color{  };
		color.x = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.y = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.z = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.w = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;

		//時間
		const int rnd_life = 80;
		int life = 0;
		life = rand() % rnd_life + 1;
		life += 20;

		//追加
		particles->Add(life, pos, vel, acc, 0.5f, 0.1f, color, false);
	}
	particles->Update();
}
//画面遷移時に使うパーティクル
void BaseScene::TransitionBackParticle(XMFLOAT3 pos_, int num)
{
	//パーティクル
	/*std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");*/
	particles->SetEmitterPos(pos_);
	for (int i = 0; i < num; i++) {
		//X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
		const float rnd_pos = 20.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos / 2.0f;
		//pos.z = (float)rand() / RAND_MAX * rnd_pos / 2.0f;

		//X,Y,Z全て[-0.05f,+0.05f]でランダムに分布
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		//vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = rnd_vel;
		//vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		//重力に見立ててYのみ[-0.001f,0]でランダムに分布
		//const float rnd_acc = 0.001f;
		XMFLOAT3 acc{};
		//acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		//色
		const float rnd_color = 1.0f;
		XMFLOAT4 color{  };
		color.x = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.y = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.z = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.w = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;

		//時間
		const int rnd_life = 40;
		int life = 0;
		life = rand() % rnd_life + 1;
		life += 20;

		//追加
		particles->Add(life, pos, vel, acc, 0.5f, 0.1f, color, false);
	}
	particles->Update();
}

//イージング
double BaseScene::easeOutQuad(double time_, double start_, double difference, double totaltime_)
{
	double x = totaltime_ / time_;
	double v = 1 - (1 - x) * (1 - x);
	double ret = difference * v + start_;
	return ret;
}
