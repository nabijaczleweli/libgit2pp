#include "libgit2++/repository.hpp"
#include <iostream>


int main(int, char ** argv) {
	std::cout << argv[0] << '\n';
	std::cout << git2pp::discover_repository(".") << '\n';
}
