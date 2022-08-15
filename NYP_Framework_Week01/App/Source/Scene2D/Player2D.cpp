/**
 Player2D
 @brief A class representing the player object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Player2D.h"

#include <iostream>
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

	cIM = CInventoryManager::GetInstance();
	cII = cIM->Add("Lives","Image/Scene2D_Lives.tga",3,3);
	cII->vec2Size = glm::vec2(25, 25);

	cII = cIM->Add("Health", "Image/Scene2D_Health.tga", 100, 100);
	cII->vec2Size = glm::vec2(25, 25);


	cII = cIM->Add("Food", "Image/dogFood.tga", 3, 3);
	cII->vec2Size = glm::vec2(25, 25);

	spawn = vec2Index;
	foodBowl = false;


	Yflip = false;
	Rflip = false;

	CSC = CSoundController::GetInstance();

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
	bool foundB = false;
	bool foundO = false;
	foodBowl = false;
	switch (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x))
	{
	case 20://food
		cII = cIM->GetItem("Food");
		cII->Add(1);
		cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 0);
		CSC->PlaySoundByID(1);

		break;
	case 10:
		cII = cIM->GetItem("Food");
		if (cII->GetCount() > 0)
		{
			foodBowl = 1;
		}
		if (cKeyboardController->IsKeyDown(GLFW_KEY_E))
		{
			cII = cIM->GetItem("Food");
			if (cII->GetCount() > 0)
			{
				cII->Remove(1);
				CSC->PlaySoundByID(3);
				cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 11);
			}
		}
		break;
	case 11:
	{
		foodBowl = 2;
		break;
	}
	default:
		break;
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
	static float RespawnTimer = 5;
	CGameManager::GetInstance()->timer += dElapsedTime;
	

	// Store the old position
	if (bActive ==false)
	{
		cII = cIM->GetItem("Food");
		cII->Remove(cII->GetCount());
		if (RespawnTimer <= 0)
		{
			RespawnTimer = 5;
			bActive = true;
		}
		vec2Index = spawn;
		RespawnTimer -= dElapsedTime;
		return;
	}

	vec2OldIndex = vec2Index;
	runtimeColour = glm::vec4(1.0f, 1.0, 0.0, 1.0f);
	static int dir=2;
	static int tid = 100;

	for (int i = 0; i < CallAlly.size(); i++)
	{
		if (CallAlly[i] == 1)
		{
			CallAlly[i] = 0;

		}
	}

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

	if (cKeyboardController->IsKeyPressed(GLFW_KEY_1))
	{
		CallAlly[0] = 1;
		CSC->PlaySoundByID(4);
	}
	else if (cKeyboardController->IsKeyPressed(GLFW_KEY_2))
	{

		CallAlly[1] = 1;
		CSC->PlaySoundByID(4);
	}


	InteractWithMap();

	
	if (cKeyboardController->IsKeyPressed(GLFW_KEY_Q) && (cMap2D->GetCurrentLevel()==3|| cMap2D->GetCurrentLevel() == 2))
	{
		CGameManager::GetInstance()->bGameToRestart = true;

		cII = cIM->GetItem("Lives");
		cII->Add(cII->GetMaxCount());

		cII = cIM->GetItem("KeyCard");
		cII->Remove(cII->GetCount());

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

int CPlayer2D::GetfoodBowl()
{

		return foodBowl;

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
