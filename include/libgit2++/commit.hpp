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


#include "commit.hpp"
#include "guard.hpp"
#include <git2/commit.h>
#include <git2/annotated_commit.h>
#include <memory>


namespace git2pp {
	class commit_deleter {
	public:
		bool owning;

		void operator()(git_commit * cmt) const noexcept;
	};

	class annotated_commit_deleter {
	public:
		bool owning;

		void operator()(git_annotated_commit * cmt) const noexcept;
	};


	class commit : public guard {
	public:
	private:
		friend class repository;

		commit(git_commit * cmt, bool owning = true) noexcept;

		std::unique_ptr<git_commit, commit_deleter> cmt;
	};

	class annotated_commit : public guard {
	public:
	private:
		friend class repository;

		annotated_commit(git_annotated_commit * cmt, bool owning = true) noexcept;

		std::unique_ptr<git_annotated_commit, annotated_commit_deleter> cmt;
	};
}
