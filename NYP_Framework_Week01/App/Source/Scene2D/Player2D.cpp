/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>

#include <algorithm>

using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
#include "Primitives/MeshBuilder.h"

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::CPlayer2D(void)
	: cMap2D(NULL)
	, cKeyboardController(NULL)
	, animatedSprites(NULL)
	, runtimeColour(glm::vec4(1.0f))
{
	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CPlayer2D::~CPlayer2D(void)
{
	// We won't delete this since it was created elsewhere
	cKeyboardController = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);

	if (animatedSprites)
	{
		delete animatedSprites;
		animatedSprites = NULL;
	}
}

/**
  @brief Initialise this instance
  */
bool CPlayer2D::Init(void)
{
	
	// Store the keyboard controller singleton instance here
	cKeyboardController = CKeyboardController::GetInstance();
	// Reset all keys since we are starting a new game
	cKeyboardController->Reset();
	// Store the keyboard controller singleton instance here
	cMouseController = CMouseController::GetInstance();
	// Reset all keys since we are starting a new game
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game

	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	// Create the quad mesh for the player
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	//quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the player texture
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/chrono.png", true);
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/chrono.png" << endl;
		return false;
	}

	//CS: Create the animated sprite and setup the animation 
	animatedSprites = CMeshBuilder::GenerateSpriteAnimation(4, 4, cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	animatedSprites->AddAnimation("idle", 0,1);
	animatedSprites->AddAnimation("right", 8, 11);
	animatedSprites->AddAnimation("left", 4, 7);

	animatedSprites->AddAnimation("up", 12, 15);

	animatedSprites->AddAnimation("down", 1, 3);
	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	cPhysics2D.Init();
	cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);


	InventoryMan();
	spawn = vec2Index;


	Yflip = false;
	Rflip = false;

	CSC = CSoundController::GetInstance();
	iFrame = false;
	return true;
}

/**
 @brief Reset this instance
 */
bool CPlayer2D::Reset()
{
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	if (cMap2D->FindValue(200, uiRow, uiCol) == false)
		return false;	// Unable to find the start position of the player, so quit this game
	animatedSprites->PlayAnimation("idle", -1, 1.0f);
	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	vec2NumMicroSteps = glm::i32vec2(0, 0);

	//Set it to fall upon entering new level
	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);


	//CS: Play the "idle" animation as default
	animatedSprites->PlayAnimation("idle", -1, 1.0f);

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	return true;
}

void CPlayer2D::InteractWithMap(void)
{
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x))
	{

	case 9: //eruption tile
		if (getIframe() == false)
		{
			SetIframe();
			setHealth(15);
		}
		break;
	case 30:
		portal = true;
		cMap2D->SetCurrentLevel(10);
		break;
	default:
		break;
	}

	for (size_t i = 0; i < WatchOutBullet.size(); i++)
	{
		if (((vec2Index.x >= WatchOutBullet[i]->vec2Index.x - 1) &&
			(vec2Index.x <= WatchOutBullet[i]->vec2Index.x + 1))
			&&
			((vec2Index.y >= WatchOutBullet[i]->vec2Index.y - 1) &&
				(vec2Index.y <= WatchOutBullet[i]->vec2Index.y + 1)))
		{
			if (getIframe()==false)
			{
				SetIframe();
				if (WatchOutBullet[i]->boss == false)
				{
					setHealth(5);
				}
				else
				{
					setHealth(10);
				}
			}
			WatchOutBullet[i]->bIsActive = false;
		}

	}


}

bool CPlayer2D::bInteractWithMap(int i)
{

	return false;
}

/**
 @brief Update this instance
 */
