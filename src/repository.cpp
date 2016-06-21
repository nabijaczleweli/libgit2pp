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
#include "libgit2++/detail/scope.hpp"
#include <git2/buffer.h>


void git2pp::repository_deleter::operator()(git_repository * repo) const noexcept {
	git_repository_free(repo);
}


git2pp::repository git2pp::repository::init(const char * path, bool bare) {
	guard grd;

	git_repository * result{};
	git_repository_init(&result, path, bare);
	return {result};
}

git2pp::repository git2pp::repository::init(const std::string & path, bool bare) {
	return init(path.c_str(), bare);
}

git2pp::repository git2pp::repository::open(const char * path) {
	guard grd;

	git_repository * result{};
	git_repository_open(&result, path);
	return {result};
}

git2pp::repository git2pp::repository::open(const std::string & path) {
	return open(path.c_str());
}

git2pp::repository::repository(git_repository * r) : repo(r) {}


std::string git2pp::discover_repository(const std::string & start, const std::string & ceiling_dirs, bool across_fs) {
	return discover_repository(start.c_str(), ceiling_dirs, across_fs);
}

std::string git2pp::discover_repository(const char * start, const std::string & ceiling_dirs, bool across_fs) {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_repository_discover(&buf, start, across_fs, ceiling_dirs.c_str());

	return buf.ptr;
}
