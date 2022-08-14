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
CScene2D::CScene2D(void) : cMap2D(NULL),cKeyboardController(NULL),cPlayer2D(NULL),cGUI_Scene2D(NULL),cGameManager(NULL), cMouseController(NULL),CSC(NULL),enemyVector(NULL)
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
	while (true)
	{
		CEnemy2D* cE = new CEnemy2D();
		cE->SetShader("Shader2D_Colour");
		if (cE->Init())
		{
			cE->SetPlayer2D(cPlayer2D);
			enemyVector.push_back(cE);
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

	cPlayer2D->Update(dElapsedTime);
	cMap2D->Update(dElapsedTime);
	
	cGUI_Scene2D->Update(dElapsedTime);
	if (cGameManager->bLevelCompleted)
	{
		cGameManager->bLevelCompleted = false;
	}

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_C))
	{
		cGameManager->bPlayerLost = true;
	}


	if (cGameManager->bPlayerLost)
	{

		return false;
	}

	
	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->Update(dElapsedTime);
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

	cGUI_Scene2D->Render();
	for (int i = 0; i < enemyVector.size(); i++)
	{
		enemyVector[i]->PreRender();

		enemyVector[i]->Render();

		enemyVector[i]->PostRender();
	}


}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CScene2D::PostRender(void)
{


}