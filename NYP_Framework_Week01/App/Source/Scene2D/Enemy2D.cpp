/**
 CEnemy2D
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Enemy2D.h"

#include <iostream>
using namespace std;

// Include Shader Manager
#include "RenderControl\ShaderManager.h"
// Include Mesh Builder
#include "Primitives/MeshBuilder.h"

// Include GLEW
#include <GL/glew.h>

// Include ImageLoader
#include "System\ImageLoader.h"

// Include the Map2D as we will use it to check the player's movements and actions
#include "Map2D.h"
// Include math.h
#include <math.h>

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::CEnemy2D(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, cSettings(NULL)
	, cPlayer2D(NULL)
	, sCurrentFSM(FSM::IDLE)
	, iFSMCounter(0)
	, quadMesh(NULL)
{
	transform = glm::mat4(1.0f);	// make sure to initialize matrix to identity matrix first

	// Initialise vecIndex
	vec2Index = glm::i32vec2(0);

	// Initialise vecNumMicroSteps
	i32vec2NumMicroSteps = glm::i32vec2(0);

	// Initialise vec2UVCoordinate
	vec2UVCoordinate = glm::vec2(0.0f);

	i32vec2Destination = glm::i32vec2(0, 0);	// Initialise the iDestination
	i32vec2Direction = glm::i32vec2(0, 0);		// Initialise the iDirection
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CEnemy2D::~CEnemy2D(void)
{
	// Delete the quadMesh// Delete the quadMesh
	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}

	// We won't delete this since it was created elsewhere
	cPlayer2D = NULL;

	// We won't delete this since it was created elsewhere
	cMap2D = NULL;

	// optional: de-allocate all resources once they've outlived their purpose:
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

/**
  @brief Initialise this instance
  */
bool CEnemy2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	//if (cMap2D->FindValue(300, uiRow, uiCol) == false)
	//{
	//	return false;	// Unable to find the start position of the player, so quit this game
	//}

	if (cMap2D->FindValue(302, uiRow, uiCol) == false && enemyType == 0) // Unable to find the start position of the player, so quit this game
	{
		//Clifford
		enemyType = 1;
	}

	if (cMap2D->FindValue(301, uiRow, uiCol) == false && enemyType==1)
	{
		//Mechanical cow
		enemyType = 2;
	}

	 if (cMap2D->FindValue(400, uiRow, uiCol) == false && enemyType == 2)
	{
		//Iron unicorn
		enemyType = 3;
	}

	if (cMap2D->FindValue(401, uiRow, uiCol) == false && enemyType == 3)
	{
		enemyType = 4;
	}
	if (enemyType==4) // Unable to find the start position of the player, so quit this game
	{
		return false;
	}


	// Erase the value of the player in the arrMapInfo
	cMap2D->SetMapInfo(uiRow, uiCol, 0);

	// Set the start position of the Player to iRow and iCol
	vec2Index = glm::i32vec2(uiCol, uiRow);
	// By default, microsteps should be zero
	i32vec2NumMicroSteps = glm::i32vec2(0, 0);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//CS: Create the Quad Mesh using the mesh builder
	quadMesh = CMeshBuilder::GenerateQuad(glm::vec4(1, 1, 1, 1), cSettings->TILE_WIDTH, cSettings->TILE_HEIGHT);

	// Load the enemy2D texture
	//iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/robodog.tga", true);

	if (enemyType == 0)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/robodog.tga", true);

	}
	else if (enemyType == 1)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/Clifford.tga", true);
	}
	else if (enemyType == 2)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/mechanicalcow.tga", true);
	}
	else if (enemyType == 3)
	{
		iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/ironunicorn.tga", true);
	}
	if (iTextureID == 0)
	{
		cout << "Unable to load Image/Enemy.tga" << endl;
		return false;
	}

	if (iTextureID == 0)
	{
		//cout << "Unable to load Image/Scene2D_EnemyTile.tga" << endl;
		return false;
	}

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// Set the Physics to fall status by default
	cPhysics2D.Init();

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	rand_dir = rand() % 2; // 0 or 1



	return true;
}

/**
 @brief Update this instance
 */
