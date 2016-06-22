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


git2pp::repository git2pp::repository::init(const char * path, bool bare) noexcept {
	guard grd;

	git_repository * result{};
	git_repository_init(&result, path, bare);
	return {result};
}

git2pp::repository git2pp::repository::init(const std::string & path, bool bare) noexcept {
	return init(path.c_str(), bare);
}

git2pp::repository git2pp::repository::open(const char * path) noexcept {
	guard grd;

	git_repository * result{};
	git_repository_open(&result, path);
	return {result};
}

git2pp::repository git2pp::repository::open(const std::string & path) noexcept {
	return open(path.c_str());
}


git2pp::reference git2pp::repository::head() noexcept {
	git_reference * result{};
	git_repository_head(&result, repo.get());
	return {result};
}

bool git2pp::repository::head_detached() noexcept {
	return git_repository_head_detached(repo.get());
}

bool git2pp::repository::head_unborn() noexcept {
	return git_repository_head_unborn(repo.get());
}

bool git2pp::repository::empty() noexcept {
	return git_repository_is_empty(repo.get());
}

bool git2pp::repository::bare() noexcept {
	return git_repository_is_bare(repo.get());
}

std::string git2pp::repository::path() {
	return git_repository_path(repo.get());
}

std::experimental::optional<std::string> git2pp::repository::working_directory() {
	if(const auto dir = git_repository_workdir(repo.get()))
		return {dir};
	else
		return std::experimental::nullopt;
}

void git2pp::repository::working_directory(const std::string & path, bool update_gitlink) noexcept {
	working_directory(path.c_str(), update_gitlink);
}

void git2pp::repository::working_directory(const char * path, bool update_gitlink) noexcept {
	git_repository_set_workdir(repo.get(), path, update_gitlink);
}

git2pp::configuration git2pp::repository::config() noexcept {
	git_config * result{};
	git_repository_config(&result, repo.get());
	return {result};
}

git2pp::configuration git2pp::repository::config_snapshot() noexcept {
	git_config * result{};
	git_repository_config_snapshot(&result, repo.get());
	return {result};
}

std::string git2pp::repository::message() {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_repository_message(&buf, repo.get());

	return buf.ptr;
}

void git2pp::repository::remove_message() noexcept {
	git_repository_message_remove(repo.get());
}

void git2pp::repository::cleanup_state() noexcept {
	git_repository_state_cleanup(repo.get());
}

git_oid git2pp::repository::hash_file(const char * path, object_type type, const char * filters) noexcept {
	git_oid ret;
	git_repository_hashfile(&ret, repo.get(), path, static_cast<git_otype>(type), filters);
	return ret;
}

git_oid git2pp::repository::hash_file(const std::string & path, object_type type, std::experimental::optional<std::string> filters) noexcept {
	return hash_file(path.c_str(), type, filters ? filters->c_str() : nullptr);
}


git2pp::repository::repository(git_repository * r) noexcept : repo(r) {}


std::string git2pp::discover_repository(const std::string & start, const std::string & ceiling_dirs, bool across_fs) {
	return discover_repository(start.c_str(), ceiling_dirs, across_fs);
}

std::string git2pp::discover_repository(const char * start, const std::string & ceiling_dirs, bool across_fs) {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_repository_discover(&buf, start, across_fs, ceiling_dirs.c_str());

	return buf.ptr;
}
