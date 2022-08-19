/**
 CPlayer2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include Singleton template
#include "DesignPatterns\SingletonTemplate.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Keyboard controller
#include "Inputs\KeyboardController.h"
#include "Inputs\MouseController.h"

#include "InventoryManager.h"
#include "GameManager.h"

// Include AnimatedSprites
#include "Primitives/SpriteAnimation.h"

#include "../SoundController/SoundController.h"

#include "Physics2D.h"


class CPlayer2D : public CSingletonTemplate<CPlayer2D>, public CEntity2D
{ 
	friend CSingletonTemplate<CPlayer2D>;
public:

	// Init
	bool Init(void);

	// Reset
	bool Reset();

	//map
	void InteractWithMap(void);

	bool bInteractWithMap(int i);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	void UpdateJumpFall(const double dElapsedTime);

	bool isMidAir(void);

	void earseBu(int i);
	int GetfoodBowl();

	void resetStuff();

	int GetCall(int i);

	void SetbActive();

	bool GetbActive();


	void AssignAmtOfDog(int amt);

	std::vector<string> GetHotKeyInv();

	std::vector<int> GetHotKeyQuitity();

	std::vector<int> GetHotKeyid();

	void setHotKeyInventory(std::vector<string> name, std::vector<int> id, std::vector<int> quan);

	int GetSelect();

	int getSelected();

	bool getIframe();
	void SetIframe();

	void fuelTime();

	void setHealth(int damage);

	void addToinventory(int num, string name, int amt, int maxQuantity);

	bool changed=false;

	float healthTime = 0;//this is the counter for how long until health increase

	float getDmg();
protected:
	int select = 5;
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};
	bool iFrame;
	CSpriteAnimation* animatedSprites;

	CPhysics2D cPhysics2D;

	CInventoryManager* cIM;
	CInventoryItem* cII;


	glm::vec2 vec2OldIndex;

	std::vector<glm::vec2> bVec2;

	std::vector<glm::vec2> bVec2Micro;

	std::vector<int> bColor;
	std::vector<int> ID;

	std::vector<int> dirr;
	int foodBowl = false;

	bool Yflip = false;
	bool Rflip = false;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// Keyboard Controller singleton instance
	CKeyboardController* cKeyboardController;
	CMouseController* cMouseController;
	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	CSoundController* CSC;

	// Player's colour
	glm::vec4 runtimeColour;

	// Constructor
	CPlayer2D(void);

	// Destructor
	virtual ~CPlayer2D(void);

	// Constraint the player's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	bool BConstraint(DIRECTION eDirection, int i);

	bool CheckPosition(DIRECTION eDirection);

	void UpdateHealthLives();

	bool canTP = false;


	bool bActive = true;
	std::vector<int> CallAlly;

	glm::vec2 spawn;
	
	void MouseInteracteWithMap(const double dElapsedTime);

	double dt;

	std::vector<string> hotKeyInv = {"Food","","Food","","","","","","",
									"Food","","","Food","","","","","",
									"Food","","Food","","","","","","" };

	std::vector<int> hotKeyInvID = { 0,0,0,0,0,0,0,0,0,
									 0,0,0,0,0,0,0,0,0,
									 0,0,0,0,0,0,0,0,0 };

	std::vector<int> hotKeyInvQuantity = { 99,0,1,0,0,0,0,0,0 ,
											99,0,0,1,0,0,0,0,0,
											99,0,1,0,0,0,0,0,0 };


	string equip="";
	float dmg = 1;
	void InventoryMan();
	void selectKey();

	void Wepon(string e);

	void MouseAction();
};

