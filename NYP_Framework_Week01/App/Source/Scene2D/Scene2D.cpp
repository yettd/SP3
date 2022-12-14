/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Scene2D.h"
#include <iostream>
using namespace std;

 
// Include Shader Manager
#include "RenderControl\ShaderManager.h"
#include "System\filesystem.h"





/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CScene2D::CScene2D(void) : cMap2D(NULL),cKeyboardController(NULL),cPlayer2D(NULL),cGUI_Scene2D(NULL),cGameManager(NULL), cMouseController(NULL),CSC(NULL),enemyVector(NULL), bulletVector(NULL), boss(NULL), Pick(NULL)
{


	
}

/**
 @brief Destructor
 */
CScene2D::~CScene2D(void)
{ 
	if (cKeyboardController)
	{
		// We won't delete this since it was created elsewhere
		cKeyboardController = NULL;
	}
	if (cMouseController)
	{
		cMouseController = NULL;
	}
	if (cMap2D)
	{
		cMap2D->Destroy();
		cMap2D = NULL;
	}
	if (cPlayer2D)
	{
		cPlayer2D->Destroy();
		cPlayer2D = NULL;
	}
	if (boss)
	{
		delete boss;
		boss = NULL;
	}
	if (cGUI_Scene2D)
	{

		cGUI_Scene2D->Destroy();
		cGUI_Scene2D = NULL;
	}
	if (cGameManager)
	{

		cGameManager->Destroy();
		cGameManager = NULL;
	}
	if(CSC)
	{
		CSC->Destroy();
		CSC = NULL;
	}

	for (int i = 0; i < enemyVector.size(); i++)
	{
		delete enemyVector[i];
		enemyVector[i] = NULL;	
	}
	for (int i = 0; i < bulletVector.size(); i++)
	{
		delete bulletVector[i];
		bulletVector[i] = NULL;
	}
	for (int i = 0; i < Pick.size(); i++)
	{
		delete Pick[i];
		Pick[i] = NULL;
	}
	bulletVector.clear();
	Pick.clear();
	enemyVector.clear();
}

/**
@brief Init Initialise this instance
*/ 
bool CScene2D::Init(void)
{
	// Include Shader Manager

	srand(time(NULL));
	CShaderManager::GetInstance()->Use("Shader2D");
	cKeyboardController = CKeyboardController::GetInstance();
	cMouseController = CMouseController::GetInstance();
	cMap2D = CMap2D::GetInstance();
	cGUI_Scene2D = CGUI_Scene2D::GetInstance();

	cMap2D->SetShader("Shader2D");
	//cGUI_Scene2D->SetShader("Shader2D_Colour");
	if (cMap2D->Init(12 , CSettings::GetInstance()->NUM_TILES_YAXIS, CSettings::GetInstance()->NUM_TILES_XAXIS) == false)
	{
		cout << "map2d failed" << endl;
		return false;
	}

	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv") == false)
	{
		cout << "map 1 failed" << endl;

		return false;
	}

	//level 1
	//top left
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_topleft.csv", 1) == false)
	{
		cout << "map 1 failed" << endl;

		return false;
	}
	//top middle
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_topmiddle.csv", 2) == false)
	{
		cout << "map 2 failed" << endl;

		return false;
	}
	//top right
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_topright.csv", 3) == false)
	{
		cout << "map 3 failed" << endl;

		return false;
	}
	//middle left
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_middleleft.csv", 4) == false)
	{
		cout << "map 4 failed" << endl;

		return false;
	}
	//middle: player spawns here first
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_middle.csv", 5) == false)
	{
		cout << "map 5 failed" << endl;

		return false;
	}
	//middle right
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_middleright.csv", 6) == false)
	{
		cout << "map 6 failed" << endl;

		return false;
	}
	//bottom left
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01.csv", 7) == false)
	{
		cout << "map 7 failed" << endl;

		return false;
	}
	//bottom middle
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_bottommiddle.csv", 8) == false)
	{
		cout << "map 8 failed" << endl;

		return false;
	}
	//bottom right: portal placeholder
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_01_bottomright.csv", 9) == false)
	{
		cout << "map 9 failed" << endl;

		return false;
	}

	//level 2
	//left
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_02_left.csv", 10) == false)
	{
		cout << "map 10 failed" << endl;

		return false;
	}
	//right (ghens)
	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_02_right.csv", 11) == false)
	{
		cout << "map 11 failed" << endl;

		return false;
	}
	cMap2D->SetCurrentLevel(5);
	CShaderManager::GetInstance()->Use("Shader2D_Colour");
	cPlayer2D = CPlayer2D::GetInstance();
	cPlayer2D->SetShader("Shader2D_Colour");

	cGUI_Scene2D->Init();
	if (cPlayer2D->Init() == false)
	{
		cout << "Failed to load CPlayer2D" << endl;
		return false;
	}

	CShaderManager::GetInstance()->Use("Shader2D_Colour");


	cGameManager = CGameManager::GetInstance();
	cGameManager->Init();

	CSC = CSoundController::GetInstance();

	CSC->Init();
	CSC->LoadSound(FileSystem::getPath("Sounds\\break.ogg"),1,true);//added
	CSC->LoadSound(FileSystem::getPath("Sounds\\playerHit.ogg"), 2, true);//added
	CSC->LoadSound(FileSystem::getPath("Sounds\\place.ogg"), 3, true);//added
	CSC->LoadSound(FileSystem::getPath("Sounds\\death.ogg"), 4, true);//added
	CSC->LoadSound(FileSystem::getPath("Sounds\\shoot.ogg"), 5, true);//added
	CSC->LoadSound(FileSystem::getPath("Sounds\\drink.ogg"), 6, true);//added

	enemyVector.clear();
	bulletVector.clear();
	while (true)
	{
		CEnemy2D* cE = new CEnemy2D();
		cE->SetShader("Shader2D_Colour");
		if (cE->Init())
		{
			cE->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(cE);
			a.push_back(cE);
		} 
		else
		{
			break;
		}
	}


	return true;
}

