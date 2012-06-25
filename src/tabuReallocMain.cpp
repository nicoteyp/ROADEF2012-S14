#include "tabuRealloc.h"
#define REALLOC_AUTHOR "TEYPAZ"
#define REALLOC_IDENTIFIER "S14"

int main(int argc, char * argv[])
{
	reallocTimer timer;
	std::ifstream file_data;
	std::ifstream file_solution;
	std::ofstream file_result;
	//SRandom(0);
	if (argc == 1)
	{
		timer.activate_for_time_sec(300);
		file_data.open("model_a1_1.txt");
		file_solution.open("assignment_a1_1.txt");
		file_result.open("solution_a1_1");
	}
	else
	{
		for (int i = 0; i < argc; ++i)
		{
			if (std::string(argv[i]) == "-t")
			{
				//std::cout << argv[i + 1]<<"\n";
				timer.activate_for_time_sec(atof(argv[i + 1])-0.5);
			}
			else if (std::string(argv[i]) == "-p")
			{
				//std::cout << argv[i + 1]<<"\n";
				file_data.open(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-i")
			{
				//std::cout << argv[i + 1]<<"\n";
				file_solution.open(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-o")
			{
				//std::cout << argv[i + 1]<<"\n";
				file_result.open(argv[i + 1]);
			}
			else if (std::string(argv[i]) == "-s")
			{
				//SRandom(atoi(argv[i + 1]));
			}
			else if (std::string(argv[i]) == "-name")
			{
				std::cout << REALLOC_IDENTIFIER << std::endl;
			}
			/*else if (std::string(argv[i]) == "-class")
			{
				nb_classes = atoi(argv[i + 1]);
				//std::cout << "nb_classes" <<nb_classes<<"\n";
			}
			else if (std::string(argv[i]) == "-update")
			{
				update = atoi(argv[i + 1]);
				//std::cout << "coeff" <<coeff<<"\n";
			}*/
		}
	}
	if (file_solution.is_open() && file_data.is_open())
	{
		tabuReallocInstance temp(timer);
		temp.loadFile(file_data);
		temp.loadInitialAffectation(file_solution);
		temp.search(file_result);
		file_result.close();
	}
	return 0;

}
