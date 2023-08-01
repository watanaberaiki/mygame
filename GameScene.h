#pragma once
#include"Vector3.h"
#include "Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include"Object3d.h"
#include <DirectXMath.h>
#include<math.h>
#include <d3dcompiler.h>
#include <DirectXTex.h>
#include "CollisionPrimitive.h"
#include "Collision.h"
#include"Camera.h"
#include <math.h>
#include"ParticleManager.h"
#include"FbxLoader.h"
#include"FbxObject3D.h"
#include"Railcamera.h"

#include"LineModel.h"
#include"LineObject.h"
#include"Enemy.h"
#include"Player.h"
#include"CubeModel.h"
#include"CubeObject3D.h"
#include"ResourceManager.h"
#include"EnemyBullet.h"
#include"CSVLoader.h"
#pragma warning(push)
#pragma warning(disable:4267)
#include<map>
#include<utility>
#pragma warning(pop)
class GameScene
{
//�����o�֐�
public:
	GameScene();
	~GameScene();
	//������
	void Initialize(DirectXCommon* dxCommon, Input* input);
	//�X�V
	void Update();
	//�`��
	void Draw();
	//����ꗗ
	void AllCollision();
	//�p�[�e�B�N������
	void Particle(XMFLOAT3 pos);
	//�C�[�W���O
	double easeOutQuad(double time,double start,double difference,double totaltime);
private:
	//�|�C���^
	Input* input_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;

	//�p�[�e�B�N��
	std::list<std::unique_ptr<ParticleManager>> particles;

	//�X�v���C�g���ʕ�
	SpriteCommon* spriteCommon = nullptr;
	//�X�v���C�g
	Sprite* hitsprite = new Sprite();
	Sprite* mariosprite = new Sprite();
	Sprite* menu = new Sprite();

	/*OBJ���烂�f���f�[�^��ǂݍ���*/
	//3D���f��
	Model* spheremodel=nullptr;
	Model* blockmodel = nullptr;
	//3D���f���̓����蔻��
	XMFLOAT3 minsphereModel = {}, maxsphereModel = {};
	
	//3D�I�u�W�F�N�g
	Object3d* sphereobj=nullptr;
	Object3d* blockobj = nullptr;

	//�J����
	Camera* camera = nullptr;

	//�J�����p
	XMFLOAT3 eye = {0,0,0};		//���_���W
	XMFLOAT3 target = {0,0,0};	//�����_���W
	XMFLOAT3 up = {0,0,0};		//������x�N�g��
	XMMATRIX matView = {};

	//FBXModel
	FbxModel* boneTestModel = nullptr;
	FbxModel* cube = nullptr;

	static const int bonetestsize = 5;
	FbxObject3D* bonetest[bonetestsize] = {};

	//���[���J����
	Railcamera* railCamera = nullptr;

	//�G
	static const int enemysize = 10;
	std::list<std::unique_ptr<Enemy>>enemys;
	/*const std::list<std::unique_ptr<EnemyBullet>>& enemyBullets;*/
	//�v���C���[
	Player* player = nullptr;
	////�}�b�v�p
	int enemycount = 0;

	//�q�b�g�m�F
	bool isHit = false;

	//���C��
	LineModel* linemodel = nullptr;
	LineObject* lineobject = nullptr;

	ResourceManager* resorcemanager = nullptr;

	//������܂Ƃ߂ĊǗ�
	std::list<std::unique_ptr<CubeObject3D>>collisionBoxs;

	CSVLoader* enemycsv = nullptr;

	//���j���[
	bool isMenu = false;
	bool backMenu = false;
	double time = 0.0;
	double backtime = 0.0;
	double maxTime = 50.0;
	double start = -WinApp::window_width/2;
	double end = WinApp::window_width / 2;

	//�n��
	Object3d* floorobj = nullptr;

};

