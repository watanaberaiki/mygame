#include "Input.h"
#include"WinApp.h"
#include"DirectXCommon.h"
#include "CollisionPrimitive.h"
#include "Collision.h"
#include"Camera.h"
#include"FbxLoader.h"
#include"GameScene.h"
#include"2d/PostEffect.h"
#include"2d/SpriteCommon.h"

#pragma comment (lib,"d3dcompiler.lib")

enum Scene {
	title,
	game,
	clear,
	gameover
};

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	//�@��ՃV�X�e���̏�����
	//�|�C���^
	Input* input = nullptr;
	WinApp* winApp = nullptr;
	DirectXCommon* dxCommon = nullptr;
	PostEffect* postEffect = nullptr;

	//WindowsAPI�̏�����
	winApp = new WinApp();
	winApp->Initialize();

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		//�f�o�b�O���C���[��L���ɂ���
		debugController->EnableDebugLayer();
		//�����GPU���ł��`�F�b�N���s����悤�ɂ���
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif
	//DirectX�̏�����
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);
#ifdef  _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(dxCommon->GetDevice()->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		//���o���G���[���Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		//�G���[���Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		//�x�����Ɏ~�܂�
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
		//���
		infoQueue->Release();
		//�}�����郁�b�Z�[�W��ID
		D3D12_MESSAGE_ID denyIds[] = {
			//Windows11�ł�DXGI�f�o�b�O���C���[��DX12�f�o�b�O���C���[�̑��ݍ�p�o�O�ɂ��G���[���b�Z�[�W
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE
		};
		//�}�����郌�x��
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		//�w�肵�����b�Z�[�W�̕\����}������
		infoQueue->PushStorageFilter(&filter);
	}
#endif 
	FbxLoader::GetInstance()->Initialize(dxCommon->GetDevice());


	//���͂̏�����
	input = new Input();
	input->Initialize(winApp);

	//3D�I�u�W�F�N�g�ÓI������
	Object3d::StaticInitialize(dxCommon->GetDevice(), winApp->window_width, winApp->window_height);
	ParticleManager::StaticInitialize(dxCommon, winApp->window_width, winApp->window_height);

	GameScene* gamescene = nullptr;
	gamescene = new GameScene();
	gamescene->Initialize(dxCommon,input);

	//�|�X�g�G�t�F�N�g�p�e�N�X�`���̓ǂݍ���
	SpriteCommon* spritecommon = nullptr;
	spritecommon = new SpriteCommon();
	spritecommon->Initialize(dxCommon);
	spritecommon->LoadTexture(0, "hit.png");
	spritecommon->LoadTexture(1, "white1280x720.png");
	/*SpriteCommon::LoadTexture(0, "hit.png");*/
	//�|�X�g�G�t�F�N�g�̏�����
	postEffect = new PostEffect();
	postEffect->SetSpriteCommon(spritecommon);
	/*postEffect->SetTexture(spritecommon, 1);*/
	postEffect->Initialize();
	//postEffect->SetSize(XMFLOAT2(200.0f, 200.0f));
	/*postEffect->SetAnchorPoint(XMFLOAT2(0.5f, 0.5f));
	postEffect->SetPosition(XMFLOAT2(500.0f,200.0f));*/

	//�ŏ��̃V�[���̏�����

#ifdef _DEBUG

#endif
	//�Q�[�����[�v
	while (true) {
		//���b�Z�[�W������H
		if (winApp->ProcessMesseage()) {
			//�Q�[�����[�v�𔲂���
			break;
		}
		//���͂̍X�V
		input->Update();
		//�Q�[���V�[��
		gamescene->Update();
		//// 4.�`��R�}���h��������

		//�����_�[�e�N�X�`���ւ̕`��
		postEffect->PreDrawScene(dxCommon->GetCommandlist());
		//�Q�[���V�[��
		gamescene->Draw();
		postEffect->PostDrawScene(dxCommon->GetCommandlist());
		dxCommon->PreDraw();

		//�|�X�g�G�t�F�N�g�̕`��
		postEffect->Draw(dxCommon->GetCommandlist());
		

		dxCommon->PostDraw();
		// 4.�`��R�}���h�����܂�
	}
	//���͊J��
	delete input;
	//WindowsAPI�̏I������
	winApp->Finalize();
	//WindowsAPI�̉��
	delete winApp;
	//DirectX���
	delete dxCommon;
	////GameScene���
	//delete gamescene;
	//�|�X�g�G�t�F�N�g���
	delete postEffect;
	//�X�v���C�g�R�������
	delete spritecommon;
	//�J�������
	/*delete camera;*/
	FbxLoader::GetInstance()->Finalize();
	//��ՃV�X�e���̏I��

	return 0;
}

