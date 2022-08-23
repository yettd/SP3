/**
 main.cpp
 @brief This file contains the main function for this project
 By: Toh Da Jun
 Date: Mar 2020
 */
#include "Application.h"
 
/**
 @brief This function is the main function which is called by the operating system when you run the executables
 @return This function returns the error codes
 */
int main(void)
{
	Application* pApp = Application::GetInstance();
	// if the application is initialised properly, then run it
	if (pApp->Init() == true)
	{
		// Run the Application instance
		pApp->Run();

		// Destroy the Application instance, just to be sure
		pApp->Destroy();

		// Set the Application handler to NULL
		pApp = NULL;

		// Return 0 since the application successfully ran
		return 0;
	}

	// Return 1 if the application failed to run
	return 1;
}
