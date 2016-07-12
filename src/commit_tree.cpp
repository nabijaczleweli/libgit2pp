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


#include "libgit2++/commit_tree.hpp"
#include "libgit2++/repository.hpp"
#include <git2/tree.h>
#include <sstream>


void git2pp::commit_tree_deleter::operator()(git_tree * trr) const noexcept {
	if(owning)
		git_tree_free(trr);
}

void git2pp::commit_tree_entry_deleter::operator()(git_tree_entry * ent) const noexcept {
	if(owning)
		git_tree_entry_free(ent);
}


const git_oid & git2pp::commit_tree::id() const noexcept {
	return *git_tree_id(trr.get());
}

git2pp::repository git2pp::commit_tree::owner() const noexcept {
	return {git_tree_owner(trr.get()), false};
}

std::size_t git2pp::commit_tree::size() const noexcept {
	return git_tree_entrycount(trr.get());
}

git2pp::commit_tree_entry git2pp::commit_tree::operator[](std::size_t idx) const {
	if(const auto ent = git_tree_entry_byindex(trr.get(), idx))
		return {ent};
	else
		throw std::out_of_range(
		    static_cast<std::stringstream &>(std::stringstream{} << "idx=" << idx << " >= size()=" << size() << " when accessing a tree entry").str());
}

git2pp::commit_tree_entry git2pp::commit_tree::operator[](const git_oid & id) const {
	if(const auto ent = git_tree_entry_byid(trr.get(), &id))
		return {ent};
	else {
		std::stringstream buf;
		buf << std::hex << "id=";
		for(unsigned int b : id.id)
			buf << b;
		buf << " nonexistant when accessing a tree entry";
		throw std::out_of_range(buf.str());
	}
}

git2pp::commit_tree_entry git2pp::commit_tree::at_filename(const char * filename) const {
	if(const auto ent = git_tree_entry_byname(trr.get(), filename))
		return {ent};
	else
		throw std::out_of_range(
		    static_cast<std::stringstream &>(std::stringstream{} << "filename=\"" << filename << "\" nonexistant when accessing a tree entry").str());
}

git2pp::commit_tree_entry git2pp::commit_tree::at_filename(const std::string & filename) const {
	return at_filename(filename.c_str());
}

git2pp::commit_tree_entry git2pp::commit_tree::at_path(const char * path) const {
	git_tree_entry * result;
	git_tree_entry_bypath(&result, trr.get(), path);
	if(result)
		return {result};
	else
		throw std::out_of_range(static_cast<std::stringstream &>(std::stringstream{} << "path=\"" << path << "\" nonexistant when accessing a tree entry").str());
}

git2pp::commit_tree_entry git2pp::commit_tree::at_path(const std::string & path) const {
	return at_path(path.c_str());
}


const char * git2pp::commit_tree_entry::name() const noexcept {
	return git_tree_entry_name(ent.get());
}

const git_oid & git2pp::commit_tree_entry::id() const noexcept {
	return *git_tree_entry_id(ent.get());
}

git2pp::object_type git2pp::commit_tree_entry::type() const noexcept {
	return static_cast<object_type>(git_tree_entry_type(ent.get()));
}

git2pp::filemode git2pp::commit_tree_entry::file_mode() const noexcept {
	return static_cast<filemode>(git_tree_entry_filemode(ent.get()));
}

git2pp::filemode git2pp::commit_tree_entry::file_mode_raw() const noexcept {
	return static_cast<filemode>(git_tree_entry_filemode_raw(ent.get()));
}

git2pp::object git2pp::commit_tree_entry::to_object(git2pp::repository & repo) const noexcept {
	git_object * result;
	git_tree_entry_to_object(&result, repo.repo.get(), ent.get());
	return {result};
}

git2pp::commit_tree_entry::commit_tree_entry(const commit_tree_entry & other) noexcept {
	git_tree_entry * result;
	git_tree_entry_dup(&result, other.ent.get());
	ent = {result, {true}};
}


bool git2pp::operator<(const git2pp::commit_tree_entry & lhs, const git2pp::commit_tree_entry & rhs) noexcept {
	return git_tree_entry_cmp(lhs.ent.get(), rhs.ent.get()) < 0;
}

bool git2pp::operator>(const git2pp::commit_tree_entry & lhs, const git2pp::commit_tree_entry & rhs) noexcept {
	return git_tree_entry_cmp(lhs.ent.get(), rhs.ent.get()) > 0;
}

bool git2pp::operator<=(const git2pp::commit_tree_entry & lhs, const git2pp::commit_tree_entry & rhs) noexcept {
	return !(lhs > rhs);
}

bool git2pp::operator>=(const git2pp::commit_tree_entry & lhs, const git2pp::commit_tree_entry & rhs) noexcept {
	return !(lhs < rhs);
}

bool git2pp::operator==(const git2pp::commit_tree_entry & lhs, const git2pp::commit_tree_entry & rhs) noexcept {
	return git_tree_entry_cmp(lhs.ent.get(), rhs.ent.get()) == 0;
}


git2pp::commit_tree::commit_tree(git_tree * r, bool owning) noexcept : trr(r, {owning}) {}

git2pp::commit_tree_entry::commit_tree_entry(git_tree_entry * r, bool owning) noexcept : ent(r, {owning}) {}
git2pp::commit_tree_entry::commit_tree_entry(const git_tree_entry * r) noexcept : commit_tree_entry(const_cast<git_tree_entry *>(r), false) {}
