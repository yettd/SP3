/**
 CGUI_Scene2D
 @brief A class which manages the GUI for Scene2D
 By: Toh Da Jun
 Date: May 2021
 */
#include "GUI_Scene2D.h"

#include <iostream>
#include <sstream>

using namespace std;

/**
 @brief Constructor This constructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::CGUI_Scene2D(void)
	: cSettings(NULL)
	, m_fProgressBar(0.0f)
	, cInventoryManager(NULL)
	, cInventoryItem(NULL)
	, lever(NULL)
	, map(NULL)
{
}

/**
 @brief Destructor This destructor has protected access modifier as this class will be a Singleton
 */
CGUI_Scene2D::~CGUI_Scene2D(void)
{
	if (cInventoryManager)
	{
		cInventoryManager->Destroy();
		cInventoryManager = NULL;
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	// We won't delete this since it was created elsewhere
	cSettings = NULL;
}



/**
  @brief Initialise this instance
  */
bool CGUI_Scene2D::Init(void)
{
	// Get the handler to the CSettings instance
	cSettings = CSettings::GetInstance();

	// Store the CFPSCounter singleton instance here
	cFPSCounter = CFPSCounter::GetInstance();

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

	// Define the window flags
	window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	window_flags |= ImGuiWindowFlags_NoMove;
	window_flags |= ImGuiWindowFlags_NoResize;
	window_flags |= ImGuiWindowFlags_NoCollapse;

	//// Show the mouse pointer
	glfwSetInputMode(CSettings::GetInstance()->pWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// Initialise the cInventoryManager
	cInventoryManager = CInventoryManager::GetInstance();
	// Add a Tree as one of the inventory items


	// These variables are for IMGUI demo only
	show_demo_window = false;
	show_another_window = false;
	clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	m_fProgressBar = 0.0f;


	lever = CPlayer2D::GetInstance();
	map = CMap2D::GetInstance();
	int max = map->GetCountOfId(2);
	cInventoryItem = cInventoryManager->Add("KeyCard", "Image/Scene2D_TreeTile.tga", max, 0);
	cInventoryItem->vec2Size = glm::vec2(25, 25);
	return true;
}




/**
 @brief Update this instance
 */
void CGUI_Scene2D::Update(const double dElapsedTime)
{
	// Calculate the relative scale to our default windows width
	const float relativeScale_x = cSettings->iWindowWidth / 800.0f;
	const float relativeScale_y = cSettings->iWindowHeight / 600.0f;

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	if (relativeScale_x == 0 || relativeScale_y == 0)
	{
		return;
	}


	
	// Render a progress bar
	/*m_fProgressBar += 0.001f;
	if (m_fProgressBar > 1.0f)
		m_fProgressBar = 0.0f;
	ImVec4 col = ImVec4(0.0f, 0.0f, 1.0f, 1.0f);
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, col);
	col = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
		ImGui::PushStyleColor(ImGuiCol_FrameBg, col);
		ImGui::ProgressBar(cInventoryItem->GetCount() /
			(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f * relativeScale_x, 20.0f * relativeScale_y));
		ImGui::PopStyleColor();
	ImGui::PopStyleColor();*/

	// Render the inventory items
	//cInventoryItem = cInventoryManager->GetItem("Tree");
	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));  // Set a background color
	//ImGuiWindowFlags inventoryWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
	//	ImGuiWindowFlags_NoTitleBar |
	//	ImGuiWindowFlags_NoMove |
	//	ImGuiWindowFlags_NoResize |
	//	ImGuiWindowFlags_NoCollapse |
	//	ImGuiWindowFlags_NoScrollbar;
	//ImGui::Begin("Image", NULL, inventoryWindowFlags);
	//ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.03f, cSettings->iWindowHeight * 0.9f));
	//ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
	//ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
	//	ImVec2(cInventoryItem->vec2Size.x * relativeScale_x, cInventoryItem->vec2Size.y * relativeScale_y),
	//	ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::SameLine();
	//ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "Tree: %d / %d",
	//	cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
	//ImGui::End();
	//ImGui::PopStyleColor();


	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! 
// You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		ImGui::ShowDemoWindow(&show_demo_window);
		static float f = 0.0f;
		static int counter = 0;

		// Create a window called "Preview IMGUI" and append into it.
		ImGui::Begin("Preview IMGUI", NULL);
		ImGui::SetWindowPos(ImVec2(10.0f, 10.0f));				// Set the top-left of the window at (10,10)
		ImGui::SetWindowSize(ImVec2(300.0f, 200.0f));			// Set the window size to (300,200)

		ImGui::Text("Toggle IMGUI demo.");						// Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}



	//ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 1.0f, 0.0f));  // Set a background color

	//ImGuiWindowFlags dd = ImGuiWindowFlags_AlwaysAutoResize |
	//	ImGuiWindowFlags_NoTitleBar |
	//	ImGuiWindowFlags_NoMove |
	//	ImGuiWindowFlags_NoResize |
	//	ImGuiWindowFlags_NoCollapse |
	//	ImGuiWindowFlags_NoScrollbar;
	//ImGui::Begin("Food", NULL, dd);
	//ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.f, cSettings->iWindowHeight * 0.1f));
	//ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
	//cInventoryItem = cInventoryManager->GetItem("Food");
	//ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
	//	ImVec2(cInventoryItem->vec2Size.x * relativeScale_x,
	//		cInventoryItem->vec2Size.y * relativeScale_y),
	//	ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::SameLine();
	//ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	//ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d / %d",
	//	cInventoryItem->GetCount(), cInventoryItem->GetMaxCount());
	//ImGui::End(); 
	//ImGui::PopStyleColor();

	//hotkey
	
	cInventoryItem = cInventoryManager->GetItem("");


	ostringstream ss;
	
	PHK = CPlayer2D::GetInstance()->GetHotKeyInv();
	PHKQ = CPlayer2D::GetInstance()->GetHotKeyQuitity();
	PHKID = CPlayer2D::GetInstance()->GetHotKeyid();
	PS = CPlayer2D::GetInstance()->GetSelect();
	static float timer = 0.0f;

	if (CPlayer2D::GetInstance()->changed==true)
	{
		timer = 0.0001f;
		CPlayer2D::GetInstance()->changed = false;
	}


	PHKQ = CPlayer2D::GetInstance()->GetHotKeyQuitity();
	PHK = CPlayer2D::GetInstance()->GetHotKeyInv();
	PHKID = CPlayer2D::GetInstance()->GetHotKeyid();
	PS = CPlayer2D::GetInstance()->GetSelect();

	static float offset;



	
	 

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.66f, 0.66f, 0.66f, 1.f));  // Set a background color
	 ImGuiWindowFlags vv = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;


	for (size_t i = 0; i <9; i++)
	{
		if ((i+1) == CPlayer2D::GetInstance()->getSelected())
		{
			ImGui::PopStyleColor();

			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));  // Set a background color

		}
		ss << i;
		string a = ss.str();
		const char* b = a.c_str();
		if (timer <= 0)
		{
			ImGui::Begin(b, NULL, vv);
			ImGui::SetWindowPos(ImVec2((cSettings->iWindowWidth * 0.05f * i) + (cSettings->iWindowWidth * 0.3f), cSettings->iWindowHeight * .9f ));
			ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
			ImGui::SetWindowFontScale(1.f * relativeScale_y);
	
		
				cInventoryItem = cInventoryManager->GetItem(PHK[i]);

				if (ImGui::ImageButton((void*)(intptr_t)cInventoryItem->GetTextureID(),
					ImVec2(cInventoryItem->vec2Size.x * relativeScale_x * 0.80,
						cInventoryItem->vec2Size.y * relativeScale_y * 0.80),
					ImVec2(0, 1), ImVec2(1, 0)))
				{
					if (i != PS)
					{
						string tempname = PHK[i];
						int tempQuant = PHKQ[i];
						int tempid = PHKID[i];
						PHK[i] = PHK[PS];
						PHKQ[i] = PHKQ[PS];
						PHKID[i] = PHKID[PS];
						PHK[PS] = tempname;
						PHKQ[PS] = tempQuant;
						PHKID[PS] = tempid;
						CPlayer2D::GetInstance()->setHotKeyInventory(PHK, PHKID, PHKQ);

					}
				}
				if (PHK[i] != "")
				{
					ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", PHKQ[i]);
				}
				else
				{
					ImGui::TextColored(ImVec4(1, 1, 1, 1), "");
				}


			ImGui::End();
		}
			ImGui::PopStyleColor();
			ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.66f, 0.66f, 0.66f,1.f));  // Set a background color


	}
	ImGui::PopStyleColor();

	if (CMouseController::GetInstance()->GetMouseScrollStatus(CMouseController::GetInstance()->SCROLL_TYPE_YOFFSET) > 0 || CMouseController::GetInstance()->GetMouseScrollStatus(CMouseController::GetInstance()->SCROLL_TYPE_YOFFSET) < 0)
	{
		offset += CMouseController::GetInstance()->GetMouseScrollStatus(CMouseController::GetInstance()->SCROLL_TYPE_YOFFSET);
		CMouseController::GetInstance()->UpdateMouseScroll(0, -CMouseController::GetInstance()->GetMouseScrollStatus(CMouseController::GetInstance()->SCROLL_TYPE_YOFFSET));
		if (offset < 0)
		{
			offset = 0;
		}

	}


	static bool openInv = false;
	if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_E))
	{
		if (openInv)
		{
			openInv = false;
		}
		else
		{
			openInv = true;
			offset = 0;

		}
	}

	if (openInv)
	{
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.66f, 0.66f, 0.66f, 1.f));  // Set a background color


		int startingpoint = 9;
		for (size_t i = 0; i < 2; i++)
		{
			for (size_t j = 0; j < 9; j++)
			{

				ss << i;
				string a = ss.str();
				const char* b = a.c_str();
				if (timer <= 0)
				{
					ImGui::Begin(b, NULL, vv);
					ImGui::SetWindowPos(ImVec2((cSettings->iWindowWidth * 0.05f * j) + (cSettings->iWindowWidth * 0.3f), cSettings->iWindowHeight * 0.09f * i+ cSettings->iWindowHeight * .3f));
					ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
					ImGui::SetWindowFontScale(1.f * relativeScale_y);
					

						cInventoryItem = cInventoryManager->GetItem(PHK[startingpoint]);

						if (ImGui::ImageButton((void*)(intptr_t)cInventoryItem->GetTextureID(),
							ImVec2(cInventoryItem->vec2Size.x * relativeScale_x * 0.80,
								cInventoryItem->vec2Size.y * relativeScale_y * 0.80),
							ImVec2(0, 1), ImVec2(1, 0)))
						{
							if (startingpoint != PS)
							{
								string tempname = PHK[startingpoint];
								int tempQuant = PHKQ[startingpoint];
								int tempid = PHKID[startingpoint];
								PHK[startingpoint] = PHK[PS];
								PHKQ[startingpoint] = PHKQ[PS];
								PHKID[startingpoint] = PHKID[PS];
								PHK[PS] = tempname;
								PHKQ[PS] = tempQuant;
								PHKID[PS] = tempid;
								CPlayer2D::GetInstance()->setHotKeyInventory(PHK, PHKID, PHKQ);

							}
						}
						if (PHK[startingpoint] != "")
						{
						ImGui::TextColored(ImVec4(1, 1, 1, 1), "%d", PHKQ[startingpoint]);

						}
						else
						{
							ImGui::TextColored(ImVec4(1, 1, 1, 1), "");
						}


					ImGui::End();
				}

				startingpoint++;
			}
		}
		ImGui::PopStyleColor();
	}
	//crafting UI
	static float distanceGap = 0;
	if (openInv)
	{

		distanceGap = 0;
		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 0.f, 1.f));  // Set a background color
		for (size_t i = 0; i <nameID.size() ; i++)
		{
			if (craftable(i))
			{
				ImGui::PopStyleColor();

				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 1.f, 0.f, 1.f));  // Set a background color
			}
				ss << i;
				string a = ss.str();
				const char* b = a.c_str();
				if (timer <= 0)
				{
					distanceGap += 30.0f * relativeScale_y;
					ImGui::Begin(b, NULL, vv);
					ImGui::SetWindowPos(ImVec2((cSettings->iWindowWidth * 0.0f), cSettings->iWindowHeight * 0.09f * i + cSettings->iWindowHeight * .3f  + distanceGap-offset));
					ImGui::SetWindowSize(ImVec2(200.0f * relativeScale_x, 25.0f * relativeScale_y));
					ImGui::SetWindowFontScale(1.5f * relativeScale_y);
					ImGui::TextColored(ImVec4(1, 1, 1, 1),  nameID[i].first.c_str());
					cInventoryItem = cInventoryManager->GetItem(nameID[i].first);
					cout << i << " : " << endl;
					if (ImGui::ImageButton((void*)(intptr_t)cInventoryItem->GetTextureID(),
						ImVec2(cInventoryItem->vec2Size.x * relativeScale_x * 0.80,
							cInventoryItem->vec2Size.y * relativeScale_y * 0.80),
						ImVec2(0, 1), ImVec2(1, 0)))
					{
						if (craftable(i))
						{
							craft(i);
						}
					}
					ImGui::SameLine();
					ImGui::TextColored(ImVec4(1, 1, 1, 1), ": ");
					float NextLine = 0;
					for (size_t j = 0; j < recipie[i].size(); j++)
					{
						NextLine++;
						if (NextLine<3)
						{
							ImGui::SameLine();
						}
						else
						{
							NextLine = 0;
							distanceGap += 30.0f * relativeScale_y;
						}
						cInventoryItem = cInventoryManager->GetItem(recipie[i][j].first);
						ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
							ImVec2(cInventoryItem->vec2Size.x * relativeScale_x,
								cInventoryItem->vec2Size.y * relativeScale_y),
							ImVec2(0, 1), ImVec2(1, 0));
						ImGui::SameLine();
						ImGui::TextColored(ImVec4(1, 1, 1, 1), "x %d",recipie[i][j].second);
						
					}

					ImGui::End();
				}
				ImGui::PopStyleColor();
				ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1.f, 0.f, 0.f, 1.f));  // Set a background color

		}
		ImGui::PopStyleColor();
	}
	// Create an invisible window which covers the entire OpenGL window
	ImGui::Begin("Invisible window", NULL, window_flags);
	ImGui::SetWindowPos(ImVec2(0.0f, 0.0f));
	ImGui::SetWindowSize(ImVec2((float)cSettings->iWindowWidth, (float)cSettings->iWindowHeight));
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);

	// Display the FPS
	//ImGui::TextColored(ImVec4(1, 1, 0, 1), "FPS: %d", cFPSCounter->GetFrameRate());

	ImGui::TextColored(ImVec4(1, 1, 0, 1), "Time: %d :00", Clock);

	ImGui::End();


	cInventoryItem = cInventoryManager->GetItem("Fuel");
	ImGuiWindowFlags fuelWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Fuel", NULL, fuelWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.0f,
		cSettings->iWindowHeight * 0.1f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x* relativeScale_x,
			cInventoryItem->vec2Size.y* relativeScale_y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	ImGui::ProgressBar(cInventoryItem->GetCount() /
		(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f *
			relativeScale_x, 20.0f * relativeScale_y));
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();

	cInventoryItem = cInventoryManager->GetItem("Health");
	ImGuiWindowFlags healthWindowFlags = ImGuiWindowFlags_AlwaysAutoResize |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar;
	ImGui::Begin("Health", NULL, healthWindowFlags);
	ImGui::SetWindowPos(ImVec2(cSettings->iWindowWidth * 0.0f,
		cSettings->iWindowHeight * 0.03f));
	ImGui::SetWindowSize(ImVec2(100.0f * relativeScale_x, 25.0f * relativeScale_y));
	ImGui::SetWindowFontScale(1.5f * relativeScale_y);
	ImGui::Image((void*)(intptr_t)cInventoryItem->GetTextureID(),
		ImVec2(cInventoryItem->vec2Size.x* relativeScale_x,
			cInventoryItem->vec2Size.y* relativeScale_y),
		ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SameLine();
	ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
	ImGui::ProgressBar(cInventoryItem->GetCount() /
		(float)cInventoryItem->GetMaxCount(), ImVec2(100.0f *
			relativeScale_x, 20.0f * relativeScale_y));
	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::End();

	
	/*if (CKeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_Z))
	{
		if (craftable(0))
		{
			craft(0);
		}
	}
*/

	timer-=dElapsedTime;
}

