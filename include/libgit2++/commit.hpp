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


#include "commit_tree.hpp"
#include "guard.hpp"
#include "signature.hpp"
#include <chrono>
#include <experimental/optional>
#include <git2/annotated_commit.h>
#include <git2/commit.h>
#include <memory>
#include <utility>


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


	class repository;

	class commit : public guard {
	public:
		const git_oid & id() const noexcept;
		repository owner() const noexcept;
		std::pair<std::chrono::time_point<std::chrono::system_clock>, int> time() const noexcept;

		const char * message() const noexcept;
		const char * message_raw() const noexcept;
		std::experimental::optional<std::string> message_summary() const;
		std::experimental::optional<std::string> message_body() const;
		std::experimental::optional<std::string> message_encoding() const;

		const git_signature & committer() const noexcept;
		const git_signature & author() const noexcept;

		commit_tree tree() const noexcept;
		const git_oid & tree_id() const noexcept;
		unsigned int parent_amount() const noexcept;

		commit parent(unsigned int n) const noexcept;
		std::experimental::optional<git_oid> parent_id(unsigned int n) const noexcept;
		commit ancestor(unsigned int n) const noexcept;

		std::string header_field(const char * field) const;
		std::string header_field(const std::string & field) const;

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