void CPlayer2D::Update(const double dElapsedTime)
{
	fireRate -= dElapsedTime;
	shooting = false;
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_F))
	{
		addToinventory(14, "ironhorn", 10, 10);
		addToinventory(13, "metalparts", 10, 10);
		addToinventory(10, "rustedwood", 10, 10);
		addToinventory(12, "firepowder", 10, 10);
		addToinventory(15, "oilcan", 1, 1);
		addToinventory(103, "metalcubePLUS (block)", 1, 10);
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_K))
	{
		setHealth(100);
	}
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_Q))
	{
		if (equip != "")
		{

			cII = cIM->GetItem(equip);
			cII->Remove(1);
			DropId.push_back(std::make_pair(hotKeyInvID[select - 1] + 1000, 1));
			hotKeyInvQuantity[select - 1]--;
			if (hotKeyInvQuantity[select - 1] == 0)
			{
				hotKeyInv[select - 1] = "";
				hotKeyInvID[select - 1] = 0;
				equip = hotKeyInv[select - 1];
			}
		}
	}
	static float RespawnTimer = 5;
	CGameManager::GetInstance()->timer += dElapsedTime;
	dt = dElapsedTime;

	// Store the old position
	if (bActive ==false)
	{
		
	}
	static float iFrameCounter = 1;
	if (iFrame)
	{
		iFrameCounter -= dt;

		if (iFrameCounter <= 0)
		{
			iFrame = false;
			iFrameCounter = 1;
		}
		
	}


	MouseInteracteWithMap(dElapsedTime);
	MouseAction();

	vec2OldIndex = vec2Index;
	runtimeColour = glm::vec4(1.0f, 1.0, 0.0, 1.0f);
	static int dir=2;
	static int tid = 100;
	healthTime += dElapsedTime;
	selectKey();

	

	if (cKeyboardController->IsKeyDown(GLFW_KEY_A))
	{
		
		if (vec2Index.x >= 0)
		{
			vec2NumMicroSteps.x--;
			if (vec2NumMicroSteps.x < 0)
			{
				vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}
		Constraint(LEFT);

		if (!CheckPosition(LEFT))
		{
			vec2Index.x = vec2OldIndex.x;
			vec2NumMicroSteps.x = 0;
		}
		/*if (isMidAir() == true)
		{
			if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
			{
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			}
		}*/
		animatedSprites->PlayAnimation("left", -1, 1.0f);
		//runtimeColour = glm::vec4(1.0f, 1.0, 0.0, 1.f);

	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_D))
	{

		
		vec2NumMicroSteps.x++;
		if (vec2NumMicroSteps.x > cSettings->NUM_STEPS_PER_TILE_XAXIS)
		{
			vec2NumMicroSteps.x =0;
			vec2Index.x++;
		}

		Constraint(RIGHT);

 		if (!CheckPosition(RIGHT))
		{
			vec2NumMicroSteps.x = 0;
		}

	/*	if (isMidAir() == true)
		{
			if (cPhysics2D.GetStatus() != CPhysics2D::STATUS::JUMP)
			{
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
			}
		}*/


		animatedSprites->PlayAnimation("right", -1, 1.0f);
//			runtimeColour = glm::vec4(1.0f, 1.0, 0.0, 1.f);

	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_W))
	{
		//buffer = 1;

		
		
		//moveHowLong += dElapsedTime;
		vec2NumMicroSteps.y++;
		if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_XAXIS)
		{
			vec2NumMicroSteps.y = 0;
			vec2Index.y++;
		}

		Constraint(UP);

		if (!CheckPosition(UP))
		{

			vec2NumMicroSteps.y = 0;
		}


		Constraint(UP);

	 	CheckPosition(UP);
		animatedSprites->PlayAnimation("up", -1, 1.0f);
		//runtimeColour = glm::vec4(1.0f, 0.0, 0.0, 0.5f);

	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_S))
	{
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y--;
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}
		Constraint(DOWN);

		if (!CheckPosition(DOWN))
		{
			vec2Index.y = vec2OldIndex.y;
			vec2NumMicroSteps.y = 0;
		}

		Constraint(DOWN);
		CheckPosition(DOWN);
		animatedSprites->PlayAnimation("down", -1, 1.0f);

		//runtimeColour = glm::vec4(0.0f, 1.0, 0.0, 0.5f);

	}
	if (cKeyboardController->IsKeyDown(GLFW_KEY_N))
	{
		SetIframe();
	}

	InteractWithMap();
	cII = cIM->GetItem("Fuel");
	if (cII->GetCount() == cII->GetMaxCount())
	{
		if (healthTime >= 2) // set timing for how fast u want the health to increase
		{
			cII = cIM->GetItem("Health");
			cII->Add(15);
			healthTime = 0;
		}
	}

	UpdateJumpFall(dElapsedTime);
	animatedSprites->Update(dElapsedTime);
	UpdateHealthLives();


	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);

}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CPlayer2D::PreRender(void)
{
	// Activate blending mode
	if (bActive == false)
	{
		return;
	}
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CPlayer2D::Render(void)
{
	if (bActive == false)
	{
		return;
	}
	glBindVertexArray(VAO);
	// get matrix's uniform location and set matrix
	unsigned int transformLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "transform");
	unsigned int colorLoc = glGetUniformLocation(CShaderManager::GetInstance()->activeShader->ID, "runtimeColour");
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	transform = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
	transform = glm::translate(transform, glm::vec3(vec2UVCoordinate.x,
													vec2UVCoordinate.y,
													0.0f));
	// Update the shaders with the latest transform
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform4fv(colorLoc, 1, glm::value_ptr(runtimeColour));

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);
	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

		//Render the Player sprite
		glBindVertexArray(VAO);
		//quadMesh->Render();
		animatedSprites->Render();
		glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CPlayer2D::PostRender(void)
{
	if (bActive == false)
	{
		return;
	}
	// Disable blending
	glDisable(GL_BLEND);
}

