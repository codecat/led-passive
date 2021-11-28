#include <Common.h>
#include <Program.h>

int main()
{
	srand((unsigned int)time(nullptr));

	Program::instance = new Program;
	Program::instance->run();
	delete Program::instance;

	return 0;
}
