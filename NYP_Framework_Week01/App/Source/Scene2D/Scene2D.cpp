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
	if (cMap2D->Init(2 , CSettings::GetInstance()->NUM_TILES_YAXIS, CSettings::GetInstance()->NUM_TILES_XAXIS) == false)
	{
		cout << "map2d failed" << endl;
		return false;
	}

	if (cMap2D->LoadMap("Maps/DM2213_Map_Level_0.csv") == false)
	{
		cout << "map failed" << endl;

		return false;
	}

	
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
	CSC->LoadSound(FileSystem::getPath("Sounds\\pickUp.ogg"),1,true);
	CSC->LoadSound(FileSystem::getPath("Sounds\\lose.ogg"), 2,true);
	CSC->LoadSound(FileSystem::getPath("Sounds\\pour.ogg"), 3,true);
	CSC->LoadSound(FileSystem::getPath("Sounds\\Wishtle.ogg"), 4, true);
	CSC->LoadSound(FileSystem::getPath("Sounds\\Sound_Explosion.ogg"), 5, true);

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

	/*if (G->erupt == true)
	{

	}*/

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_C))
	{
		cGameManager->bPlayerLost = true;
	}

	if (cMap2D->GetCurrentLevel() == 0)
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
		boss->Update(dElapsedTime);
	}

	if (cGameManager->bPlayerLost)
	{
		CGameStateManager::GetInstance()->SetActiveGameState("END");
		cPlayer2D->Reset();
		CSC->PlaySoundByID(2);
		cGameManager->bPlayerLost = false;
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
	if (clock >= 18 || clock <= 6)
	{
		if (worldTime1 >= 10)//change this to see how fast you want enemy to be spawn 
		{
			while (true)
			{
				glm::vec2 asd;
				asd.x = rand() % 32;

				asd.y = rand() % 24;

				if (cMap2D->GetMapInfo(asd.y, asd.x) == 0) {
					timer = 10;
					//change back
					int random_enemy_spawn = rand() % 2;
					if (enemies_spawnned < 11)
					{
						/*int random_enemy_spawn = rand() % 4;*/ // 0 1 2 3
						int random_enemy_spawn = 777;
						if (random_enemy_spawn == 0)
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 302);
							enemies_spawnned++;
						}
						else if (random_enemy_spawn == 1)
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 400);
							enemies_spawnned++;
						}
						else if (random_enemy_spawn == 2)
						{
							cMap2D->SetMapInfo(asd.y, asd.x, 401);
							enemies_spawnned++;
						}
						else
						{
							/*cMap2D->SetMapInfo(asd.y, asd.x, 301);
							enemies_spawnned++;*/
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
			worldTime1 = 0;
		}
	}
	if (cPlayer2D->drop)
	{
		cPlayer2D->drop = false;
		PickUP* cPU = new PickUP();
		cPU->SetShader("Shader2D_Colour");
		if (cPU->Init())
		{
			cPU->SetPlayer2D(cPlayer2D);
			Pick.push_back(cPU);
		}
	}
	for (int i = 0; i < Pick.size(); i++)
	{
		Pick[i]->Update(dElapsedTime);
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