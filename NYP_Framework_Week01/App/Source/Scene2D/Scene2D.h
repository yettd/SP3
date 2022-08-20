/**
 CScene2D
 @brief A class which manages the 2D game scene
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>
#include "DesignPatterns/SingletonTemplate.h"

#include "../SoundController/SoundController.h"
#include "../GameStateManagement/GameStateManager.h"

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include vector
#include <vector>

// Include Keyboard controller
#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

#include "Map2D.h"
#include "Player2D.h"
#include "GUI_Scene2D.h"
#include "GameManager.h"
#include "Enemy2D.h"
#include "bullet.h"
#include "ghens.h"
#include <vector>
// Add your include files here

class CScene2D : public CSingletonTemplate<CScene2D>
{
	friend CSingletonTemplate<CScene2D>;
public:
	// Init
	bool Init(void);

	// Update
	bool Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	float worldTime=0;

	float worldTime1=0;

	float clock = 0;
	int enemies_spawnned = 1;

protected:
	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;
	CMouseController* cMouseController;
	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// Add your variables and methods here.
	std::vector<CEntity2D*> enemyVector;
	CEntity2D* boss;
	ghens* G;


	std::vector<CEnemy2D*> a;


	// Constructor
	CScene2D(void);
	// Destructor
	virtual ~CScene2D(void);

	CPlayer2D* cPlayer2D;
	CMap2D* cMap2D;
	CGUI_Scene2D* cGUI_Scene2D;
	CGameManager* cGameManager;
	CSoundController* CSC;

	std::vector<bullet*> bulletVector;

	bool spawnGhens = false;

};

