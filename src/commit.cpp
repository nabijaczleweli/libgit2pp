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


#include "libgit2++/commit.hpp"
#include "libgit2++/detail/scope.hpp"
#include "libgit2++/repository.hpp"


void git2pp::commit_deleter::operator()(git_commit * cmt) const noexcept {
	if(owning)
		git_commit_free(cmt);
}

void git2pp::annotated_commit_deleter::operator()(git_annotated_commit * cmt) const noexcept {
	if(owning)
		git_annotated_commit_free(cmt);
}


const git_oid & git2pp::commit::id() const noexcept {
	return *git_commit_id(cmt.get());
}

git2pp::repository git2pp::commit::owner() const noexcept {
	return {git_commit_owner(cmt.get()), false};
}

std::pair<std::chrono::time_point<std::chrono::system_clock>, int> git2pp::commit::time() const noexcept {
	return {std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(git_commit_time(cmt.get()))), git_commit_time_offset(cmt.get())};
}

const char * git2pp::commit::message() const noexcept {
	return git_commit_message(cmt.get());
}

const char * git2pp::commit::message_raw() const noexcept {
	return git_commit_message_raw(cmt.get());
}

std::experimental::optional<std::string> git2pp::commit::message_summary() const {
	if(const auto sum = git_commit_summary(cmt.get()))
		return {sum};
	else
		return std::experimental::nullopt;
}

std::experimental::optional<std::string> git2pp::commit::message_body() const {
	if(const auto bod = git_commit_body(cmt.get()))
		return {bod};
	else
		return std::experimental::nullopt;
}

std::experimental::optional<std::string> git2pp::commit::message_encoding() const {
	if(const auto enc = git_commit_message_encoding(cmt.get()))
		return {enc};
	else
		return std::experimental::nullopt;
}

const git_signature & git2pp::commit::committer() const noexcept {
	return *git_commit_committer(cmt.get());
}

const git_signature & git2pp::commit::author() const noexcept {
	return *git_commit_author(cmt.get());
}

git2pp::commit_tree git2pp::commit::tree() const noexcept {
	git_tree * result;
	git_commit_tree(&result, cmt.get());
	return {result};
}

const git_oid & git2pp::commit::tree_id() const noexcept {
	return *git_commit_tree_id(cmt.get());
}

unsigned int git2pp::commit::parent_amount() const noexcept {
	return git_commit_parentcount(cmt.get());
}

git2pp::commit git2pp::commit::parent(unsigned int n) const noexcept {
	git_commit * result;
	git_commit_parent(&result, cmt.get(), n);
	return {result};
}

std::experimental::optional<git_oid> git2pp::commit::parent_id(unsigned int n) const noexcept {
	if(const auto id = git_commit_parent_id(cmt.get(), n))
		return {*id};
	else
		return std::experimental::nullopt;
}

git2pp::commit git2pp::commit::ancestor(unsigned int n) const noexcept {
	git_commit * result;
	git_commit_nth_gen_ancestor(&result, cmt.get(), n);
	return {result};
}

std::string git2pp::commit::header_field(const char * field) const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_commit_header_field(&buf, cmt.get(), field);

	return buf.ptr;
}

std::string git2pp::commit::header_field(const std::string & field) const {
	return header_field(field.c_str());
}


git2pp::annotated_commit git2pp::annotated_commit::from_reference(const git2pp::reference & ref) noexcept {
	auto repo = ref.owner();
	return from_reference(ref, repo);
}

git2pp::annotated_commit git2pp::annotated_commit::from_reference(const git2pp::reference & ref, git2pp::repository & repo) noexcept {
	git_annotated_commit * result;
	git_annotated_commit_from_ref(&result, repo.repo.get(), ref.ref.get());
	return {result};
}

git2pp::annotated_commit git2pp::annotated_commit::from_fetch_head(git2pp::repository & repo, const char * branch_name, const char * remote_url,
                                                                   const git_oid & remote_id) noexcept {
	git_annotated_commit * result;
	git_annotated_commit_from_fetchhead(&result, repo.repo.get(), branch_name, remote_url, &remote_id);
	return {result};
}

git2pp::annotated_commit git2pp::annotated_commit::from_fetch_head(git2pp::repository & repo, const std::string & branch_name, const char * remote_url,
                                                                   const git_oid & remote_id) noexcept {
	return from_fetch_head(repo, branch_name.c_str(), remote_url, remote_id);
}

git2pp::annotated_commit git2pp::annotated_commit::from_fetch_head(git2pp::repository & repo, const char * branch_name, const std::string & remote_url,
                                                                   const git_oid & remote_id) noexcept {
	return from_fetch_head(repo, branch_name, remote_url.c_str(), remote_id);
}

git2pp::annotated_commit git2pp::annotated_commit::from_fetch_head(git2pp::repository & repo, const std::string & branch_name, const std::string & remote_url,
                                                                   const git_oid & remote_id) noexcept {
	return from_fetch_head(repo, branch_name.c_str(), remote_url.c_str(), remote_id);
}

git2pp::annotated_commit git2pp::annotated_commit::from_revspec(git2pp::repository & repo, const char * revspec) noexcept {
	git_annotated_commit * result;
	git_annotated_commit_from_revspec(&result, repo.repo.get(), revspec);
	return {result};
}

git2pp::annotated_commit git2pp::annotated_commit::from_revspec(git2pp::repository & repo, const std::string & revspec) noexcept {
	return from_revspec(repo, revspec.c_str());
}


const git_oid & git2pp::annotated_commit::id() const noexcept {
	return *git_annotated_commit_id(cmt.get());
}


git2pp::commit::commit(git_commit * r, bool owning) noexcept : cmt(r, {owning}) {}

git2pp::annotated_commit::annotated_commit(git_annotated_commit * r, bool owning) noexcept : cmt(r, {owning}) {}
