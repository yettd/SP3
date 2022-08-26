// Include GLEW
#ifndef GLEW_STATIC
#include <GL/glew.h>
#define GLEW_STATIC
#endif

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#include <includes/glm.hpp>
#include <includes/gtc/matrix_transform.hpp>
#include <includes/gtc/type_ptr.hpp>

#include "death.h"

// Include CGameStateManager
#include "GameStateManager.h"

// Include Mesh Builder
#include "Primitives/MeshBuilder.h"
// Include ImageLoader
#include "System\ImageLoader.h"
// Include Shader Manager
#include "RenderControl\ShaderManager.h"

 // Include shader
#include "RenderControl\shader.h"

// Include CSettings
#include "GameControl/Settings.h"

// Include CKeyboardController
#include "Inputs/KeyboardController.h"

#include <iostream>
using namespace std;

/**
 @brief Constructor
 */
Cdeath::Cdeath(void)
	: background(NULL)
{

}

/**
 @brief Destructor
 */
Cdeath::~Cdeath(void)
{

}

/**
 @brief Init this class instance
 */
bool Cdeath::Init(void)
{
	cout << "Cdeath::Init()\n" << endl;

	CShaderManager::GetInstance()->Use("Shader2D");
	//CShaderManager::GetInstance()->activeShader->setInt("texture1", 0);

	//Create Background Entity
	background = new CBackgroundEntity("Image/MenuBackground.png");
	background->SetShader("Shader2D");
	background->Init();

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Setup Platform/Renderer bindings
	ImGui_ImplGlfw_InitForOpenGL(CSettings::GetInstance()->pWindow, true);
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Load the images for buttons
	CImageLoader* il = CImageLoader::GetInstance();
	Reply.fileName = "Image\\GUI\\retry.png";
	Reply.textureID = il->LoadTextureGetID(Reply.fileName.c_str(), false);

	text.fileName = "Image\\GUI\\dead.png";
	text.textureID = il->LoadTextureGetID(text.fileName.c_str(), false);

	exitButtonData.fileName = "Image\\GUI\\quit.png";
	exitButtonData.textureID = il->LoadTextureGetID(exitButtonData.fileName.c_str(), false);

	// Enable the cursor
	if (CSettings::GetInstance()->bDisableMousePointer == true)
		glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	return true;
}

/**
 @brief Update this class instance
 */
bool Cdeath::Update(const double dElapsedTime)
{

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoScrollbar;
	//window_flags |= ImGuiWindowFlags_MenuBar;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoCollapse;
	window_flags |= ImGuiWindowFlags_NoNav;

	float buttonWidth = 200;
	float buttonHeight = 50;
	float titleWidth = 560;
	float titleHeight = 91;

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Death", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth / 2.0 - titleWidth / 2.0,
			CSettings::GetInstance()->iWindowHeight / 5.0));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));

		//Added rounding for nicer effect
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 200.0f;
		ImGui::Image((ImTextureID)text.textureID,
			ImVec2(titleWidth, titleHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0));

		ImGui::End();

		// Create a window called "Hello, world!" and append into it.
		ImGui::Begin("Main Menu", NULL, window_flags);
		ImGui::SetWindowPos(ImVec2(CSettings::GetInstance()->iWindowWidth/2.0 - buttonWidth/2.0, 
			CSettings::GetInstance()->iWindowHeight/5.0 + 100));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(CSettings::GetInstance()->iWindowWidth, CSettings::GetInstance()->iWindowHeight));
		//ADD TEXT HERE


		////Added rounding for nicer effect
		//ImGuiStyle& style = ImGui::GetStyle();
		//style.FrameRounding = 200.0f;
		
		// Add codes for Start button here
		if (ImGui::ImageButton((ImTextureID)Reply.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			// Load the menu state
			cout << "Loading PlayGameState" << endl;
			CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
		}
	
		// Add codes for Exit button here
		if (ImGui::ImageButton((ImTextureID)exitButtonData.textureID,
			ImVec2(buttonWidth, buttonHeight), ImVec2(0.0, 0.0), ImVec2(1.0, 1.0)))
		{
			// Reset the CKeyboardController
			CKeyboardController::GetInstance()->Reset();

			// Load the menu state
			cout << "Quitting the game from MenuState" << endl;

			return false;
		}
		ImGui::End();

		const float relativeScale_x = CSettings::GetInstance()->iWindowWidth / 800.0f;
		const float relativeScale_y = CSettings::GetInstance()->iWindowHeight / 600.0f;


		if (relativeScale_x == 0 || relativeScale_y == 0)
		{
			return false;
		}
		ImGuiWindowFlags bb = ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoScrollbar;

	}

	//For keyboard controls
	if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_SPACE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PlayGameState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("PlayGameState");
		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ENTER))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Loading PlayGameState" << endl;
		CGameStateManager::GetInstance()->SetActiveGameState("Play3DGameState");
		return true;
	}
	else if (CKeyboardController::GetInstance()->IsKeyReleased(GLFW_KEY_ESCAPE))
	{
		// Reset the CKeyboardController
		CKeyboardController::GetInstance()->Reset();

		// Load the menu state
		cout << "Quitting the game from MenuState" << endl;
		return false;
	}

	return true;
}

/**
 @brief Render this class instance
 */
void Cdeath::Render(void)
{
	// Clear the screen and buffer
	glClearColor(0.0f, 0.55f, 1.00f, 1.00f);

	//Render Background
	background->Render();

	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	//cout << "Cdeath::Render()\n" << endl;
}

/**
 @brief Destroy this class instance
 */
void Cdeath::Destroy(void)
{
	// Disable the cursor
	if (CSettings::GetInstance()->bDisableMousePointer == true)
		glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else
		// Hide the cursor
		if (CSettings::GetInstance()->bShowMousePointer == false)
			glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// Delete the background
	if (background)
	{
		delete background;
		background = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	cout << "Cdeath::Destroy()\n" << endl;
}
