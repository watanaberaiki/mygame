#include "GameScene.h"

GameScene::GameScene()
{
}

GameScene::~GameScene()
{
	delete spriteCommon;
	//3d�I�u�W�F�N�g���
	delete sphereobj;
	delete blockobj;
	//3D���f�����
	delete spheremodel;
	delete blockmodel;
	FBX_SAFE_DELETE(boneTestModel);
	FBX_SAFE_DELETE(cube);
	for (int i = 0; i < bonetestsize; i++) {
		FBX_SAFE_DELETE(bonetest[i]);
	}
	delete enemycsv;
}

void GameScene::Initialize(DirectXCommon* dxCommon, Input* input)
{
	this->dxCommon_ = dxCommon;
	this->input_ = input;

	//���f�������w�肵�ăt�@�C���ǂݍ���
	/*FbxLoader::GetInstance()->LoadModelFromFile("cube");*/

	eye = XMFLOAT3(0, 0, -10);	//���_���W
	target = XMFLOAT3(0, 0, 0);	//�����_���W
	up = XMFLOAT3(0, 1, 0);		//������x�N�g��
	//�J����
	camera = new Camera();
	camera->Initialize(input_);
	camera->SetEye(eye);
	camera->SetTarget(target);
	camera->SetUp(up);
	camera->Update();

	//�I�u�W�F�N�g3d�J����
	Object3d::SetCamera(camera);
	//�����蔻��L���[�u�I�u�W�F�N�g
	CubeObject3D::SetCamera(camera);
	CubeObject3D::SetDevice(dxCommon_->GetDevice());
	CubeObject3D::CreateGraphicsPipeline();

	//�f�o�C�X���Z�b�g
	FbxObject3D::SetDevice(dxCommon_->GetDevice());
	//�J�������Z�b�g
	FbxObject3D::SetCamera(camera);
	//�O���t�B�b�N�X�p�C�v���C������
	FbxObject3D::CreateGraphicsPipeline();

	//���C��������
	LineObject::SetCamera(camera);
	LineObject::SetDevice(dxCommon->GetDevice());
	LineObject::CreateGraphicsPipeline();

	//�ǂݍ���
	resorcemanager = ResourceManager::Getinstance();
	boneTestModel = resorcemanager->LoadFbx("boneTest");
	cube = resorcemanager->LoadFbx("fbxcube");
	resorcemanager->LoadObj("blackcube");
	resorcemanager->LoadObj("redcube");



	//�v���C���[
	Player::SetInput(input);
	Player::SetDxCommon(dxCommon);
	player = new Player;
	player->Initialize();



	//�G
	enemycsv = new CSVLoader();
	enemycsv->LoadCSV("Resources/csv/enemy.csv");
	Enemy::SetDxCommon(dxCommon);
	for (int i = 0; i < enemysize; i++) {
		std::unique_ptr<Enemy>newObject = std::make_unique<Enemy>();
		newObject->Initialize();
		newObject->SetPosition(enemycsv->GetPosition(i));
		newObject->SetRotation(enemycsv->GetRotation(i));
		newObject->SetScale(enemycsv->GetScale(i));

		//newObject->SetPosition(XMFLOAT3((float)(i*0.2),(float)(i*0.2),(float)i*20));
		newObject->Update();
		enemys.push_back(std::move(newObject));
	}
	
	//�p�[�e�B�N��


	//�X�v���C�g���ʕ��̏�����
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);

	//�e�N�X�`���ǂݍ���
	spriteCommon->LoadTexture(0, "hit.png");
	spriteCommon->LoadTexture(1, "mario.jpg");

	//�X�v���C�g�Ƀe�N�X�`�����蓖��
	hitsprite->Initialize(spriteCommon, 0);
	mariosprite->Initialize(spriteCommon, 1);
	//�X�v���C�g�����ʒu
	mariosprite->SetPosition({ 800,0 });
	mariosprite->Update();

	////3D���f��
	//spheremodel = Model::LoadFromObj("Skydome");
	//blockmodel = Model::LoadFromObj("redcube");
	////�����蔻��
	//minsphereModel = spheremodel->GetminModel();
	//maxsphereModel = spheremodel->GetmaxModel();

	////��
	//sphereobj = Object3d::Create();
	//sphereobj->SetModel(spheremodel);
	//sphereobj->SetPosition({ 0,0,0 });

	////�u���b�N
	//blockobj = Object3d::Create();
	//blockobj->SetModel(blockmodel);
	//blockobj->SetPosition({ 0,5,0 });

	/*railCamera->Initialize(camera);
	railCamera->Update();
	blockobj->SetParentCamera(railCamera);*/


	////���C��
	//linemodel = new LineModel();
	//linemodel->Initialize(dxCommon->GetDevice(), 1.5, -1.5);
	//linemodel->SetImageData(XMFLOAT4(255, 255, 255, 1));
	//lineobject = new LineObject();
	//lineobject->Initialize();
	//lineobject->SetModel(linemodel);
	//lineobject->SetPosition(XMFLOAT3(0, 0, 0));
	//lineobject->SetScale(XMFLOAT3(10.0, 1.0, 1.0));
}

