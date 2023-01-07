
#include "Random.h"
#include "GameScene.h"

#include <atlstr.h>

#include "CpComponents.h"
#include "GameObject.h"
#include "KeyInput.h"
#include "Time.h"
#include "XMFLOATHelper.h"
#include <tchar.h>


void GameScene::Start()
{
	InitData();
	if (m_soundManager == nullptr)
	{
		m_soundManager = std::make_shared<SoundManager>();
		m_soundManager->Register("score", 0, (TCHAR*)_T("Assets/Audio/score.wav"));
		m_soundManager->Register("jump", 1, (TCHAR*)_T("Assets/Audio/jump.wav"));
		m_soundManager->Register("isFloat", 2, (TCHAR*)_T("Assets/Audio/isFloat.wav"));
		m_soundManager->Register("bgm", 3, (TCHAR*)_T("Assets/Audio/bgm.wav"));
		m_soundManager->Register("damage", 4,(TCHAR*)_T( "Assets/Audio/damage.wav"));
	}

	m_world = GameObject::Create();

	for (int i = 0; i < 30; i++)
	{
		auto backSquare = GameObject::Create();
		const auto meshRenderer = backSquare->AddComponent<CpMeshRenderer>();
		const auto rigidBody = backSquare->AddComponent<CpRigidBody>();
		const auto squareMesh = SquareMesh({ 1,1,1,1 });
		meshRenderer->SetMesh(core, squareMesh);
		backSquare->SetParent(m_world);
		rigidBody->isGravity = false;
		rigidBody->velocity = { Random::RangeF(-5, -10),0,0 };
		backSquare->transform.Position = { Random::RangeF(-8, 8),Random::RangeF(0,4),-2 };
		auto scale = 0.1f;
		backSquare->transform.Scale = { scale,scale * 0.1f,scale, };
		m_backSquares.emplace_back(backSquare);
	}

	// 障害物の初期化
	for (int i = 0; i < 20; i++)
	{
		auto cube = GameObject::Create();
		const auto meshRenderer = cube->AddComponent<CpMeshRenderer>();
		const auto rigidBody = cube->AddComponent<CpRigidBody>();
		const auto cubeMesh = CubeMesh({ 1,1,1,1 });
		meshRenderer->SetMesh(core, cubeMesh);
		cube->SetParent(m_world);
		rigidBody->velocity = { Random::RangeF(-7, -3) ,0,0 };
		cube->transform.Position = { Random::RangeF(20, 36), Random::RangeF(2, 4) ,0 };
		auto scale = 0.3f;
		cube->transform.Scale = { scale,scale,scale, };
		m_obstacles.emplace_back(cube);
	}

	// チュートリアル看板の初期化
	{

		m_tutorial = GameObject::Create();
		const auto meshRenderer = m_tutorial->AddComponent<CpMeshRenderer>();
		const auto rigidBody = m_tutorial->AddComponent<CpRigidBody>();
		const auto mesh = SquareMesh({ 1,1,1,1 }, L"Assets/tutorial.png");
		meshRenderer->SetMesh(core, mesh);
		m_tutorial->SetParent(m_world);
		m_tutorial->transform.Position = { 3, -0.2, 0 };
		rigidBody->isGravity = false;
		rigidBody->velocity = { -5, 0, 0 };
		rigidBody->SetActive(false);
		auto scale = 1.f;
		m_tutorial->transform.Scale = { scale,scale,scale, };
	}

	// プレイヤーの初期化
	{
		m_player = GameObject::Create();
		const auto meshRenderer = m_player->AddComponent<CpFlipAnimatedMeshRenderer>();
		const auto rigidBody = m_player->AddComponent<CpRigidBody>();
		rigidBody->gravityMultiplier = 2.f;
		std::vector<Mesh> meshes;
		for (int i = 1; i < 11; i++)
		{
			const std::wstring path = std::format(L"Assets/Player/player{}.png", i);
			const auto mesh = SquareMesh({ 1,1,1,1 }, path);
			meshes.emplace_back(mesh);
		}
		meshRenderer->SetMeshes(core, meshes);
		meshRenderer->RegisterAnimation("default", { 0.05f, true, {0} });
		meshRenderer->RegisterAnimation("run", { 0.05f, true, {0,1} });
		meshRenderer->RegisterAnimation("air", { 0.05f, true, {4} });
		meshRenderer->RegisterAnimation("fall", { 0.05f, true, {5} });
		meshRenderer->RegisterAnimation("float", { 0.05f, true, {6,7,8,9} });
		meshRenderer->RegisterAnimation("damage", { 0.05f, true, {8} });
		meshRenderer->ChangeAnimation("default");
		m_player->SetParent(m_world);
		m_player->transform.Position = { -3, m_playerMinY + 0.01f, 0 };
		auto scale = 0.25f;
		m_player->transform.Scale = { scale,scale,scale, };
		m_playerFloatEnergy = m_playerFloatEnergyMax;
		m_playerFloatEnergyBar = GameObject::Create();
		const auto meshRendererBar = m_playerFloatEnergyBar->AddComponent<CpMeshRenderer>();
		const auto mesh = SquareMesh({ 1,1,1,1 });
		meshRendererBar->SetMesh(core, mesh);
		m_playerFloatEnergyBar->transform.Position = { 0, -1.3,3 };
		m_playerFloatEnergyBar->transform.Scale = { m_playerFloatEnergy ,0.01,1 };
	}

	// 地面の初期化
	{
		m_groundObj = GameObject::Create();
		const auto groundMesh = CubeMesh({ 0.2f,0.2f,0.2f,1.0f }, L"Assets/ground.tga");
		const auto meshRenderer = m_groundObj->AddComponent<CpMeshRenderer>(-999);
		m_groundObj->SetParent(m_world);
		meshRenderer->SetMesh(core, groundMesh);
		m_groundObj->transform.Position = { 0,-2,0 };
		m_groundObj->transform.Scale = { 10,1,2 };
	}

	// スコア表示部の初期化
	{
		m_scoreObj = GameObject::Create();
		m_scoreObj->transform.Position = { -0.235, -1.8,3 };
		m_scoreObj->transform.Scale *= 0.12f;
		const auto numMeshRenderer = m_scoreObj->AddComponent<CpNumMeshRenderer>();
		numMeshRenderer->Init(core, 3);
		numMeshRenderer->SetNumber(0);
	}

	// ロゴ
	{
		m_gameLogo = GameObject::Create();
		m_gameLogo->transform.Position = { 0, 1.5, 0 };
		m_gameLogo->transform.Scale = { 3 , 1, 1 };
		const auto meshRenderer = m_gameLogo->AddComponent<CpMeshRenderer>();
		const auto mesh = SquareMesh({ 1,1,1,1 }, L"Assets/isFloat.png");
		meshRenderer->SetMesh(core, mesh);
	}

	// がめおべら
	{
		m_gameOver = GameObject::Create();
		m_gameOver->transform.Position = { 0, 1.5, 2 };
		m_gameOver->transform.Scale = { 2.5 , 1.5, 1 };
		m_gameOver->transform.Scale *= 0.4;
		const auto meshRenderer = m_gameOver->AddComponent<CpMeshRenderer>();
		const auto mesh = SquareMesh({ 1,1,1,1 }, L"Assets/gameover.png");
		meshRenderer->SetMesh(core, mesh);
	}


	KeyInput::AddListen(VK_SPACE);
	//core->clearColor = {0,0,0,0};
}

