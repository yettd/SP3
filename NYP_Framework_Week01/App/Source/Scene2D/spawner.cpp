/**
 spawner
 @brief A class which represents the enemy object
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "spawner.h"

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
spawner::spawner(void)
	: bIsActive(false)
	, cMap2D(NULL)
	, cSettings(NULL)
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
spawner::~spawner(void)
{
	// Delete the quadMesh
	if (quadMesh)
	{
		delete quadMesh;
		quadMesh = NULL;
	}



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
bool spawner::Init(void)
{

	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Get the handler to the CMap2D instance
	cMap2D = CMap2D::GetInstance();
	// Find the indices for the player in arrMapInfo, and assign it to cPlayer2D
	unsigned int uiRow = -1;
	unsigned int uiCol = -1;
	//asign food bowl

	if (cMap2D->FindValue(99, uiRow, uiCol) == false)
	{ 
		return false;	// Unable to find the start position of the player, so quit this game
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
	iTextureID = CImageLoader::GetInstance()->LoadTextureGetID("Image/door.tga", true);
	if (iTextureID == 0)
	{
		//cout << "Unable to load Image/Scene2D_EnemyTile.tga" << endl;
		return false;
	}

	//CS: Init the color to white
	runtimeColour = glm::vec4(1.0, 1.0, 1.0, 1.0);

	// If this class is initialised properly, then set the bIsActive to true
	bIsActive = true;

	SetEscape();

	

	return true;
}

/**
 @brief Update this instance
 */
void spawner::Update(const double dElapsedTime)
{


	if (!bIsActive)
		return;

	static float timer = 20;





	if (timer <= 0)
	{
		timer = 20;
		SpawnSpeedMultiplyer += 100;
		if (SpawnSpeedMultiplyer > 300)
		{
			SpawnSpeedMultiplyer = 300;
		}
	} 

	timer -= dElapsedTime;
	switch (sCurrentFSM)
	{
	case IDLE:
		if (randomTimer == 0)
		{
			randomTimer = rand() % (200) + 300;
			randomTimer -= SpawnSpeedMultiplyer;
			cout << randomTimer << endl;

		}

		if (iFSMCounter > randomTimer)
		{
			sCurrentFSM = Spawn;
			iFSMCounter = 0;
			SpanweTime = true;
			//cout << "Switching to Patrol State" << endl;
		}

		iFSMCounter++;
		break;
	case Spawn:
	{
	
		if (iFSMCounter > 100)
		{
			randomTimer = 0;
			sCurrentFSM = IDLE;
			iFSMCounter = 0;
			//cout << "Switching to Patrol State" << endl;
		}
		iFSMCounter++;
		break;
	}

	default:
		break;
	}

	

	// Update the UV Coordinates
	vec2UVCoordinate.x = cSettings->ConvertIndexToUVSpace(cSettings->x, vec2Index.x, false, i32vec2NumMicroSteps.x*cSettings->MICRO_STEP_XAXIS);
	vec2UVCoordinate.y = cSettings->ConvertIndexToUVSpace(cSettings->y, vec2Index.y, false, i32vec2NumMicroSteps.y*cSettings->MICRO_STEP_YAXIS);


}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void spawner::PreRender(void)
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
void spawner::Render(void)
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
void spawner::PostRender(void)
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
void spawner::Seti32vec2Index(const int iIndex_XAxis, const int iIndex_YAxis)
{
	this->vec2Index.x = iIndex_XAxis;
	this->vec2Index.y = iIndex_YAxis;
}

/**
@brief Set the number of microsteps of the enemy2D
@param iNumMicroSteps_XAxis A const int variable storing the current microsteps in the X-axis
@param iNumMicroSteps_YAxis A const int variable storing the current microsteps in the Y-axis
*/
void spawner::Seti32vec2NumMicroSteps(const int iNumMicroSteps_XAxis, const int iNumMicroSteps_YAxis)
{
	this->i32vec2NumMicroSteps.x = iNumMicroSteps_XAxis;
	this->i32vec2NumMicroSteps.y = iNumMicroSteps_YAxis;
}

/**
 @brief Set the handle to cPlayer to this class instance
 @param cPlayer2D A CPlayer2D* variable which contains the pointer to the CPlayer2D instance
 */


void spawner::SetEscape()
{
	this->escapeDestination = this->vec2Index;
}


/**
 @brief Constraint the enemy2D's position within a boundary
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */


/**
 @brief Check if a position is possible to move into
 @param eDirection A DIRECTION enumerated data type which indicates the direction to check
 */

// Check if the enemy2D is in mid-air


void spawner::pathingShortCut(glm::vec2 des)
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




