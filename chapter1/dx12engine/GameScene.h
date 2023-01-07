#pragma once
#include "SoundManager.h"
#include "GameObject.h"
#include "Scene.h"

class GameScene : public Scene
{
private:
	int m_score = 0;
	bool m_isGame = false;
	bool m_isPlayerAir = false;
	bool m_isFloat = false;
	bool m_isGameOver = false;
	int m_level = 1;
	int m_next = 10;
	std::shared_ptr<GameObject> m_world;
	std::shared_ptr<GameObject> m_player;
	std::shared_ptr<GameObject> m_playerFloatEnergyBar;
	float m_jumpPower = 8;
	float m_waitTime = 1.f;
	const float m_playerFloatEnergyMax = 2.f;
	float m_playerFloatEnergy = 0;
	float m_playerFloatingTime = 0;
	const float m_playerMinY = -0.7f;
	std::vector<std::shared_ptr<GameObject>> m_backSquares;
	std::shared_ptr<GameObject> m_tutorial;
	std::vector<std::shared_ptr<GameObject>> m_obstacles;
	std::shared_ptr<GameObject> m_groundObj;
	std::shared_ptr<GameObject> m_scoreObj;
	std::shared_ptr<GameObject> m_gameLogo;
	std::shared_ptr<GameObject> m_gameOver;
	std::shared_ptr<SoundManager> m_soundManager;

public:
	explicit GameScene(Core* core) : Scene(core) {}
	void Start() override;
	SceneState Update() override;
	void Draw() override;
private:
	void InitData();
	void PreGameLoop();
	void InGameLoop();
	SceneState GameOverLoop();
private:
	void GameOver();
};
