#include <filesystem>
#include <iostream>
#include <vector>
#include "src/GenNode.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cout << "Usage: " << argv[0] << " [InputDirectory] [OutDirectory] "
				  << std::endl;
		return 0;
	}
	if (!fs::directory_entry(argv[1]).exists() ||
		!fs::directory_entry(argv[1]).is_directory())
	{
		std::cout << "Error: Second argument must be an existing directory"
				  << std::endl;
		return 0;
	}
	try
	{
		/* iterate recursively through each directory entry */
		for (const fs::directory_entry &dir_entry: fs::recursive_directory_iterator(
				argv[1]))
		{
				/* create an object for each directory entry and start generating process */
				GenNode(dir_entry, argv[1], argv[2]).commit();
		}
	}
	catch (fs::filesystem_error &error)
	{
		std::cout << "Error occurred while iterating directories " << std::endl
					<< "\t" << error.what() << std::endl;
		return 1;
	}
	return 0;
}