SceneState GameScene::Update()
{
	Scene::Update();
	Time::SetCurrent();
	if (!m_isGame)
	{
		PreGameLoop();
		return SceneState::Running;
	}
	if (m_isGameOver)
	{
		return GameOverLoop();
	}
	InGameLoop();
	return SceneState::Running;
}

void GameScene::Draw()
{
	core->BeginRender();
	m_groundObj->Draw();
	if (m_isGame)
	{
		for (const auto& sq : m_backSquares)
		{
			sq->Draw();
		}
		for (int i = 0; i < m_level + 1; i++)
		{
			if (i >= m_obstacles.size()) break;
			m_obstacles[i]->Draw();
		}
	}
	else
	{
		m_gameLogo->Draw();
	}
	if (m_isGameOver)
	{
		m_gameOver->Draw();
	}
	m_player->Draw();
	m_tutorial->Draw();
	m_scoreObj->Draw();
	m_playerFloatEnergyBar->Draw();
	core->EndRender();
}

void GameScene::InitData()
{
	m_score = 0;
	m_isGame = false;
	m_isPlayerAir = false;
	m_isFloat = false;
	m_isGameOver = false;
	m_level = 1;
	m_next = 10;
	m_playerFloatEnergy = m_playerFloatEnergyMax;
	m_playerFloatingTime = 0;
}