/**
@brief Update Update this instance
*/
bool CScene2D::Update(const double dElapsedTime)
{
	static float timer = 1;
	timer -= dElapsedTime;
	worldTime += dElapsedTime;
	worldTime1 += dElapsedTime;

	cPlayer2D->Update(dElapsedTime);
	cMap2D->Update(dElapsedTime);
	
	cGUI_Scene2D->Update(dElapsedTime);
	if (cGameManager->bLevelCompleted)
	{
		cGameManager->bLevelCompleted = false;
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_L))
	{
		cMap2D->SetCurrentLevel(10);
	}
	/*if (G->erupt == true)
	{

	}*/

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_C))
	{
		cGameManager->bPlayerLost = true;
	}

	if (cMap2D->GetCurrentLevel() == 10)
	{
		if (spawnGhens == false)
		{
			//spawn ghens
			ghens* cE = new ghens();
			cE->SetShader("Shader2D_Colour");
			if (cE->Init())
			{
				cE->SetPlayer2D(cPlayer2D);
				boss = cE;
				G = cE;
				/*enemyVector.push_back(cE);*/
			}
			spawnGhens = true;
		}
	}

	if (boss)
	{
		if (G->corpse_arise == true)
		{
			glm::vec2 corpse_tile;

			if (G->summoned <= cPlayer2D->enemies_unalived)
			{
				corpse_tile.x = rand() % 32;

				corpse_tile.y = rand() % 24;

				if (cMap2D->GetMapInfo(corpse_tile.y, corpse_tile.x) == 0)
				{
					int rand_enemy = rand() % 2;
					if (rand_enemy == 0)
					{
						cMap2D->SetMapInfo(corpse_tile.y, corpse_tile.x, 302);
						G->summoned++;
					}
					else
					{
						cMap2D->SetMapInfo(corpse_tile.y, corpse_tile.x, 301);
						G->summoned++;
					}

					while (true)
					{
						CEnemy2D* cE = new CEnemy2D();
						cE->SetShader("Shader2D_Colour");
						if (cE->Init())
						{
							cE->SetPlayer2D(cPlayer2D);
							enemyVector.push_back(cE);
							a.push_back(cE);
						}
						else
						{
							break;
						}
					}
				}
			}
			G->summonDone = true;
		}
	}

	if (boss)
	{
		boss->Update(dElapsedTime);
	}

	if (cGameManager->bPlayerLost)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("DEATH");
		cPlayer2D->Reset();
		CSC->PlaySoundByID(4);
		cGameManager->bPlayerLost = false;
		cPlayer2D->Reset();
		return false;
	}
	if (cGameManager->bPlayerWon)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("END");
		cPlayer2D->Reset();
		cGameManager->bPlayerWon = false;
		cPlayer2D->Reset();
		return false;
	}

	for (size_t i = 0; i < cPlayer2D->pBullet.size(); i++)
	{
		bulletVector.push_back(cPlayer2D->pBullet[i]);
	}
	cPlayer2D->pBullet.clear();
	cPlayer2D->WatchOutBullet.clear();

	if (boss)
	{

		for (size_t i = 0; i < G->eBullet.size(); i++)
		{
			bulletVector.push_back(G->eBullet[i]);
		}
		G->eBullet.clear();
		G->watchout.clear();
	}

	for (size_t i = 0; i < enemyVector.size(); i++)
	{
		a[i]->watchout.clear();
		for (size_t j = 0; j < a[i]->eBullet.size(); j++)
		{
			bulletVector.push_back(a[i]->eBullet[j]);
			a[i]->eBullet.clear();
		}
	}

	for (size_t i = 0; i < bulletVector.size(); i++)
	{
		if (bulletVector[i]->bIsActive)
		{
			if (bulletVector[i]->player == false)
			{
				cPlayer2D->WatchOutBullet.push_back(bulletVector[i]);
			}
			else
			{
				for (size_t j = 0; j < a.size(); j++)
				{
					a[j]->watchout.push_back(bulletVector[i]);

				}
				if (boss)
				{
					G->watchout.push_back(bulletVector[i]);
				
				}
			}
		}
	}
	
	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->Update(dElapsedTime);
	}
	for (size_t i = 0; i < bulletVector.size(); i++)
	{
		bulletVector[i]->Update(dElapsedTime);
	}


	//	//spawn Enemy
	//if (timer <= 0)
	//{
	//	while (true)
	//	{
	//		glm::vec2 asd;
	//		asd.x = rand() % 32;

	//		asd.y = rand() % 24;

	//		if (cMap2D->GetMapInfo(asd.y, asd.x) == 0) {
	//			timer = 10;
	//			cMap2D->SetMapInfo(asd.y, asd.x, 20);
	//			break;
	//		}

	//	}
	//}

	static int hunger = 0;
	cGUI_Scene2D->setClock(clock);

	if (worldTime >= 15) //change this to set how long you want it to take to 1 hour ( worldtime = 1 means every 1 sec 1 hour passes)
	{
		hunger++;
		clock ++;

		if (clock == 24)
		{
			clock = 0;
		}
		worldTime = 0;
		
	}
	if (hunger >= 1)
	{
		cPlayer2D->fuelTime();
		hunger = 0;
	}

	if (worldTime1 >= 10 && cMap2D->GetCurrentLevel() != 10)//change this to see how fast you want enemy to be spawn 
	{
		for (size_t i = 0; i < 2; i++)
		{
			while (true)
			{
				glm::vec2 asd;
				asd.x = rand() % 32;

				asd.y = rand() % 24;

				if (cMap2D->GetMapInfo(asd.y, asd.x) == 0) {
					timer = 10;


					if (spawnable == true)
					{
						if (clock >= 18 || clock <= 6)
						{
							random_enemy_spawn = rand() % 4; // 0 1 2 3
						}
						else
						{
							random_enemy_spawn = rand() % 2; // 0 1
						}
						if (random_enemy_spawn == 0)
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 401);
							enemies_spawnned++;
						}
						else if (random_enemy_spawn == 1)
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 400);
							enemies_spawnned++;
						}
						else if (random_enemy_spawn == 2)
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 302);
							enemies_spawnned++;
						}
						else
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 301);
							enemies_spawnned++;
						}
					}

					while (true)
					{
						CEnemy2D* cE = new CEnemy2D();
						cE->SetShader("Shader2D_Colour");
						if (cE->Init())
						{
							cE->SetPlayer2D(cPlayer2D);
							enemyVector.push_back(cE);
							a.push_back(cE);
						}
						else
						{
							break;
						}
					}
					break;
				}

			}
		}
		worldTime1 = 0;
	}
	/*if (cPlayer2D->drop)
	{
		cPlayer2D->drop = false;
		PickUP* cPU = new PickUP();
		cPU->SetShader("Shader2D_Colour");
		if (cPU->Init())
		{
			cPU->SetPlayer2D(cPlayer2D);
			Pick.push_back(cPU);
		}
	}*/
	if (cPlayer2D->DropId.size() > 0)
	{
			for (size_t i = 0; i < cPlayer2D->DropId.size(); i++)
			{
					for (size_t j = 0; j < cPlayer2D->DropId[i].second; j++)
					{
						//cout << cPlayer2D->DropId.size() << endl;
						cMap2D->SetMapInfo(cPlayer2D->vec2Index.y, cPlayer2D->vec2Index.x, cPlayer2D->DropId[i].first);
						PickUP* cPU = new PickUP();
						cPU->SetShader("Shader2D_Colour");
						if (cPU->Init())
						{
							cPU->SetPlayer2D(cPlayer2D);
							Pick.push_back(cPU);
						}
					}
			}
	}
	cPlayer2D->DropId.clear();

	if (boss)
	{
		cPlayer2D->bossHp = G->hp;
	}

	for (int i = 0; i < Pick.size(); i++)
	{
		Pick[i]->Update(dElapsedTime);
	}

	//map changing
	//left & right
	static float asd = 5;
	if (cPlayer2D->vec2Index.x >= 31)
	{
		asd += 1;
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 1);
		cPlayer2D->vec2Index.x = 1;
		for (size_t i = 0; i < Pick.size(); i++)
		{
			Pick[i]->bIsActive = false;
		}
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i]->bIsActive = false;
		}
		for (size_t i = 0; i < bulletVector.size(); i++)
		{
			bulletVector[i]->bIsActive = false;
		}

	}
	else if (cPlayer2D->vec2Index.x <= 0)
	{
		asd -= 1;
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() - 1);
		cPlayer2D->vec2Index.x = 30;
		for (size_t i = 0; i < Pick.size(); i++)
		{
			Pick[i]->bIsActive = false;
		}
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i]->bIsActive = false;
		}
		for (size_t i = 0; i < bulletVector.size(); i++)
		{
			bulletVector[i]->bIsActive = false;
		}
	}
	//up and down
	if (cPlayer2D->vec2Index.y >= 23)
	{
		asd -= 3;
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() - 3);
		cPlayer2D->vec2Index.y = 1;
		for (size_t i = 0; i < Pick.size(); i++)
		{
			Pick[i]->bIsActive = false;
		}
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i]->bIsActive = false;
		}
		for (size_t i = 0; i < bulletVector.size(); i++)
		{
			bulletVector[i]->bIsActive = false;
		}
	}
	else if (cPlayer2D->vec2Index.y <= 0)
	{
		asd += 3;
		cMap2D->SetCurrentLevel(cMap2D->GetCurrentLevel() + 3);
		cPlayer2D->vec2Index.y = 22;
		for (size_t i = 0; i < Pick.size(); i++)
		{
			Pick[i]->bIsActive = false;
		}
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i]->bIsActive = false;
		}
		for (size_t i = 0; i < bulletVector.size(); i++)
		{
			bulletVector[i]->bIsActive = false;
		}
	}
	if (cPlayer2D->portal == true)
	{
		cPlayer2D->portal = false;
		for (size_t i = 0; i < Pick.size(); i++)
		{
			Pick[i]->bIsActive = false;
		}
		for (size_t i = 0; i < a.size(); i++)
		{
			a[i]->bIsActive = false;
		}
		for (size_t i = 0; i < bulletVector.size(); i++)
		{
			bulletVector[i]->bIsActive = false;
		}
	}
	return true;
}



/**
 @brief PreRender Set up the OpenGL display environment before rendering
 */
void CScene2D::PreRender(void)
{
	// Reset the OpenGL rendering environment
	glLoadIdentity();

	// Clear the screen and buffer
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Enable 2D texture rendering
	glEnable(GL_TEXTURE_2D);
}

/**
 @brief Render Render this instance
 */
void CScene2D::Render(void)
{
	cMap2D->PreRender();

	cMap2D->Render();

	cMap2D->PostRender();

	cPlayer2D->PreRender();

	cPlayer2D->Render();

	cPlayer2D->PostRender();

	if (boss)
	{
		boss->PreRender();

		boss->Render();

		boss->PostRender();
	}


	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->PreRender();

		enemyVector[i]->Render();

		enemyVector[i]->PostRender();
	}
	for (int i = 0; i < Pick.size(); i++)
	{
		Pick[i]->PreRender();

		Pick[i]->Render();

		Pick[i]->PostRender();
	}
	for (size_t i = 0; i < bulletVector.size(); i++)
	{

		bulletVector[i]->PreRender();

		bulletVector[i]->Render();

		bulletVector[i]->PostRender();
	}
	cGUI_Scene2D->Render();//render last alll the time
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{


}