void CEnemy2D::Update(const double dElapsedTime)
{
	if (!bIsActive)
		return;

	dt = dElapsedTime;

	/*cout << enemyHealth << endl;*/

	if (enemyHealth <= 0)
	{
		if (enemyType < 3)
		{
			cPlayer2D->enemies_unalived++;
		}

		if (enemyType == 0) //blues
		{
			cPlayer2D->addToinventory(13,"metalparts", rand() % 2 + 1, 10);
		}

		else if (enemyType == 1) //clifford
		{
			cPlayer2D->addToinventory(12, "firepowder", rand() % 2 + 1, 10);
		}

		else if (enemyType == 2) //cow
		{
			cPlayer2D->addToinventory(15, "oilcan", 1, 1);
		}

		else //unicorn
		{
			cPlayer2D->addToinventory(14, "ironhorn", 1, 10);
		}

		bIsActive = false;
	}

	

	if (enemyType == 0) // FSM for blues
	{
		switch (sCurrentFSM)
		{
		case IDLE:
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				iFSMCounter = 0;
				//cout << "Switching to Patrol State" << endl;
			}
			if (chase == true)
			{
				sCurrentFSM = HUNT;
			}
		}
		iFSMCounter++;
		break;

		case HUNT:
		{
			iFSMCounter = 0;
			//cout << "Switching to Patrol State" << endl;
			ShortCutPath(cPlayer2D->vec2Index);
			UpdatePosition();
		}
		iFSMCounter++;
		break;

		default:
			break;
		}
	}

	if (enemyType == 1) //FSM for Clifford
	{
		switch (sCurrentFSM)
		{
		case IDLE:
		{
			if (iFSMCounter > iMaxFSMCounter)
			{
				iFSMCounter = 0;
				//cout << "Switching to Patrol State" << endl;
			}
			if (chase == true)
			{
				if (rand_dir == 0)
				{
					sCurrentFSM = TARGETX;
				}
				else
				{
					sCurrentFSM = TARGETY;
				}
			}
		}
		iFSMCounter++;
		break;

		case TARGETX:
		{
			iFSMCounter = 0;
			//cout << "Switching to Patrol State" << endl;
			glm::vec2 rundes; //run destination
			rundes.x = cPlayer2D->vec2Index.x;
			rundes.y = vec2Index.y;
			ShortCutPath(rundes);
			if (vec2Index.x == cPlayer2D->vec2Index.x)
			{
				targetLocked = true;
			}
			else
			{
				targetLocked = false;
			}
			UpdatePosition();
		}
		iFSMCounter++;
		break;

		case TARGETY:
		{
			iFSMCounter = 0;
			//cout << "Switching to Patrol State" << endl;
			glm::vec2 rundes; //run destination
			rundes.y = cPlayer2D->vec2Index.y;
			rundes.x = vec2Index.x;
			ShortCutPath(rundes);
			if (vec2Index.y == cPlayer2D->vec2Index.y)
			{
				targetLocked = true;
			}
			else
			{
				targetLocked = false;
			}
			UpdatePosition();
		}
		iFSMCounter++;
		break;

		default:
			break;
		}
	}

	if (enemyType == 2 || enemyType == 3) // FSM for mechanical cow & iron unicorn
	{
		switch (sCurrentFSM)
		{
		case IDLE:
		{
				iFSMCounter = 0;
				walktimer += dElapsedTime;
				if (walktimer > 0.3f)
				{
					int random_walk_direction = rand() % 4;
					
					if (random_walk_direction == 1)
					{
						i32vec2Direction.y = 1;
						i32vec2Direction.x = 0;
					}
					else if (random_walk_direction == 2)
					{
						i32vec2Direction.y = 0;
						i32vec2Direction.x = 1;
					}
					else if (random_walk_direction == 3)
					{
						i32vec2Direction.y = -5;
						i32vec2Direction.x = 1;
					}
					else
					{
						i32vec2Direction.y = 0;
						i32vec2Direction.x = -1;
					}
					walktimer = 0.0f;
					UpdatePosition();
				}
				

			
		}
		iFSMCounter++;
		break;

		

		default:
			break;
		}
	}

	InteractWithPlayer();
	// Update Jump or Fall
	UpdateJumpFall(dElapsedTime);

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, i32vec2NumMicroSteps.x * cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, i32vec2NumMicroSteps.y * cSettings->MICRO_STEP_YAXIS);
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CEnemy2D::PreRender(void)
{
	if (!bIsActive)
		return;

	// bind textures on corresponding texture units
	glActiveTexture(GL_TEXTURE0);

	// Activate blending mode
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Activate the shader
	CShaderManager::GetInstance()->Use(sShaderName);
}

