#include "application.hpp"
#include <iostream>

int main(int argc, char* argv[])
{
	democollection::SaveProgramFolder(argv[0]);

	democollection::Application app;

	try
	{
		app.Init(argc, argv);
		app.Run();
	}
	catch (const std::exception& ex)
	{
		std::cerr << ex.what() << std::endl;
	}
	return 0;
}
