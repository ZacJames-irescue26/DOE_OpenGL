#include "GameLayer.h"

class Game : public GameEngine
{
public:
	Game(const ApplicationSpecification& spec)
	{
		PushLayer(new Scene());
	}
};

GameEngine* CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Hazelnut";
	spec.CommandLineArgs = args;

	return new Game(spec);
}