/**
 @brief Render this instance
 */
void CEnemy2D::Render(void)
{
	if (!bIsActive)
		return;

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

	// Get the texture to be rendered
	glBindTexture(GL_TEXTURE_2D, iTextureID);

	// Render the tile
	//glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	quadMesh->Render();

	glBindVertexArray(0);

}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CEnemy2D::PostRender(void)
{
	if (!bIsActive)
		return;

	// Disable blending
	glDisable(GL_BLEND);
}

/**
@brief Set the indices of the enemy2D
@param iIndex_XAxis A const int variable which stores the index in the x-axis
@param iIndex_YAxis A const int variable which stores the index in the y-axis
*/
void CEnemy2D::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->vec2Index.x = iIndex_XAxis;
	this->vec2Index.y = iIndex_YAxis;
}

/**
@brief Set the number of microsteps of the enemy2D
@param iNumMicroSteps_XAxis A const int variable storing the current microsteps in the X-axis
@param iNumMicroSteps_YAxis A const int variable storing the current microsteps in the Y-axis
*/
void CEnemy2D::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis)
{
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
}

/**
 @brief Set the handle to cPlayer to this class instance
 @param cPlayer2D A CPlayer2D* variable which contains the pointer to the CPlayer2D instance
 */
void CEnemy2D::SetPlayer2D(CPlayer2D* cPlayer2D)
{
	this->cPlayer2D = cPlayer2D;

	// Update the enemy's direction
	UpdateDirection();
}



void CEnemy2D::SetHp(int i)
{
	hp -= i;
}


