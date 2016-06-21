// The MIT License (MIT)

// Copyright (c) 2016 nabijaczleweli

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include "libgit2++/repository.hpp"
#include "catch.hpp"
#include "util.hpp"
#include <fstream>
#include <string>


void check_for_head(std::string head);


TEST_CASE("init() - non-bare", "[repository]") {
	const auto dir = git2pp::discover_repository(".") + "../out/test/repos/repository/init()/1";
	remove_directory(dir.c_str());

	const auto repo = git2pp::repository::init(dir);
	check_for_head(dir + "/.git/HEAD");
}

TEST_CASE("init() - bare", "[repository]") {
	const auto dir = git2pp::discover_repository(".") + "../out/test/repos/repository/init()/2.git";
	remove_directory(dir.c_str());

	const auto repo = git2pp::repository::init(dir, true);
	check_for_head(dir + "/HEAD");
}


void check_for_head(std::string head_path) {
	INFO(head_path);
	std::ifstream head(head_path);
	CHECK(head);
	CHECK(head.is_open());
}
