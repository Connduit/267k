#include "test.h"
#include "Client.h"
#include "ClientTest.h"
#include <iostream>



int main()
{
	std::cout << "inside main" << std::endl;
	ClientTest clientTest;
	clientTest.testAll();

	return 0;
}