/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
void CEnemy2D::Constraint(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		if (vec2Index.x < 0)
		{
			vec2Index.x = 0;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == RIGHT)
	{
		if (vec2Index.x >= (int)cSettings->NUM_TILES_XAXIS - 1)
		{
			vec2Index.x = ((int)cSettings->NUM_TILES_XAXIS) - 1;
			i32vec2NumMicroSteps.x = 0;
		}
	}
	else if (eDirection == UP)
	{
		if (vec2Index.y >= (int)cSettings->NUM_TILES_YAXIS - 1)
		{
			vec2Index.y = ((int)cSettings->NUM_TILES_YAXIS) - 1;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else if (eDirection == DOWN)
	{
		if (vec2Index.y < 0)
		{
			vec2Index.y = 0;
			i32vec2NumMicroSteps.y = 0;
		}
	}
	else
	{
		//cout << "CEnemy2D::Constraint: Unknown direction." << endl;
	}
}

/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */
bool CEnemy2D::CheckPosition(DIRECTION eDirection)
{
	if (eDirection == LEFT)
	{
		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100))
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
			i32vec2NumMicroSteps.x = 0;
			return true;
		}

		// If the new position is fully within a row, then check this row only
		if (i32vec2NumMicroSteps.y == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 rows, then check both rows as well
		else if (i32vec2NumMicroSteps.y != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
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
			i32vec2NumMicroSteps.y = 0;
			return true;
		}

		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y + 1, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else if (eDirection == DOWN)
	{
		// If the new position is fully within a column, then check this column only
		if (i32vec2NumMicroSteps.x == 0)
		{
			// If the grid is not accessible, then return false
			if (cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100)
			{
				return false;
			}
		}
		// If the new position is between 2 columns, then check both columns as well
		else if (i32vec2NumMicroSteps.x != 0)
		{
			// If the 2 grids are not accessible, then return false
			if ((cMap2D->GetMapInfo(vec2Index.y, vec2Index.x) >= 100) ||
				(cMap2D->GetMapInfo(vec2Index.y, vec2Index.x + 1) >= 100))
			{
				return false;
			}
		}
	}
	else
	{
		//cout << "CEnemy2D::CheckPosition: Unknown direction." << endl;
	}

	return true;
}

// Check if the enemy2D is in mid-air
bool CEnemy2D::IsMidAir(void)
{
	// if the player is at the bottom row, then he is not in mid-air for sure
	if (vec2Index.y == 0)
		return false;

	// Check if the tile below the player's current position is empty
	if ((i32vec2NumMicroSteps.x == 0) &&
		(cMap2D->GetMapInfo(vec2Index.y - 1, vec2Index.x) == 0))
	{
		return true;
	}

	return false;
}

// Update Jump or Fall
void CEnemy2D::UpdateJumpFall(const double dElapsedTime)
{
	if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.SetTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS); //DIsplacement divide by distance for 1 microstep
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			i32vec2NumMicroSteps.y += iDisplacement_MicroSteps;
			if (i32vec2NumMicroSteps.y > cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				i32vec2NumMicroSteps.y -= cSettings->NUM_STEPS_PER_TILE_YAXIS;
				if (i32vec2NumMicroSteps.y < 0)
					i32vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(UP);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop jump if so.
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i <= iIndex_YAxis_Proposed; i++)
		{
			// Change the player's index to the current i value
			vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(UP) == false)
			{
				// Align with the row
				i32vec2NumMicroSteps.y = 0;
				// Set the Physics to fall status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
				break;
			}
		}

		// If the player is still jumping and the initial velocity has reached zero or below zero, 
		// then it has reach the peak of its jump
		if ((cPhysics2D.GetStatus() == CPhysics2D::STATUS::JUMP) && (cPhysics2D.GetInitialVelocity().y <= 0.0f))
		{
			// Set status to fall
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}
	}
	else if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::FALL)
	{
		// Update the elapsed time to the physics engine
		cPhysics2D.SetTime((float)dElapsedTime);
		// Call the physics engine update method to calculate the final velocity and displacement
		cPhysics2D.Update();
		// Get the displacement from the physics engine
		glm::vec2 v2Displacement = cPhysics2D.GetDisplacement();

		// Store the current vec2Index.y
		int iIndex_YAxis_OLD = vec2Index.y;

		// Translate the displacement from pixels to indices
		int iDisplacement_MicroSteps = (int)(v2Displacement.y / cSettings->MICRO_STEP_YAXIS);

		if (vec2Index.y >= 0)
		{
			i32vec2NumMicroSteps.y -= fabs(iDisplacement_MicroSteps);
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}

		// Constraint the player's position within the screen boundary
		Constraint(DOWN);

		// Iterate through all rows until the proposed row
		// Check if the player will hit a tile; stop fall if so.
		int iIndex_YAxis_Proposed = vec2Index.y;
		for (int i = iIndex_YAxis_OLD; i >= iIndex_YAxis_Proposed; i--)
		{
			// Change the player's index to the current i value
			vec2Index.y = i;
			// If the new position is not feasible, then revert to old position
			if (CheckPosition(DOWN) == false)
			{
				// Revert to the previous position
				if (i != iIndex_YAxis_OLD)
					vec2Index.y = i + 1;
				// Set the Physics to idle status
				cPhysics2D.SetStatus(CPhysics2D::STATUS::IDLE);
				i32vec2NumMicroSteps.y = 0;
				break;
			}
		}
	}
}

/**
 @brief Let enemy2D interact with the player.
 */
 //				

