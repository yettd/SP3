/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#pragma once

// Include SingletonTemplate
#include "DesignPatterns/SingletonTemplate.h"

// Include CEntity2D
#include "Primitives/Entity2D.h"

// FPS Counter
#include "TimeControl\FPSCounter.h"

// Include CInventoryManager
#include "InventoryManager.h"

// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

// Include IMGUI
// Important: GLEW and GLFW must be included before IMGUI
#ifndef IMGUI_ACTIVE
#include "GUI\imgui.h"
#include "GUI\backends\imgui_impl_glfw.h"
#include "GUI\backends\imgui_impl_opengl3.h"
#define IMGUI_ACTIVE
#endif

#include "InventoryManager.h"

#include "GameControl/Settings.h"

#include <string>

#include "Player2D.h"

#include "Map2D.h"

using namespace std;

class CGUI_Scene2D : public CSingletonTemplate<CGUI_Scene2D>, public CEntity2D
{
	friend CSingletonTemplate<CGUI_Scene2D>;
public:
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

	void setUI(bool e);

	void setClock(int i);
	void craft(int i);
	bool craftable(int i);
protected:
	// Constructor
	CGUI_Scene2D(void);

	int Clock;
	// Destructor
	virtual ~CGUI_Scene2D(void);

	// FPS Control
	CFPSCounter* cFPSCounter;

	// Flags for IMGUI
	ImGuiWindowFlags window_flags;
	float m_fProgressBar;

	CSettings* cSettings;

	// The handler containing the instance of CInventoryManager
	CInventoryManager* cInventoryManager;
	// The handler containing the instance of CInventoryItem
	CInventoryItem* cInventoryItem;

	// These variables are for IMGUI demo only
	bool show_demo_window;
	bool show_another_window;
	ImVec4 clear_color;


	CPlayer2D* lever;
	CMap2D* map;
	bool PressE;


	std::vector<string>PHK;
	std::vector<int>PHKQ;
	std::vector<int> PHKID;
	int PS;
//crafting stuff
	//name and ID of iteam that can craft
	vector<std::pair<string, int>> nameID = 
	{ 
		{"metalcubePLUS (block)",103},
		{"rustedwoodPLUS (block)",105},
		{"upgradealtar (block)",104},
		{"pistol mark1 (weapon)",880},
		{"pistol mark2 (weapon)",881},
		{"energy gun (weapon)",882},
		{"rusted sword (weapon)",872},
		{"metal sword (weapon)",873},
		{"photon sword (weapon)",874},
	};

	vector<int> maxAmt = { 1,1,1,1,1,1,1,1,1 };

	//first vector to decide what to craft
	//second vector what is needed to craft
	vector<std::vector<std::pair<string, int>>> recipie = 
	{
		{{"metalcube",4}},
		{{"rustedwood",4}},
		{{"metalparts",7},{"ironhorn",5}},
		{{"firepowder",3}, {"metalparts",4}, {"rustedwood",2}},
		{{"firepowder",5}, {"metalparts",6}, {"pistol mark1 (weapon)",1}},
		{{"firepowder",10}, {"metalparts",12}, {"pistol mark2 (weapon)",1}},
		{{"metalparts",5}, {"rustedwood",3}},
		{{"metalparts",7}, {"rustedwood",5}, {"rusted sword (weapon)",1}},
		{{"metalparts",14}, {"rustedwood",10}, {"metal sword (weapon)",1}},
	};

};
