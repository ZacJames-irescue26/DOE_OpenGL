
#include "EditorLayer.h"


class Editor : public GameEngine
{
public:
	Editor(const ApplicationSpecification& spec)
	{
		PushLayer(new EditorLayer());
	}
};

GameEngine* CreateApplication(ApplicationCommandLineArgs args)
{
	ApplicationSpecification spec;
	spec.Name = "Hazelnut";
	spec.CommandLineArgs = args;

	return new Editor(spec);
}