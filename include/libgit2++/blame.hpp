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
#include <experimental/optional>
#include <git2/blame.h>
#include <memory>


namespace git2pp {
	enum class blame_flags {
		normal                  = GIT_BLAME_NORMAL,
		intrafile_line_moves    = GIT_BLAME_TRACK_COPIES_SAME_FILE,
		intracommit_line_moves  = GIT_BLAME_TRACK_COPIES_SAME_COMMIT_MOVES,
		intracommit_line_copies = GIT_BLAME_TRACK_COPIES_SAME_COMMIT_COPIES,
		all_commit_line_copies  = GIT_BLAME_TRACK_COPIES_ANY_COMMIT_COPIES,
		first_parent            = GIT_BLAME_FIRST_PARENT,
	};

	constexpr blame_flags operator&(blame_flags lhs, blame_flags rhs) noexcept;
	constexpr blame_flags operator|(blame_flags lhs, blame_flags rhs) noexcept;


	class blame_options {
	public:
		blame_flags flags;
		std::uint16_t min_match_characters;

		git_oid newest_commit;
		git_oid oldest_commit;

		std::size_t min_line;
		std::size_t max_line;

		blame_options() noexcept;
		blame_options(const git_blame_options & opts) noexcept;
		operator git_blame_options() const noexcept;
	};


	class blame_deleter {
	public:
		bool owning;

		void operator()(git_blame * blm) const noexcept;
	};


	class blame : public guard {
	public:
		std::uint32_t size() noexcept;

		const git_blame_hunk & operator[](std::uint32_t idx) noexcept;
		const git_blame_hunk & at_line(std::uint32_t line_number) noexcept;

		blame buffer(blame & ref, const char * buf, std::size_t buffer_len) noexcept;
		blame buffer(blame & ref, const char * buf) noexcept;
		blame buffer(blame & ref, const std::string & buf, std::size_t buffer_len) noexcept;
		blame buffer(blame & ref, const std::string & buf) noexcept;

	private:
		friend class repository;

		blame(git_blame * blm, bool owning = true) noexcept;

		std::unique_ptr<git_blame, blame_deleter> blm;
	};
}


constexpr git2pp::blame_flags git2pp::operator&(git2pp::blame_flags lhs, git2pp::blame_flags rhs) noexcept {
	return static_cast<blame_flags>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

constexpr git2pp::blame_flags git2pp::operator|(git2pp::blame_flags lhs, git2pp::blame_flags rhs) noexcept {
	return static_cast<blame_flags>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}