bool CEnemy2D::InteractWithPlayer(void)
{
	glm::i32vec2 i32vec2PlayerPos = cPlayer2D->vec2Index;

	// Check if the enemy2D is within 6.5 indices of the player2D
	if (((vec2Index.x >= i32vec2PlayerPos.x - 6.5) &&
		(vec2Index.x <= i32vec2PlayerPos.x + 6.5))
		&&
		((vec2Index.y >= i32vec2PlayerPos.y - 6.5) &&
			(vec2Index.y <= i32vec2PlayerPos.y + 6.5)))
	{
		//	cout << "Gotcha!" << endl;
		chase = true;
		//cPlayer2D->SetbActive();
		// Since the player has been caught, then reset the FSM
		iFSMCounter = 0;
	}

	for (size_t i = 0; i < watchout.size(); i++)
	{
		if (((vec2Index.x >= watchout[i]->vec2Index.x - .5) &&
			(vec2Index.x <= watchout[i]->vec2Index.x + .5))
			&&
			((vec2Index.y >= watchout[i]->vec2Index.y - .5) &&
				(vec2Index.y <= watchout[i]->vec2Index.y + .5)))
		{
			enemyHealth -= cPlayer2D->getGunDmg();
			watchout[i]->bIsActive = false;
		}

	}


	//if enemy is Blues, deal dmg to player when in range :D
	if (enemyType == 0)
	{
		if (((vec2Index.x >= i32vec2PlayerPos.x - 2) &&
			(vec2Index.x <= i32vec2PlayerPos.x + 2))
			&&
			((vec2Index.y >= i32vec2PlayerPos.y - 2) &&
				(vec2Index.y <= i32vec2PlayerPos.y + 2)))
		{
			if (cPlayer2D->getIframe() == false)
			{
				cPlayer2D->SetIframe();
				cPlayer2D->setHealth(10);
			}
		}
	}

	if (targetLocked == true)
	{
		firerate += dt;
		if (firerate >= 0.3)
		{
			cMap2D->SetMapInfo(vec2Index.y, vec2Index.x, 372);
			bullet* p = new bullet();
			p->SetShader("Shader2D_Colour");
			p->Init();
			p->des = cPlayer2D->vec2Index;
			eBullet.push_back(p);
			firerate = 0.f;
		}
	}

	float posX = CMouseController::GetInstance()->GetMousePositionX() / cSettings->iWindowWidth * 32; //convert (0,800) to (0,80)
	float posY = 24 - (CMouseController::GetInstance()->GetMousePositionY() / cSettings->iWindowHeight * 24);
	glm::vec2 mousePos(posX, posY);

	if ((mousePos.x > 0 && mousePos.x < cSettings->NUM_TILES_XAXIS - 1) && (mousePos.y > 0 && mousePos.y < cSettings->NUM_TILES_YAXIS - 1))
	{
		if (cPhysics2D.CalculateDistance(cPlayer2D->vec2Index, mousePos) <= 2)
		{
			if (((vec2Index.x >= mousePos.x - 2) &&
				(vec2Index.x <= mousePos.x + 2))
				&&
				((vec2Index.y >= mousePos.y - 2) &&
					(vec2Index.y <= mousePos.y + 2)))
			{
				if (CMouseController::GetInstance()->IsButtonDown(0))
				{
					enemyHealth -= cPlayer2D->getDmg();
				}
				if (CMouseController::GetInstance()->IsButtonDown(0) && cowplscome == true)
				{
					enemyHealth -= cPlayer2D->getDmg();
				}
			}

		}
	}


	return false;
}

/**
 @brief Update the enemy's direction.
 */
void CEnemy2D::UpdateDirection(void)
{
	// Set the destination to the player
	i32vec2Destination = cPlayer2D->vec2Index;

	// Calculate the direction between enemy2D and player2D
	i32vec2Direction = i32vec2Destination - vec2Index;

	// Calculate the distance between enemy2D and player2D
	float fDistance = cPhysics2D.CalculateDistance(vec2Index, i32vec2Destination);
	if (fDistance >= 0.01f)
	{
		// Calculate direction vector.
		// We need to round the numbers as it is easier to work with whole numbers for movements
		i32vec2Direction.x = (int)round(i32vec2Direction.x / fDistance);
		i32vec2Direction.y = (int)round(i32vec2Direction.y / fDistance);
	}
	else
	{
		// Since we are not going anywhere, set this to 0.
		i32vec2Direction = glm::i32vec2(0);
	}
}

/**
 @brief Flip horizontal direction. For patrol use only
 */
void CEnemy2D::FlipHorizontalDirection(void)
{
	i32vec2Direction.x *= -1;
}

