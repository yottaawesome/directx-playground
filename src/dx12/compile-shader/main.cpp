import std;

auto main(int argc, char* argv[]) -> int
{
	if (argc < 2)
	{
		std::cerr << "Usage: " << argv[0] << " <shader_file>" << std::endl;
		return 1;
	}

	return 0;
}