void GameScene::Update()
{
	camera->Update();
	matView = camera->GetmatView();
	//�v���C���[
	player->SetPositionZ(eye.z + 3.0f);
	player->Update();

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Update();
		//���񂾍ۂ̃p�[�e�B�N��
		if (enemy->GetisDead()) {
			Particle(enemy->GetPos());
		}
	}
	//�G�̎��񂾏���
	enemys.remove_if([](std::unique_ptr<Enemy>& enemy) {
		return enemy->GetisDead();
		});

	//�p�[�e�B�N��
	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Update();
	}

	AllCollision();
}

void GameScene::Draw()
{
	//�I�u�W�F�N�g�`��
	Object3d::PreDraw(dxCommon_->GetCommandlist());
	//�v���C���[
	player->Draw(dxCommon_->GetCommandlist());

	//�G
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemy->Draw(dxCommon_->GetCommandlist());
	}
	Object3d::PostDraw();

	//�p�[�e�B�N��
	for (std::unique_ptr<ParticleManager>& particle : particles)
	{
		particle->Draw();
	}

	//�f�o�b�O�\��
	//�v���C���[
	player->DebugDraw(dxCommon_->GetCommandlist());

	////�G
	//for (std::unique_ptr<Enemy>& enemy : enemys)
	//{
	//	enemy->DebugDraw(dxCommon_->GetCommandlist());
	//}

	//�X�v���C�g�`��
	spriteCommon->PreDraw();
	if (isHit) {
		hitsprite->Draw();
	}

	

	spriteCommon->PostDraw();
}

void GameScene::AllCollision()
{
	//���e���X�g�̎擾
	const std::list<std::unique_ptr<PlayerBullet>>& playerBullets = player->GetBullet();
	//�G�e���X�g�̎擾
	int i = 0;
	
	/*for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		enemyBullets=enemy->GetBullet();
		i++;
	}*/

	////���L�����ƓG�e�̔���
	//for (int i = 0; i < enemysize; i++) {
	//	for (const std::unique_ptr<EnemyBullet>& bullet : enemyBullets) {
	//		if (player->GetCubeObject()->CheakCollision(bullet->GetCubeObject())) {
	//			bullet->OnCollision();
	//			player->OnCollision();
	//		}
	//	}
	//}

	//���L�����ƓG�̔���
	for (std::unique_ptr<Enemy>& enemy : enemys)
	{
		if (player->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
			player->OnCollision();
		}
	}

	for (std::unique_ptr<Enemy>& enemy : enemys) {
		for (const std::unique_ptr<EnemyBullet>& enemybullet : enemy->GetBullet()) {
			if (player->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
				enemybullet->OnCollision();
				player->OnCollision();
			}
		}

	}

	////���e�ƓG�e�̔���
	//for (int i = 0; i < enemysize; i++) {
	//	for (const std::unique_ptr<EnemyBullet>& enemybullet : enemyBullets[i]) {
	//		for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
	//			if (playerbullet->GetCubeObject()->CheakCollision(enemybullet->GetCubeObject())) {
	//				playerbullet->OnCollision();
	//				enemybullet->OnCollision();
	//			}
	//		}
	//	}
	//}

	//���e�ƓG�̔���
	for (const std::unique_ptr<PlayerBullet>& playerbullet : playerBullets) {
		for (std::unique_ptr<Enemy>& enemy : enemys) {
			if (playerbullet->GetCubeObject()->CheakCollision(enemy->GetCubeObject())) {
				playerbullet->OnCollision();
				enemy->OnCollision();
			}
		}
	}

}

void GameScene::Particle(XMFLOAT3 pos)
{
	//�p�[�e�B�N��
	std::unique_ptr<ParticleManager>newparticle = std::make_unique<ParticleManager>();
	newparticle->Initialize("line.png");
	newparticle->SetEmitterPos(pos);
	for (int i = 0; i < 50; i++) {
		//X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float rnd_pos = 1.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

		//X,Y,Z�S��[-0.05f,+0.05f]�Ń����_���ɕ��z
		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		//�d�͂Ɍ����Ă�Y�̂�[-0.001f,0]�Ń����_���ɕ��z
		const float rnd_acc = 0.001f;
		XMFLOAT3 acc{};
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		//�F
		const float rnd_color = 1.0f;
		XMFLOAT4 color{  };
		color.x = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.y = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.z = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		color.w = (float)rand() / RAND_MAX * rnd_color - rnd_color / 2.0f;
		//�ǉ�
		newparticle->Add(20, pos, vel, acc, 1.0f, 0.0f, color);
	}
	newparticle->Update();
	particles.push_back(std::move(newparticle));
}
