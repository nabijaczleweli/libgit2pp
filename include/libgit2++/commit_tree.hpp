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
#include "object.hpp"
#include <git2/oid.h>
#include <git2/tree.h>
#include <git2/types.h>
#include <memory>


namespace git2pp {
	enum class filemode {
		unreadable      = GIT_FILEMODE_UNREADABLE,
		tree            = GIT_FILEMODE_TREE,
		blob            = GIT_FILEMODE_BLOB,
		blob_executable = GIT_FILEMODE_BLOB_EXECUTABLE,
		link            = GIT_FILEMODE_LINK,
		commit          = GIT_FILEMODE_COMMIT,
	};

	enum class tree_walk_mode {
		pre  = GIT_TREEWALK_PRE,
		post = GIT_TREEWALK_POST,
	};


	class commit_tree_deleter {
	public:
		bool owning;

		void operator()(git_tree * trr) const noexcept;
	};

	class commit_tree_entry_deleter {
	public:
		bool owning;

		void operator()(git_tree_entry * ent) const noexcept;
	};

	class commit_tree_builder_deleter {
	public:
		bool owning;

		void operator()(git_treebuilder * ent) const noexcept;
	};


	class repository;
	class commit_tree_entry;

	class commit_tree : public guard {
	public:
		const git_oid & id() const noexcept;
		repository owner() const noexcept;

		std::size_t size() const noexcept;

		commit_tree_entry operator[](std::size_t idx) const;
		commit_tree_entry operator[](const git_oid & id) const;
		commit_tree_entry at_filename(const char * filename) const;
		commit_tree_entry at_filename(const std::string & filename) const;
		commit_tree_entry at_path(const char * path) const;
		commit_tree_entry at_path(const std::string & path) const;

		template <class F>
		void walk(tree_walk_mode mode, F && func) const;

	private:
		friend class commit;
		friend class repository;
		friend class commit_tree_builder;

		commit_tree(git_tree * trr, bool owning = true) noexcept;

		std::unique_ptr<git_tree, commit_tree_deleter> trr;
	};

	class commit_tree_entry : public guard {
	public:
		const char * name() const noexcept;
		const git_oid & id() const noexcept;
		object_type type() const noexcept;
		filemode file_mode() const noexcept;
		filemode file_mode_raw() const noexcept;

		object to_object(repository & repo) const noexcept;

		commit_tree_entry(const commit_tree_entry & other) noexcept;

	private:
		friend bool operator<(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
		friend bool operator>(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
		friend bool operator<=(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
		friend bool operator>=(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
		friend bool operator==(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;

		friend class commit_tree;
		friend class commit_tree_builder;

		commit_tree_entry(git_tree_entry * ent, bool owning = true) noexcept;
		commit_tree_entry(const git_tree_entry * ent) noexcept;

		std::unique_ptr<git_tree_entry, commit_tree_entry_deleter> ent;
	};

	bool operator<(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
	bool operator>(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
	bool operator<=(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
	bool operator>=(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;
	bool operator==(const commit_tree_entry & lhs, const commit_tree_entry & rhs) noexcept;

	class commit_tree_builder : public guard {
	public:
		void clear() noexcept;
		git_oid write() noexcept;
		unsigned int size() noexcept;

		commit_tree_entry operator[](const char * filename) noexcept;
		commit_tree_entry operator[](const std::string & filename) noexcept;

		commit_tree_entry insert(const char * filename, const git_oid & id, filemode mode) noexcept;
		commit_tree_entry insert(const std::string & filename, const git_oid & id, filemode mode) noexcept;

		void remove(const char * filename) noexcept;
		void remove(const std::string & filename) noexcept;

		template <class F>
		void filter(F && func);

		commit_tree_builder(repository & repo) noexcept;
		commit_tree_builder(repository & repo, const commit_tree & tree) noexcept;

	private:
		commit_tree_builder(git_treebuilder * bld, bool owning = true) noexcept;

		std::unique_ptr<git_treebuilder, commit_tree_builder_deleter> bld;
	};
}


template <class F>
void git2pp::commit_tree::walk(git2pp::tree_walk_mode mode, F && func) const {
	git_tree_walk(
	    trr.get(), static_cast<git_treewalk_mode>(mode),
	    [](const char * root, const git_tree_entry * ent, void * payload) -> int { return (*reinterpret_cast<F *>(payload))(root, commit_tree_entry(ent)); },
	    &func);
}

template <class F>
void git2pp::commit_tree_builder::filter(F && func) {
	git_treebuilder_filter(bld.get(),
	                       [](const git_tree_entry * ent, void * payload) -> int { return (*reinterpret_cast<F *>(payload))(commit_tree_entry(ent)); } & func);
}
