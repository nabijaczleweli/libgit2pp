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
#include <vector>


namespace git2pp {
	enum class repository_state {
		none                    = GIT_REPOSITORY_STATE_NONE,
		merge                   = GIT_REPOSITORY_STATE_MERGE,
		revert                  = GIT_REPOSITORY_STATE_REVERT,
		revert_sequence         = GIT_REPOSITORY_STATE_REVERT_SEQUENCE,
		cherrypick              = GIT_REPOSITORY_STATE_CHERRYPICK,
		cherrypick_sequence     = GIT_REPOSITORY_STATE_CHERRYPICK_SEQUENCE,
		bisect                  = GIT_REPOSITORY_STATE_BISECT,
		rebase                  = GIT_REPOSITORY_STATE_REBASE,
		rebase_interactive      = GIT_REPOSITORY_STATE_REBASE_INTERACTIVE,
		rebase_merge            = GIT_REPOSITORY_STATE_REBASE_MERGE,
		apply_mailbox           = GIT_REPOSITORY_STATE_APPLY_MAILBOX,
		apply_mailbox_or_rebase = GIT_REPOSITORY_STATE_APPLY_MAILBOX_OR_REBASE,
	};


	class repository_deleter {
	public:
		bool owning;

		void operator()(git_repository * repo) const noexcept;
	};


	class repository : public guard {
	public:
		struct identity_t {
			std::string name;
			std::string email;
		};


		static repository init(const char * path, bool bare = false) noexcept;
		static repository init(const std::string & path, bool bare = false) noexcept;

		static repository open(const char * path) noexcept;
		static repository open(const std::string & path) noexcept;


		reference head() noexcept;
		bool head_detached() noexcept;
		bool head_unborn() noexcept;

		bool empty() noexcept;
		bool bare() noexcept;
		bool shallow() noexcept;
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

		void set_head(const char * refname) noexcept;
		void set_head(const std::string & refname) noexcept;
		void detach_head() noexcept;
		void detach_head(const git_oid & commitish) noexcept;
		// TODO: git_repository_set_head_detached_from_annotated

		repository_state state() noexcept;

		std::experimental::optional<std::string> name_space();
		void name_space(const char * name) noexcept;
		void name_space(const std::string & name) noexcept;

		identity_t identity() const;
		void identity(const identity_t & ident) noexcept;

		reference lookup(const char * full_name) noexcept;
		reference lookup(const std::string & full_name) noexcept;
		reference lookup_dwim(const char * short_name) noexcept;
		reference lookup_dwim(const std::string & short_name) noexcept;
		git_oid lookup_id(const char * name) noexcept;
		git_oid lookup_id(const std::string & name) noexcept;

		reference make_symbolic_reference(const char * name, const char * target, const char * log_message, bool force = false) noexcept;
		reference make_symbolic_reference(const std::string & name, const std::string & target, const std::string & log_message, bool force = false) noexcept;
		std::experimental::optional<reference> make_symbolic_reference(const char * name, const char * target, const char * current_value, const char * log_message,
		                                                               bool force = false) noexcept;
		std::experimental::optional<reference> make_symbolic_reference(const std::string & name, const std::string & target, const std::string & current_value,
		                                                               const std::string & log_message, bool force = false) noexcept;

		reference make_reference(const char * name, const git_oid & id, const char * log_message, bool force = false);
		reference make_reference(const std::string & name, const git_oid & id, const std::string & log_message, bool force = false);
		std::experimental::optional<reference> make_reference(const char * name, const git_oid & id, const git_oid & current_id, const char * log_message,
		                                                      bool force = false);
		std::experimental::optional<reference> make_reference(const std::string & name, const git_oid & id, const git_oid & current_id,
		                                                      const std::string & log_message, bool force = false);

		void remove_reference(const char * name) noexcept;
		void remove_reference(const std::string & name) noexcept;
		std::vector<std::string> reference_names();

		template <class F>
		void iterate_over_references(F && func);
		template <class F>
		void iterate_over_reference_names(F && func);
		template <class F>
		void iterate_over_reference_names_glob(const char * glob, F && func);
		template <class F>
		void iterate_over_reference_names_glob(const std::string & glob, F && func);

		// Enumeration via iterators (git_reference_iterator_new()/git_reference_iterator_glob_new()) impossible because they need to be copy-constructible and
		// that's impossible because C wrappers :v

		bool reference_has_log(const char * name) noexcept;
		bool reference_has_log(const std::string & name) noexcept;
		void reference_ensure_log(const char * name) noexcept;
		void reference_ensure_log(const std::string & name) noexcept;

	private:
		friend class reference;

		repository(git_repository * repo, bool owning = true) noexcept;

		std::unique_ptr<git_repository, repository_deleter> repo;
	};


	std::string discover_repository(const char * start, const std::string & ceiling_dirs = "", bool across_fs = true);
	std::string discover_repository(const std::string & start, const std::string & ceiling_dirs = "", bool across_fs = true);
}


template <class F>
void git2pp::repository::iterate_over_fetch_head(F && func) {
	git_repository_fetchhead_foreach(repo.get(), [](const char * ref_name, const char * remote_url, const git_oid * oid, unsigned int is_merge,
	                                                void * payload) -> int { return (*static_cast<F *>(payload))(ref_name, remote_url, *oid, is_merge); },
	                                 &func);
}

template <class F>
void git2pp::repository::iterate_over_merge_head(F && func) {
	git_repository_mergehead_foreach(repo.get(), [](const git_oid * oid, void * payload) -> int { return (*static_cast<F *>(payload))(*oid); }, &func);
}

template <class F>
void git2pp::repository::iterate_over_references(F && func) {
	git_reference_foreach(repo.get(), [](git_reference * ref, void * payload) -> int { return (*static_cast<F *>(payload))(reference(ref)); }, &func);
}

template <class F>
void git2pp::repository::iterate_over_reference_names(F && func) {
	git_reference_foreach_name(repo.get(), [](const char * name, void * payload) -> int { return (*static_cast<F *>(payload))(name); }, &func);
}

template <class F>
void git2pp::repository::iterate_over_reference_names_glob(const char * glob, F && func) {
	git_reference_foreach_glob(repo.get(), glob, [](const char * name, void * payload) -> int { return (*static_cast<F *>(payload))(name); }, &func);
}

template <class F>
void git2pp::repository::iterate_over_reference_names_glob(const std::string & glob, F && func) {
	iterate_over_reference_names_glob(glob.c_str(), std::forward<F>(func));
}