void CGUI_Scene2D::setUI(bool e)
{
	PressE = e;
}

void CGUI_Scene2D::setClock(int i)
{
	Clock = i;
}
void CGUI_Scene2D::craft(int i)
{
	for (size_t j = 0; j < recipie[i].size(); j++)
	{
		int amtNeeded = recipie[i][j].second;
		int invFinder = -1;
		while (amtNeeded > 0)
		{
			invFinder++;
			if (PHK[invFinder] == recipie[i][j].first)//name same
			{
				amtNeeded -= PHKQ[invFinder];
				cInventoryItem = cInventoryManager->GetItem(recipie[i][j].first);
				cInventoryItem->Remove(PHKQ[invFinder]);
				PHKQ[invFinder] -= PHKQ[invFinder];

				if (amtNeeded < 0)
				{
					PHKQ[invFinder] -= amtNeeded;
					cInventoryItem = cInventoryManager->GetItem(recipie[i][j].first);
					cInventoryItem->Add(-amtNeeded);
					break;
				}
				else
				{

					PHKQ[invFinder] = 0;
					PHKID[invFinder] = 0;
					PHK[invFinder] = "";
				}


			}

		

		}
	}
	CPlayer2D::GetInstance()->setHotKeyInventory(PHK, PHKID, PHKQ);
	CPlayer2D::GetInstance()->addToinventory(nameID[i].second, nameID[i].first,1,maxAmt[i]);
}

bool CGUI_Scene2D::craftable(int i)
{
	for (size_t j = 0; j < recipie[i].size(); j++)
	{
		cInventoryItem = cInventoryManager->GetItem(recipie[i][j].first);
		if (cInventoryItem->GetCount() < recipie[i][j].second)
		{
			return false;
		}
	}

	return true;
}

/**
 @brief Set up the OpenGL display environment before rendering
 */
void CGUI_Scene2D::PreRender(void)
{
}

/**
 @brief Render this instance
 */
void CGUI_Scene2D::Render(void)
{
	// Rendering
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/**
 @brief PostRender Set up the OpenGL display environment after rendering.
 */
void CGUI_Scene2D::PostRender(void)
{
}
