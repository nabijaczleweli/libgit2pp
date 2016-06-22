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


#include "configuration.hpp"
#include "guard.hpp"
#include "object.hpp"
#include "reference.hpp"
#include <experimental/optional>
#include <git2/repository.h>
#include <memory>
#include <string>
#include <utility>


namespace git2pp {
	class repository_deleter {
	public:
		void operator()(git_repository * repo) const noexcept;
	};


	class repository : public guard {
	public:
		static repository init(const char * path, bool bare = false) noexcept;
		static repository init(const std::string & path, bool bare = false) noexcept;

		static repository open(const char * path) noexcept;
		static repository open(const std::string & path) noexcept;


		reference head() noexcept;
		bool head_detached() noexcept;
		bool head_unborn() noexcept;

		bool empty() noexcept;
		bool bare() noexcept;
		std::string path();

		std::experimental::optional<std::string> working_directory();
		void working_directory(const std::string & path, bool update_gitlink) noexcept;
		void working_directory(const char * path, bool update_gitlink) noexcept;

		configuration config() noexcept;
		configuration config_snapshot() noexcept;

		std::string message();
		void remove_message() noexcept;

		void cleanup_state() noexcept;

		template <class F>
		void iterate_over_fetch_head(F && func);
		template <class F>
		void iterate_over_merge_head(F && func);

		git_oid hash_file(const char * path, object_type type, const char * filters = nullptr) noexcept;
		git_oid hash_file(const std::string & path, object_type type, std::experimental::optional<std::string> filters = std::experimental::nullopt) noexcept;

	private:
		repository(git_repository * repo) noexcept;

		std::unique_ptr<git_repository, repository_deleter> repo;
	};


	std::string discover_repository(const char * start, const std::string & ceiling_dirs = "", bool across_fs = true);
	std::string discover_repository(const std::string & start, const std::string & ceiling_dirs = "", bool across_fs = true);
}


template <class F>
void git2pp::repository::iterate_over_fetch_head(F && func) {
	git_repository_fetchhead_foreach(repo.get(), [](const char * ref_name, const char * remote_url, const git_oid * oid, unsigned int is_merge,
	                                                void * payload) -> int { return (*static_cast<F *>(payload))(ref_name, remote_url, oid, is_merge); },
	                                 &func);
}

template <class F>
void git2pp::repository::iterate_over_merge_head(F && func) {
	git_repository_mergehead_foreach(repo.get(), [](const git_oid * oid, void * payload) -> int { return (*static_cast<F *>(payload))(oid); }, &func);
}
