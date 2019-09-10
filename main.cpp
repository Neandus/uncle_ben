#include <string>
#include <thread>
#include <chrono>
#include "bridge.h"


int main(int argc, char** argv)
{
	if(argc == 2)
	{
		std::string bridge_name = argv[1];
		while(1)
		{
			bridge_read(bridge_name.c_str(), stdout);

			std::this_thread::sleep_for(std::chrono::seconds(2));	
		}
	}
	return 0;
}