/**
@brief Update position.
*/
void CEnemy2D::UpdatePosition(void)
{
	// Store the old position
	i32vec2OldIndex = vec2Index;

	// if the player is to the left or right of the enemy2D, then jump to attack
	if (i32vec2Direction.x < 0)
	{
		// Move left
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x >= 0)
		{
			i32vec2NumMicroSteps.x -= speed;
			if (i32vec2NumMicroSteps.x < 0)
			{
				i32vec2NumMicroSteps.x = ((int)cSettings->NUM_STEPS_PER_TILE_XAXIS) - 1;
				vec2Index.x--;
			}
		}

		// Constraint the enemy2D's position within the screen boundary

		Constraint(LEFT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(LEFT) == false)
		{
			i32vec2NumMicroSteps.y = 0;

			vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}



		// Interact with the Player
		InteractWithPlayer();
	}
	else if (i32vec2Direction.x > 0)
	{
		// Move right
		const int iOldIndex = vec2Index.x;
		if (vec2Index.x < (int)cSettings->NUM_TILES_XAXIS)
		{
			i32vec2NumMicroSteps.x += speed;

			if (i32vec2NumMicroSteps.x >= cSettings->NUM_STEPS_PER_TILE_XAXIS)
			{
				i32vec2NumMicroSteps.x = 0;
				vec2Index.x++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(RIGHT);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(RIGHT) == false)
		{
			i32vec2NumMicroSteps.y = 0;

			//vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.x = 0;
		}

		// Check if enemy2D is in mid-air, such as walking off a platform
		//if (IsMidAir() == true)
		//{
		//	cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		//}

		// Interact with the Player
		InteractWithPlayer();
	}

	// if the player is above the enemy2D, then jump to attack
	if (i32vec2Direction.y > 0)
	{
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y < (int)cSettings->NUM_TILES_YAXIS)
		{
			i32vec2NumMicroSteps.y += speed;

			if (i32vec2NumMicroSteps.y >= cSettings->NUM_STEPS_PER_TILE_YAXIS)
			{
				i32vec2NumMicroSteps.y = 0;
				vec2Index.y++;
			}
		}

		// Constraint the enemy2D's position within the screen boundary
		Constraint(UP);

		// Find a feasible position for the enemy2D's current position
		if (CheckPosition(UP) == false)
		{
			i32vec2NumMicroSteps.x = 0;

			//vec2Index = i32vec2OldIndex;
			i32vec2NumMicroSteps.y = 0;
		}

		InteractWithPlayer();

		/*if (cPhysics2D.GetStatus() == CPhysics2D::STATUS::IDLE)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::JUMP);
			cPhysics2D.SetInitialVelocity(glm::vec2(0.0f, 3.5f));
		}*/
	}
	if (i32vec2Direction.y < 0)
	{
		const int iOldIndex = vec2Index.y;
		if (vec2Index.y >= 0)
		{
			i32vec2NumMicroSteps.y -= speed;
			if (i32vec2NumMicroSteps.y < 0)
			{
				i32vec2NumMicroSteps.y = ((int)cSettings->NUM_STEPS_PER_TILE_YAXIS) - 1;
				vec2Index.y--;
			}
		}
		// Constraint the enemy2D's position within the screen boundary

		Constraint(DOWN);

		//// Find a feasible position for the enemy2D's current position
		if (vec2Index.y != 0)
		{
			if (CheckPosition(DOWN) == false)
			{
				i32vec2NumMicroSteps.x = 0;

				vec2Index = i32vec2OldIndex;
				i32vec2NumMicroSteps.y = 0;
			}

		}
		// Check if enemy2D is in mid-air, such as walking off a platform
	/*	if (IsMidAir() == true)
		{
			cPhysics2D.SetStatus(CPhysics2D::STATUS::FALL);
		}*/

		// Interact with the Player
		InteractWithPlayer();
	}
}

void CEnemy2D::ShortCutPath(glm::vec2 des)
{
	auto path = cMap2D->PathFind(vec2Index,
		des,
		heuristic::euclidean,
		10);

	// Calculate new destination
	bool bFirstPosition = true;
	for (const auto& coord : path)
	{
		//std::cout << coord.x << "," << coord.y << "\n";
		if (bFirstPosition == true)
		{
			// Set a destination
			i32vec2Destination = coord;
			// Calculate the direction between enemy2D and this destination
			i32vec2Direction = i32vec2Destination - vec2Index;
			bFirstPosition = false;
		}
		else
		{
			if ((coord - i32vec2Destination) == i32vec2Direction)
			{
				// Set a destination
				i32vec2Destination = coord;
			}
			//else
			//	break;
		}
	}
}
