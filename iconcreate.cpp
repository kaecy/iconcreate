#include "icon.h"

int main(int argc, char* argv[])
{
	if (argc > 1)
		Icon::createICOFromBMP(argv + 1, argc - 1);
	return 0;
}