void CPlayer2D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		cPhysics2D.SetTime((float)dElapsedTime);
		cPhysics2D.Update();
		glm::vec2 v2Displacemen = cPhysics2D.GetDisplacement();

		int iIndex_YAxis_OLD = vec2Index.y;
		int iDisplacement_MicroSteps = (int)(v2Displacemen.y / cSettings->MICRO_STEP_YAXIS);
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (vec2NumMicroSteps.y < 0)
				{
					vec2NumMicroSteps.y = 0;
				}
				vec2Index.y++;

			}
		}
		Constraint(UP);

		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
		{
			vec2Index.y = i;
			if (CheckPosition(UP) == false)
			{
				vec2NumMicroSteps.y = 0;
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetDisplacement().y <= 0.0f))
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}



	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		cPhysics2D.SetTime((float)dElapsedTime);
		cPhysics2D.Update();
		glm::vec2 v2Displacemen = cPhysics2D.GetDisplacement();

		int iIndex_YAxis_OLD = vec2Index.y;
		int iDisplacement_MicroSteps = (int)(v2Displacemen.y / cSettings->MICRO_STEP_YAXIS);
		if (vec2Index.y >= 0)
		{
			vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (vec2NumMicroSteps.y < 0)
			{
				vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}
		Constraint(DOWN);
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			vec2Index.y = i;
			if (CheckPosition(DOWN) == false)
			{
				if (i != iIndex_YAxis_OLD)
				{
					vec2Index.y = i + 1;
				}
				cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				vec2NumMicroSteps.y = 0;
			}
		}
	}

}

bool CPlayer2D::isMidAir(void)
{
	if (vec2Index.y == 0 )
	{
		return false;
	}
	if ((vec2NumMicroSteps.x == 0) && (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 0)||
		(vec2NumMicroSteps.x == 0) && ((cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 12)&& canTP)||
		(vec2NumMicroSteps.x == 0) && ((cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 16)&& canTP) ||
		(vec2NumMicroSteps.x == 0) && (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 31) ||
		(vec2NumMicroSteps.x == 0) && (cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 33))
	{
		return true;
	}
	return false;
}

void CPlayer2D::earseBu(int i)
{

}

