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


#pragma once


#include "guard.hpp"
#include <git2/repository.h>
#include <memory>
#include <string>


namespace git2pp {
	class repository_deleter {
	public:
		void operator()(git_repository * repo) const noexcept;
	};


	class repository : public guard {
	public:
		static repository init(const char * path, bool bare = false);
		static repository init(const std::string & path, bool bare = false);

		static repository open(const char * path);
		static repository open(const std::string & path);

	private:
		repository(git_repository * repo);

		std::unique_ptr<git_repository, repository_deleter> repo;
	};


	std::string discover_repository(const char * start, const std::string & ceiling_dirs = "", bool across_fs = true);
	std::string discover_repository(const std::string & start, const std::string & ceiling_dirs = "", bool across_fs = true);
}