void GameScene::PreGameLoop()
{
	m_tutorial->Update();
	m_groundObj->Update();
	const auto playerRb = m_player->GetComponent<CpRigidBody>();
	playerRb->isGravity = false;

	m_waitTime -= Time::DeltaTime();
	if (m_waitTime > 0)
	{
		m_player->Update();
		m_playerFloatEnergyBar->Update();
		m_scoreObj->Update();
		m_gameLogo->Update();
		return;
	}
	if (KeyInput::OnKeyDown(VK_SPACE))
	{
		const auto playerRb = m_player->GetComponent<CpRigidBody>();
		m_player->transform.Position.y = m_playerMinY;
		playerRb->AddForce({ 0,m_jumpPower,0 });
		playerRb->isGravity = true;
		m_isGame = true;
		m_player->Update();
		const auto tutorialRb = m_tutorial->GetComponent<CpRigidBody>();
		tutorialRb->SetActive(true);
		m_scoreObj->Update();
		m_playerFloatEnergyBar->Update();
		m_soundManager->Play("jump");
		m_soundManager->Play("bgm", false, true);
		return;
	}
	m_player->Update();
	m_playerFloatEnergyBar->Update();
	m_scoreObj->Update();
	m_gameLogo->Update();
}

void GameScene::InGameLoop()
{
	if (m_score > m_next)
	{
		m_level++;
		m_next += m_next + m_level * 2;
		CString cs;
		cs.Format(_T("[level up] %d next->%d\n"), m_level, m_next);
		OutputDebugString(cs);

	}
	// チュートリアル看板
	{
		const auto diff = m_tutorial->transform.Position - m_player->transform.Position;
		const auto diffVec = XMLoadFloat3(&diff);
		const auto length = XMVector3Length(diffVec);
		if (XMVectorGetX(length) < 1.0f)
		{
			GameOver();
			return;
		}
		if (m_tutorial->transform.Position.x < -6)
		{
			m_tutorial->GetComponent<CpRigidBody>()->SetActive(false);
			m_tutorial->transform.Position.x = 100;
			m_score++;
			m_scoreObj->GetComponent<CpNumMeshRenderer>()->SetNumber(m_score);
			m_soundManager->Play("score");
			CString cs;
			cs.Format(_T("[score] %d\n"), m_score);
			OutputDebugString(cs);
		}
		m_tutorial->Update();
	}

	// プレイヤーの処理
	if (m_isFloat)
	{
		m_playerFloatEnergy -= Time::DeltaTime();
		m_playerFloatingTime += Time::DeltaTime();
		if (m_playerFloatEnergy < 0) m_playerFloatEnergy = 0;
	}
	if (m_isPlayerAir && KeyInput::OnKeyDown(VK_SPACE))
	{
		m_isFloat = true;
	}
	else if (m_isFloat && KeyInput::OnKeyUp(VK_SPACE) || m_playerFloatEnergy < 0.0001f)
	{
		m_isFloat = false;
	}

	const auto playerRb = m_player->GetComponent<CpRigidBody>();
	const auto playerAnim = m_player->GetComponent<CpFlipAnimatedMeshRenderer>();
	if (m_player->transform.Position.y < m_playerMinY)
	{
		m_player->transform.Position.y = m_playerMinY;
		playerRb->velocity.y = 0;
		m_isPlayerAir = false;
		m_playerFloatEnergy += Time::DeltaTime() * m_playerFloatEnergyMax;
		if (m_playerFloatEnergy > m_playerFloatEnergyMax)
		{
			m_playerFloatEnergy = m_playerFloatEnergyMax;
		}
	}
	else
	{
		m_isPlayerAir = true;
	}
	if (KeyInput::OnKeyDown(VK_SPACE) && m_player->transform.Position.y < m_playerMinY + 0.001f)
	{
		m_player->transform.Position.y = m_playerMinY;
		playerRb->AddForce({ 0,m_jumpPower,0 });
		m_soundManager->Play("jump");
	}
	if (m_isFloat)
	{
		playerRb->isGravity = false;
		playerRb->velocity.y = 0;
		playerAnim->ChangeAnimation("float");
		core->clearColor = { 0.4,0.4,0.4,0.4 };
		if (m_playerFloatingTime < 0.3)
		{
			if (static_cast<int>(m_playerFloatingTime / 0.025) % 2 == 0)
			{
				core->clearColor = { 0.4,0.4,0.4,0.4 };
			}
			else
			{
				core->clearColor = { 0.42,0.42,0.42,0.42 };
			}

		}
		else core->clearColor = { 0.4,0.4,0.4,0.4 };
		m_soundManager->Play("isFloat", false);
	}
	else
	{
		playerRb->isGravity = true;
		core->clearColor = { 0.5,0.5,0.5,0.5 };
		m_soundManager->Stop("isFloat");
		m_playerFloatingTime = 0;
	}

	// animation
	if (m_player->transform.Position.y == m_playerMinY)
	{
		playerAnim->ChangeAnimation("run");
	}
	else if (!m_isFloat)
	{
		if (playerRb->velocity.y >= 0) playerAnim->ChangeAnimation("air");
		else playerAnim->ChangeAnimation("fall");
	}

	// 後ろの線の処理
	for (const auto& sq : m_backSquares)
	{
		if (sq->transform.Position.x < -8)
		{
			sq->transform.Position.x = 8;
		}
		sq->Update();
	}



	// 障害物たちの処理
	for (int i = 0; i < m_level + 1; i++)
	{
		if (i >= m_obstacles.size()) break;
		const auto obstacle = m_obstacles[i];
		const auto diff = obstacle->transform.Position - m_player->transform.Position;
		const auto diffVec = XMLoadFloat3(&diff);
		const auto length = XMVector3Length(diffVec);
		if (XMVectorGetX(length) < .5f)
		{
			GameOver();
			return;
		}
		const float delta = Time::DeltaTime();
		obstacle->transform.Rotation.x -= 2.0f * delta;
		obstacle->transform.Rotation.y += 1.2f * delta;
		auto rb = obstacle->GetComponent<CpRigidBody>();
		if (m_isFloat)
		{
			rb->isGravity = false;
			rb->velocity.y = 0;
		}
		else
		{
			rb->isGravity = true;
		}
		if (obstacle->transform.Position.x < -5)
		{
			m_score++;
			m_scoreObj->GetComponent<CpNumMeshRenderer>()->SetNumber(m_score);
			m_soundManager->Play("score");
			CString cs;
			cs.Format(_T("[score] %d\n"), m_score);
			OutputDebugString(cs);
			obstacle->transform.Position.x = Random::RangeF(6, 24);
			obstacle->transform.Position.y = Random::RangeF(1, 4);
			obstacle->GetComponent<CpRigidBody>()->velocity.x = Random::RangeF(-6 - m_level, -3);
		}
		if (obstacle->transform.Position.y < -0.5f)
		{
			rb->velocity.y = 0;
			rb->AddForce({ 0,3.5f,0 });
		}
		obstacle->Update();
	}



	m_player->Update();
	m_groundObj->Update();
	m_scoreObj->Update();
	m_playerFloatEnergyBar->transform.Scale.x = m_playerFloatEnergy;
	m_playerFloatEnergyBar->Update();
}