/**
 @brief Constraint the player's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CPlayer2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			vec2NumMicroSteps.x = 0;
			runtimeColour = glm::vec4(1.f, 0.f, 0.f, 1.0f);

		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			vec2NumMicroSteps.x = 0;
			runtimeColour = glm::vec4(1.f, 0.f, 0.f, 1.0f);

		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			vec2NumMicroSteps.y = 0;
			runtimeColour = glm::vec4(1.f, 0.f, 0.f, 1.0f);

		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			vec2NumMicroSteps.y = 0;
			runtimeColour = glm::vec4(1.f, 0.f, 0.f, 1.0f);
		}
	}
	else
	{
		cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
}

bool  CPlayer2D::BConstraint(DIRECTION eDirection,int i)
{
	if (eDirection == LEFT)
	{
		if (bVec2[i].x < 0)
		{
			return true;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (bVec2[i].x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			return true;

		}
	}
	else if (eDirection == UP)
	{
		if (bVec2[i].y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			return true;

		}
	}
	else if (eDirection == DOWN)
	{
		if (bVec2[i].y < 0)
		{
 			return true;
		}
	}
	else
	{
		cout << "CPlayer2D::Constraint: Unknown direction." << endl;
	}
	return false;
}




bool CPlayer2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2NumMicroSteps.y == 0)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100))
			{

				return false;
			}
		}
		else if (vec2NumMicroSteps.y != 0)
		{
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) || (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == RIGHT)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.x >= cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2NumMicroSteps.x = 0;
			return true;
		}
		// If the new position is fully within a row, then check this row only
		if (vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == UP)
	{
		// If the new position is at the top row, then return true
		if (vec2Index.y >= cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2NumMicroSteps.y = 0;
			return true;
		}
		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100) )
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100))
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		cout << "CPlayer2D::CheckPosition: Unknown direction." << endl;
	}
	return true;
}

void CPlayer2D::UpdateHealthLives()
{
	
	
}

void CPlayer2D::UpdateFull(int max,string name)
{

	for (size_t i = 0; i < hotKeyInv.size(); i++)
	{
		if (hotKeyInv[i] == "")
		{
			InventoryIsFull = false;
			return;
		}
		
	}
	InventoryIsFull = true;
}


void CPlayer2D::MouseInteracteWithMap(const double dElapsedTime)
{
	static float breakTimer = 0;
	if (cMouseController->IsButtonUp(0))
	{
		breakTimer = 0;
	}
	if (breakTimer != 0)
	{
		//play sound;
	}

	float posX = cMouseController->GetMousePositionX() / cSettings->iWindowWidth * 32; //convert (0,800) to (0,80)
	float posY = 24 - (cMouseController->GetMousePositionY() / cSettings->iWindowHeight * 24);
	glm::vec2 mousePos(posX, posY);
	
	if ((mousePos.x > 0 && mousePos.x < cSettings->NUM_TILES_XAXIS - 1) && (mousePos.y > 0 && mousePos.y < cSettings->NUM_TILES_YAXIS - 1))
	{
		if (cPhysics2D.CalculateDistance(vec2Index, mousePos) <= 3)
		{
	
			switch (cMap2D->GetMapInfo(mousePos.y, mousePos.x))
			{
		

			case 101: //steelpile
				if (cMouseController->IsButtonDown(0))
				{
					breakTimer += dt;
					if (breakTimer >= 2)
					{
						cMap2D->SetMapInfo(mousePos.y, mousePos.x, 0);
						addToinventory(11, "metalcube", 1, 10);
						breakTimer = 0;
					}
					else
					{
						CSC->PlaySoundByID(1);
					}
				}
				break;

			case 102: //rusted tree
				if (cMouseController->IsButtonDown(0))
				{
					breakTimer += dt;
					if (breakTimer >= 2)
					{
						cMap2D->SetMapInfo(mousePos.y, mousePos.x, 0);
						addToinventory(10, "rustedwood", 1, 10);
						breakTimer = 0;
					}
					else
					{
						CSC->PlaySoundByID(1);
					}
				}
				break;

			case 103:
				if (cMouseController->IsButtonDown(0))
				{
					breakTimer += dt;
					if (breakTimer >= 2)
					{
						cMap2D->SetMapInfo(mousePos.y, mousePos.x, 0);
						addToinventory(103, "metalcubePLUS (block)", 1, 10);
						breakTimer = 0;
					}
					else
					{
						CSC->PlaySoundByID(1);
					}
				}
				break;


			case 105:
				if (cMouseController->IsButtonDown(0))
				{
					breakTimer += dt;
					if (breakTimer >= 2)
					{
						cMap2D->SetMapInfo(mousePos.y, mousePos.x, 0);
						addToinventory(105, "rustedwoodPLUS (block)", 1, 10);
						breakTimer = 0;
					}
					else
					{
						CSC->PlaySoundByID(1);
					}
				}
				break;
			case 30:
				if (cMouseController->IsButtonDown(0))
				{
					breakTimer += dt;
					if (breakTimer >= 2)
					{
						cMap2D->SetMapInfo(mousePos.y, mousePos.x, 0);
						addToinventory(30, "portal (block)", 1, 1);
						breakTimer = 0;
					}
					else
					{
						CSC->PlaySoundByID(1);
					}
				}
				break;


			default:
				break;
			}
		}

		if (cMouseController->IsButtonDown(1))
		{
			if (cPhysics2D.CalculateDistance(vec2Index, mousePos) <= 3)
			{
				if (cMap2D->GetMapInfo(mousePos.y, mousePos.x) == 0)
				{

					string checker = equip;
					std::transform(checker.begin(), checker.end(), checker.begin(), ::tolower);
					if (checker.find("block") != string::npos)
					{
						cII = cIM->GetItem(equip);
						cMap2D->SetMapInfo(mousePos.y, mousePos.x, hotKeyInvID[select - 1]);
						cII->Remove(1);
						hotKeyInvQuantity[select - 1]--;

						CSC->PlaySoundByID(3);
						if (hotKeyInvQuantity[select - 1] == 0)
						{
							hotKeyInv[select - 1] = "";
							hotKeyInvID[select - 1] = 0;
							equip = hotKeyInv[select - 1];
						}

					}
				}
			}
		}
	}
}

void CPlayer2D::InventoryMan()
{
	cIM = CInventoryManager::GetInstance();
	cII = cIM->Add("Lives", "Image/Scene2D_Lives.tga", 3, 3);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("Fuel", "Image/fuel.tga", 10, 10);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cII->vec2Size = glm::vec2(25, 25);


	/*cII = cIM->Add("Food", "Image/dogFood.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);*/

	cII = cIM->Add("", "Image/blank.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);


	//weaponary
	cII = cIM->Add("pistol mark1 (weapon)", "Image/pistolmark1.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("pistol mark2 (weapon)", "Image/pistolmark2.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("energy gun (weapon)", "Image/energygun.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("rusted sword (weapon)", "Image/rustedsword.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("metal sword (weapon)", "Image/metalsword.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("photon sword (weapon)", "Image/photonsword.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	//placeable blocks
	cII = cIM->Add("WoodenBlock (block)", "Image/Scene2D_GroundTile.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("rustedwoodPLUS (block)", "Image/rustedwoodPLUS.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("upgradealtar", "Image/upgradealtar.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("metalcubePLUS (block)", "Image/metalcubePLUS.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	//environmental blocks
	cII = cIM->Add("rustedtree", "Image/rustedtree.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("steelpile", "Image/steelpile.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	//background blocks
	cII = cIM->Add("rustedwood", "Image/rustedwood.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("metalcube", "Image/metalcube.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("firepowder", "Image/gunpowder.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("metalparts", "Image/metalparts.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("ironhorn", "Image/ironhorn.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("oilcan", "Image/oilcan.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("ghensheart", "Image/ghensheart.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("portal (block)", "Image/Scene2D_PI.tga", 999, 0);
	cII->vec2Size = glm::vec2(25, 25);

}

void CPlayer2D::selectKey()
{
	if (cKeyboardController->IsKeyDown(GLFW_KEY_1))
	{
		select = 1;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_2))
	{
		select = 2;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_3))
	{
		select = 3;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_4))
	{
		select = 4;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_5))
	{
		select = 5;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_6))
	{
		select = 6;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_7))
	{
		select = 7;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_8))
	{
		select = 8;
	}
	else if (cKeyboardController->IsKeyDown(GLFW_KEY_9))
	{
		select = 9;
	}
	equip = hotKeyInv[select - 1];
	dmg = 1;
	gunDmg = 0;
	defaultRate = 0;
	string checker = equip;
	std::transform(checker.begin(), checker.end(), checker.begin(), ::tolower);
	if (checker.find("weapon") != string::npos)
	{
		Wepon(checker);
	}
}
float CPlayer2D::getGunDmg()
{
	return gunDmg;
}
void CPlayer2D::Wepon(string wepon)
{
	if (wepon.find("mark1") != string::npos)
	{
		dmg = 1;
		gunDmg = 2;
		defaultRate = 2;
	}

	if (wepon.find("mark2") != string::npos)
	{
		dmg = 1;
		gunDmg = 4;
		defaultRate = 1;
	}

	if (wepon.find("energy") != string::npos)
	{
		dmg = 1;
		gunDmg = 6;
		defaultRate = 0.5;
	}

	if (wepon.find("rustedsword") != string::npos)
	{
		dmg = 2;
		gunDmg = 0;
		defaultRate = 1;
	}

	if (wepon.find("metalsword") != string::npos)
	{
		dmg = 3;
		gunDmg = 0;
		defaultRate = 0.7;
	}

	if (wepon.find("photon") != string::npos)
	{
		dmg = 8;
		gunDmg = 0;
		defaultRate = 0.4;
	}

}

void CPlayer2D::MouseAction()
{
	float posX = cMouseController->GetMousePositionX() / cSettings->iWindowWidth * 32; //convert (0,800) to (0,80)
	float posY = 24 - (cMouseController->GetMousePositionY() / cSettings->iWindowHeight * 24);
	glm::vec2 mousePos(posX, posY);
	if (cMouseController->IsButtonDown(1))
	{
		
		string checker = equip;
		std::transform(checker.begin(), checker.end(), checker.begin(), ::tolower);

		if (checker.find("oilcan") != string::npos)
		{	
			cII = cIM->GetItem("Fuel");
			if (cII->GetCount() < cII->GetMaxCount())
			{

				cII = cIM->GetItem(equip);
				cII->Remove(1);
				cII = cIM->GetItem("Fuel");
				cII->Add(1);
				CSC->PlaySoundByID(6);
				hotKeyInvQuantity[select - 1]--;
				if (hotKeyInvQuantity[select - 1] == 0)
				{
					hotKeyInv[select - 1] = "";
					hotKeyInvID[select - 1] = 0;
					equip = hotKeyInv[select - 1];
				}
			}
		}
		else if ((checker.find("mark1") != string::npos)|| (checker.find("mark2") != string::npos) || (checker.find("energy") != string::npos))
		{
			//shooting
			if (fireRate <= 0)
			{

				shooting = true;
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 372);
				bullet* p = new bullet();
				p->SetShader("Shader2D_Colour");
				p->Init();
				p->player = true;
				p->des = mousePos;
				pBullet.push_back(p);

				CSC->PlaySoundByID(5);
				fireRate = defaultRate;
			}
		}

	}
}

void CPlayer2D::addToinventory(int num,string name,int amt,int maxQuitity)
{
	//cout << hotKeyInv.size() << endl;
	////check For existence
	//for (size_t i = 0; i < hotKeyInv.size(); i++)
	//{
	//	if (hotKeyInv[i] == name && hotKeyInvQuantity[i]<maxQuitity)
	//	{
	//		cII = cIM->GetItem(name);
	//		cII->Add(amt);
	//		hotKeyInvQuantity[i]++;
	//		changed = true;
	//		return;
	//	}
	//}
	//for (size_t i = 0; i < hotKeyInv.size(); i++)
	//{
	//	if (hotKeyInv[i] == "")
	//	{
	//		hotKeyInv[i] = name;
	//		hotKeyInvID[i] = num;	
	//		hotKeyInvQuantity[i]++;
	//		cII = cIM->GetItem(name);
	//		cII->Add(amt);
	//		changed = true;
	//
	//		return;
	//	}
	//}
	//cMap2D->SetMapInfo(vec2Index.y,vec2Index.x,(num+1000));
	//drop = true;



	static bool done = false;
	for (size_t j = 0; j < amt; j++)
	{
		done = false;
		//check For existence
		for (size_t i = 0; i < hotKeyInv.size(); i++)
		{
			if (hotKeyInv[i] == name && hotKeyInvQuantity[i] < maxQuitity)
			{
				cII = cIM->GetItem(name);
				cII->Add(1);
				hotKeyInvQuantity[i]++;
				changed = true;
				done = true;
				break;
			}
		}
		if (!done)
		{

			for (size_t i = 0; i < hotKeyInv.size(); i++)
			{
				if (hotKeyInv[i] == "")
				{
					hotKeyInv[i] = name;
					hotKeyInvID[i] = num;
					hotKeyInvQuantity[i]++;
					cII = cIM->GetItem(name);
					cII->Add(1);
					changed = true;
					done = true;

					break;
				}
			}
		}
		if (!done)
		{
			amtDrop++;
		}
	}
	if (amtDrop > 0)
	{
		DropId.push_back(std::make_pair(num+1000, amtDrop));
		amtDrop = 0;
	}
}

float CPlayer2D::getDmg()
{
	return dmg;
}



void CPlayer2D::resetStuff()
{

	vec2Index.y = 5;
	vec2Index.x = 1;
	vec2NumMicroSteps.x = 0;
	vec2NumMicroSteps.y = 0;


	bVec2.clear();
	bVec2Micro.clear();
	dirr.clear();
	bColor.clear();
	ID.clear();

	for (size_t i = 10; i <= 17; i++)
	{
		cMap2D->replaceSomething(i, 100);
	}
	Yflip = false;
	Rflip = false;
	foodBowl = false;

	cII = cIM->GetItem("KeyCard");
	cII->Remove(cII->GetCount());
	cMap2D->replaceSomething(31, 101);
	cMap2D->replaceSomething(32, 102);
}

int CPlayer2D::GetCall(int i)
{
	return CallAlly[i];
}

void CPlayer2D::SetbActive()
{
	bActive = true;
	CSC->PlaySoundByID(5);

	if (bActive)
	{
		bActive = false;
		return;
	}
}

bool CPlayer2D::GetbActive()
{
	return bActive;
}

void CPlayer2D::AssignAmtOfDog(int amt)
{
	for (size_t i = 0; i < amt; i++)
	{
		CallAlly.push_back(0);
	}
}

std::vector<string> CPlayer2D::GetHotKeyInv()
{
	return hotKeyInv;
}

std::vector<int> CPlayer2D::GetHotKeyQuitity()
{
	return hotKeyInvQuantity;
}

std::vector<int> CPlayer2D::GetHotKeyid()
{
	return hotKeyInvID;
}

void CPlayer2D::setHotKeyInventory(std::vector<string> name, std::vector<int> id, std::vector<int> quan)
{
	hotKeyInv = name;
	hotKeyInvID = id;
	hotKeyInvQuantity = quan;
}

int CPlayer2D::GetSelect()
{
	return select-1;
}

int CPlayer2D::getSelected()
{
	return select;
}

bool CPlayer2D::getIframe()
{
	return iFrame;
}

void CPlayer2D::SetIframe()
{
	CSC->PlaySoundByID(2);
	iFrame = true;
}

void CPlayer2D::fuelTime()
{
	
	cII = cIM->GetItem("Fuel");
	

	if (cII->GetCount() == 0)
	{
		setHealth(2);
	}
	else
	{
		cII->Remove(1);
	}
}

void CPlayer2D::setHealth(int damage)
{
	cII = cIM->GetItem("Health");
	cII->Remove(damage);
	if (cII->GetCount() <= 0)
	{
		CGameManager::GetInstance()->bPlayerLost = true;
	}
}
