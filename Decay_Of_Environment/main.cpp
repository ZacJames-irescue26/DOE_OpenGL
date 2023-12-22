#include "pch.h"
#include "Engine/Engine.h"


//int main() {
//
//	int width = 800;
//	int height = 800;
//
//	auto g = new GameEngine;
//
//	/*returnCode nextAction = returnCode::CONTINUE;
//	while (nextAction == returnCode::CONTINUE) {
//		nextAction = g->mainLoop();
//	}*/
//
//	g->mainLoop();
//	return 0;
//}

extern GameEngine* CreateApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv)
{

	auto app = CreateApplication({ argc, argv });

	app->mainLoop();

	delete app;
}