SceneState GameScene::GameOverLoop()
{
	if (KeyInput::OnKeyDown(VK_SPACE))
	{
		return SceneState::Reload;
	}
	for (const auto& obj : m_backSquares)
	{
		obj->Update();
	}
	for (const auto& obj : m_obstacles)
	{
		obj->Update();
	}
	m_player->Update();
	m_groundObj->Update();
	m_scoreObj->Update();
	m_gameOver->Update();
	m_tutorial->Update();
	m_playerFloatEnergyBar->transform.Scale.x = m_playerFloatEnergy;
	m_playerFloatEnergyBar->Update();
	return SceneState::Running;
}

void GameScene::GameOver()
{
	m_soundManager->Stop("bgm");
	m_soundManager->Play("damage", false);
	m_soundManager->Stop("isFloat");
	m_player->GetComponent<CpRigidBody>()->SetActive(false);
	m_player->GetComponent<CpFlipAnimatedMeshRenderer>()->ChangeAnimation("damage");
	m_tutorial->GetComponent<CpRigidBody>()->SetActive(false);
	for (const auto& obj : m_backSquares)
	{
		obj->GetComponent<CpRigidBody>()->SetActive(false);
	}
	for (const auto& obj : m_obstacles)
	{
		obj->GetComponent<CpRigidBody>()->SetActive(false);
	}
	m_isGameOver = true;
}

