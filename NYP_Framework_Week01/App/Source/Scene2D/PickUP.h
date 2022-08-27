/**
 PickUP
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#pragma once

// Include shader
#include "RenderControl\shader.h"

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include CEntity2D
#include "Primitives/Entity2D.h"

// Include the Map2D as we will use it to check the player's movements and actions
class CMap2D;

// Include Settings
#include "GameControl\Settings.h"

// Include Physics2D
#include "Physics2D.h"

// Include Player2D
#include "Player2D.h"

#include "bullet.h"

class PickUP : public CEntity2D
{
public:
	// Constructor
	PickUP(void);

	// Destructor
	virtual ~PickUP(void);

	// Init
	bool Init(void);

	// Update
	void Update(const double dElapsedTime);

	// PreRender
	void PreRender(void);

	// Render
	void Render(void);

	// PostRender
	void PostRender(void);

	// Set the indices of the enemy2D
	void Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis);

	// Set the number of microsteps of the enemy2D
	void Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis);

	// Set the UV coordinates of the enemy2D
	void Setvec2UVCoordinates(const float fUVCoordinate_XAxis, const float fUVCoordinate_YAxis);

	// Get the indices of the enemy2D
	glm::vec2 Geti32vec2Index(void) const;

	// Get the number of microsteps of the enemy2D
	glm::vec2 Geti32vec2NumMicroSteps(void) const;

	// Set the UV coordinates of the enemy2D
	glm::vec2 Getvec2UVCoordinates(void) const;

	// Set the handle to cPlayer to this class instance
	void SetPlayer2D(CPlayer2D* cPlayer2D);


	// boolean flag to indicate if this enemy is active
	bool bIsActive;

	void SetHp(int i);

	int rand_dir; //random direction Clifford moves

	vector<bullet*> eBullet;


	vector<bullet*> watchout;

	int getId();

protected:
	enum DIRECTION
	{
		LEFT = 0,
		RIGHT = 1,
		UP = 2,
		DOWN = 3,
		NUM_DIRECTIONS
	};

	enum FSM
	{
		IDLE = 0,
		NUM_FSM
	};


	glm::vec2 i32vec2OldIndex;

	//CS: The quadMesh for drawing the tiles
	CMesh* quadMesh;

	// Handler to the CMap2D instance
	CMap2D* cMap2D;

	// A transformation matrix for controlling where to render the entities
	glm::mat4 transform;

	// The vec2 which stores the indices of the enemy2D in the Map2D
	glm::vec2 i32vec2Index;

	// The vec2 variable which stores The number of microsteps from the tile indices for the enemy2D. 
	// A tile's width or height is in multiples of these microsteps
	glm::vec2 i32vec2NumMicroSteps;

	// The vec2 variable which stores the UV coordinates to render the enemy2D
	glm::vec2 vec2UVCoordinate;

	// The vec2 which stores the indices of the destination for enemy2D in the Map2D
	glm::vec2 i32vec2Destination;
	// The vec2 which stores the direction for enemy2D movement in the Map2D
	glm::vec2 i32vec2Direction;

	// Settings
	CSettings* cSettings;

	// Physics
	CPhysics2D cPhysics2D;

	// Current color
	glm::vec4 runtimeColour;

	// Handle to the CPlayer2D
	CPlayer2D* cPlayer2D;

	glm::vec2 escapeDestination;


	// Current FSM
	FSM sCurrentFSM;

	// FSM counter - count how many frames it has been in this FSM
	int iFSMCounter;

	// Max count in a state
	const int iMaxFSMCounter = 60;

	// Constraint the enemy2D's position within a boundary
	void Constraint(DIRECTION eDirection = LEFT);

	// Check if a position is possible to move into
	bool CheckPosition(DIRECTION eDirection);

	// Check if the enemy2D is in mid-air
	bool IsMidAir(void);

	// Update Jump or Fall
	void UpdateJumpFall(const double dElapsedTime = 0.0166666666666667);

	// Let enemy2D interact with the player
	bool InteractWithPlayer(void);


	// Update direction
	void UpdateDirection(void);

	// Flip horizontal direction. For patrol use only
	void FlipHorizontalDirection(void);

	// Update position
	void UpdatePosition(void);

	void ShortCutPath(glm::vec2 des);
	//myStuff;

	float hp=11;
	float speed=.2f;

	bool chase = false;

	int enemyHealth = 100;
	bool isBlues = true;
	bool cowplscome;

	int enemyType = 0;
	/*
	0 for Blues
	1 for Clifford
	2 for Mechanical Cow
	3 for Iron Unicorn
	*/

	float walktimer = 0.0f;

	bool targetLocked = false;
	string name;
	int Iid;
	int MaxQuant;
	vector<int> id =
	{
		1100,
		1000 + 103,
		1000 + 104,
		1000 + 105,
		1000 + 10,
		1000 + 11,
		1000 + 12,
		1000 + 13,
		1000 + 14,
		1000 + 15,
		1000 + 20,
		1000 + 30,

		1000 + 872,
		1000 + 873,
		1000 + 874,
		1000 + 880,
		1000 + 881,
		1000 + 882,

	};//id + 1000


	vector<string> image = 
	{
		"Image/Scene2D_GroundTile.tga",
		"Image/metalcubePLUS.tga",
		"Image/upgradealtar.tga",
		"Image/rustedwoodPLUS.tga",
		"Image/rustedwood.tga",
		"Image/metalcube.tga",
		"Image/gunpowder.tga",
		"Image/metalparts.tga",
		"Image/ironhorn.tga",
		"Image/oilcan.tga",
		"Image/ghensheart.tga",
		"Image/Scene2D_PI.tga",

		"Image/rustedsword.tga",
		"Image/metalsword.tga",
		"Image/photonsword.tga",
		"Image/pistolmark1.tga",
		"Image/pistolmark2.tga",
		"Image/energygun.tga",
		"Image/Scene2D_PI.tga"

	};//image for drop


	vector<string>nameList = 
	{ 
		"WoodenBlock (block)",
		"metalcubePLUS (block)",
		"upgradealtar",
		"rustedwoodPLUS (block)",
		"rustedwood",
		"metalcube",
		"firepowder",
		"metalparts",
		"ironhorn",
		"oilcan",
		"ghensheart",
		"portal (block)",
		"rusted sword (weapon)",
		"metal sword (weapon)",
		"photon sword (weapon)",
		"pistol mark1 (weapon)",
		"pistol mark2 (weapon)",
		"energy gun (weapon)",

	};//name of item


	vector<int> quantity = 
	{
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		10,
		1,
		1,
		1,

		1,
		1,
		1,
		1,
		1,
		1,
	};//max amt


	float inactive = 1.f;
};

