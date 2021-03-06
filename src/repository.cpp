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
#include "libgit2++/reflog.hpp"
#include <algorithm>
#include <git2/blame.h>
#include <git2/branch.h>
#include <git2/buffer.h>
#include <git2/object.h>
#include <git2/tree.h>
#include <iterator>


void git2pp::repository_deleter::operator()(git_repository * repo) const noexcept {
	if(owning)
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

bool git2pp::repository::shallow() noexcept {
	return git_repository_is_shallow(repo.get());
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

	return {buf.ptr, buf.size};
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

void git2pp::repository::set_head(const char * path) noexcept {
	git_repository_set_head(repo.get(), path);
}

void git2pp::repository::set_head(const std::string & refname) noexcept {
	return set_head(refname.c_str());
}

void git2pp::repository::detach_head() noexcept {
	git_repository_detach_head(repo.get());
}

void git2pp::repository::detach_head(const git_oid & commitish) noexcept {
	git_repository_set_head_detached(repo.get(), &commitish);
}

git2pp::repository_state git2pp::repository::state() noexcept {
	return static_cast<repository_state>(git_repository_state(repo.get()));
}

std::experimental::optional<std::string> git2pp::repository::name_space() {
	if(const auto name = git_repository_get_namespace(repo.get()))
		return {name};
	else
		return std::experimental::nullopt;
}

void git2pp::repository::name_space(const char * name) noexcept {
	git_repository_set_namespace(repo.get(), name);
}

void git2pp::repository::name_space(const std::string & name) noexcept {
	name_space(name.c_str());
}

git2pp::repository::identity_t git2pp::repository::identity() const {
	const char * name;
	const char * email;

	git_repository_ident(&name, &email, repo.get());

	return {name, email};
}

void git2pp::repository::identity(const git2pp::repository::identity_t & ident) noexcept {
	git_repository_set_ident(repo.get(), ident.name.c_str(), ident.email.c_str());
}

git2pp::reference git2pp::repository::lookup(const char * full_name) noexcept {
	git_reference * result;
	git_reference_lookup(&result, repo.get(), full_name);
	return {result};
}

git2pp::reference git2pp::repository::lookup(const std::string & full_name) noexcept {
	return lookup(full_name.c_str());
}

git2pp::reference git2pp::repository::lookup_dwim(const char * short_name) noexcept {
	git_reference * result;
	git_reference_dwim(&result, repo.get(), short_name);
	return {result};
}

git2pp::reference git2pp::repository::lookup_dwim(const std::string & short_name) noexcept {
	return lookup_dwim(short_name.c_str());
}

git_oid git2pp::repository::lookup_id(const char * name) noexcept {
	git_oid result;
	git_reference_name_to_id(&result, repo.get(), name);
	return result;
}

git_oid git2pp::repository::lookup_id(const std::string & name) noexcept {
	return lookup_id(name.c_str());
}

std::experimental::optional<git2pp::reference> git2pp::repository::make_symbolic_reference(const char * name, const char * target, const char * current_value,
                                                                                           const char * log_message, bool force) noexcept {
	git_reference * result;
	git_reference_symbolic_create_matching(&result, repo.get(), name, target, force, current_value, log_message);
	if(result)
		return {result};
	else
		return std::experimental::nullopt;
}

std::experimental::optional<git2pp::reference> git2pp::repository::make_symbolic_reference(const std::string & name, const std::string & target,
                                                                                           const std::string & current_value, const std::string & log_message,
                                                                                           bool force) noexcept {
	return make_symbolic_reference(name.c_str(), target.c_str(), current_value.c_str(), log_message.c_str(), force);
}

git2pp::reference git2pp::repository::make_symbolic_reference(const char * name, const char * target, const char * log_message, bool force) noexcept {
	git_reference * result;
	git_reference_symbolic_create(&result, repo.get(), name, target, force, log_message);
	return result;
}

git2pp::reference git2pp::repository::make_symbolic_reference(const std::string & name, const std::string & target, const std::string & log_message,
                                                              bool force) noexcept {
	return make_symbolic_reference(name.c_str(), target.c_str(), log_message.c_str(), force);
}

git2pp::reference git2pp::repository::make_reference(const char * name, const git_oid & id, const char * log_message, bool force) {
	git_reference * result;
	git_reference_create(&result, repo.get(), name, &id, force, log_message);
	return result;
}

git2pp::reference git2pp::repository::make_reference(const std::string & name, const git_oid & id, const std::string & log_message, bool force) {
	return make_reference(name.c_str(), id, log_message.c_str(), force);
}

std::experimental::optional<git2pp::reference> git2pp::repository::make_reference(const char * name, const git_oid & id, const git_oid & current_id,
                                                                                  const char * log_message, bool force) {
	git_reference * result;
	git_reference_create_matching(&result, repo.get(), name, &id, force, &current_id, log_message);
	if(result)
		return {result};
	else
		return std::experimental::nullopt;
}

std::experimental::optional<git2pp::reference> git2pp::repository::make_reference(const std::string & name, const git_oid & id, const git_oid & current_id,
                                                                                  const std::string & log_message, bool force) {
	return make_reference(name.c_str(), id, current_id, log_message.c_str(), force);
}

void git2pp::repository::remove_reference(const char * name) noexcept {
	git_reference_remove(repo.get(), name);
}

void git2pp::repository::remove_reference(const std::string & name) noexcept {
	remove_reference(name.c_str());
}

std::vector<std::string> git2pp::repository::reference_names() {
	git_strarray names;
	git_reference_list(&names, repo.get());

	return {names.strings, names.strings + names.count};
}

bool git2pp::repository::reference_has_log(const char * name) noexcept {
	return git_reference_has_log(repo.get(), name);
}

bool git2pp::repository::reference_has_log(const std::string & name) noexcept {
	return reference_has_log(name.c_str());
}

void git2pp::repository::reference_ensure_log(const char * name) noexcept {
	git_reference_ensure_log(repo.get(), name);
}

void git2pp::repository::reference_ensure_log(const std::string & name) noexcept {
	reference_ensure_log(name.c_str());
}

git2pp::object git2pp::repository::lookup(const git_oid & id, object_type type) noexcept {
	git_object * result;
	git_object_lookup(&result, repo.get(), &id, static_cast<git_otype>(type));
	return {result};
}

git2pp::object git2pp::repository::lookup_prefix(const git_oid & id, std::size_t length, object_type type) noexcept {
	git_object * result;
	git_object_lookup_prefix(&result, repo.get(), &id, length, static_cast<git_otype>(type));
	return {result};
}

git2pp::reference git2pp::repository::create_branch(const char * name, const git2pp::commit & target, bool force) noexcept {
	git_reference * result;
	git_branch_create(&result, repo.get(), name, target.cmt.get(), force);
	return {result};
}

git2pp::reference git2pp::repository::create_branch(const std::string & name, const git2pp::commit & target, bool force) noexcept {
	return create_branch(name.c_str(), target, force);
}

git2pp::reference git2pp::repository::create_branch(const char * name, const git2pp::annotated_commit & target, bool force) noexcept {
	git_reference * result;
	git_branch_create_from_annotated(&result, repo.get(), name, target.cmt.get(), force);
	return {result};
}

git2pp::reference git2pp::repository::create_branch(const std::string & name, const git2pp::annotated_commit & target, bool force) noexcept {
	return create_branch(name.c_str(), target, force);
}

std::experimental::optional<git2pp::reference> git2pp::repository::lookup_branch(const char * name, branch_type type) noexcept {
	git_reference * result;
	git_branch_lookup(&result, repo.get(), name, static_cast<git_branch_t>(type));
	return {result};
}

std::experimental::optional<git2pp::reference> git2pp::repository::lookup_branch(const std::string & name, branch_type type) noexcept {
	return lookup_branch(name.c_str(), type);
}

std::string git2pp::repository::upstream_branch(const char * name) const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_branch_upstream_name(&buf, repo.get(), name);

	return {buf.ptr, buf.size};
}

std::string git2pp::repository::upstream_branch(const std::string & name) const {
	return upstream_branch(name.c_str());
}

std::string git2pp::repository::branch_remote_name(const char * name) const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_branch_remote_name(&buf, repo.get(), name);

	return {buf.ptr, buf.size};
}

std::string git2pp::repository::branch_remote_name(const std::string & name) const {
	return branch_remote_name(name.c_str());
}

std::string git2pp::repository::branch_remote_upstream(const char * name) const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_branch_upstream_remote(&buf, repo.get(), name);

	return {buf.ptr, buf.size};
}

std::string git2pp::repository::branch_remote_upstream(const std::string & name) const {
	return branch_remote_upstream(name.c_str());
}

git2pp::reflog git2pp::repository::reflog_read(const char * name) noexcept {
	git_reflog * result;
	git_reflog_read(&result, repo.get(), name);
	return {result};
}

git2pp::reflog git2pp::repository::reflog_read(const std::string & name) noexcept {
	return reflog_read(name.c_str());
}

void git2pp::repository::reflog_rename(const char * old_name, const char * new_name) noexcept {
	git_reflog_rename(repo.get(), old_name, new_name);
}

void git2pp::repository::reflog_rename(const std::string & old_name, const char * new_name) noexcept {
	reflog_rename(old_name.c_str(), new_name);
}

void git2pp::repository::reflog_rename(const char * old_name, const std::string & new_name) noexcept {
	reflog_rename(old_name, new_name.c_str());
}

void git2pp::repository::reflog_rename(const std::string & old_name, const std::string & new_name) noexcept {
	reflog_rename(old_name.c_str(), new_name.c_str());
}

void git2pp::repository::reflog_delete(const char * name) noexcept {
	git_reflog_delete(repo.get(), name);
}

void git2pp::repository::reflog_delete(const std::string & name) noexcept {
	reflog_delete(name.c_str());
}

git2pp::commit git2pp::repository::commit_lookup(const git_oid & id) noexcept {
	git_commit * result;
	git_commit_lookup(&result, repo.get(), &id);
	return {result};
}

git2pp::commit git2pp::repository::commit_lookup(const git_oid & id, std::size_t prefix_len) noexcept {
	git_commit * result;
	git_commit_lookup_prefix(&result, repo.get(), &id, prefix_len);
	return {result};
}

git2pp::annotated_commit git2pp::repository::annotated_commit_lookup(const git_oid & id) noexcept {
	git_annotated_commit * result;
	git_annotated_commit_lookup(&result, repo.get(), &id);
	return {result};
}

std::experimental::optional<std::pair<std::string, std::string>> git2pp::repository::extract_commit_signature(git_oid id, const char * field) {
	git_buf signature{};
	git_buf data{};
	detail::quickscope_wrapper signature_cleanup{[&]() { git_buf_free(&signature); }};
	detail::quickscope_wrapper data_cleanup{[&]() { git_buf_free(&data); }};

	if(git_commit_extract_signature(&signature, &data, repo.get(), &id, field))
		return std::experimental::nullopt;
	else
		return {{{signature.ptr, signature.size}, {data.ptr, data.size}}};
}

std::experimental::optional<std::pair<std::string, std::string>> git2pp::repository::extract_commit_signature(git_oid id, const std::string & field) {
	return extract_commit_signature(id, field.c_str());
}

std::experimental::optional<std::pair<std::string, std::string>> git2pp::repository::extract_commit_signature(git_oid id) {
	return extract_commit_signature(id, nullptr);
}

git_oid git2pp::repository::commit_create(const git_signature & author, const git_signature & committer, const char * message, const commit_tree & tree,
                                          const std::vector<const commit *> & parents, const char * update_ref, const char * message_encoding) {
	std::vector<const git_commit *> parents_raw;
	parents_raw.reserve(parents.size());
	std::transform(parents.begin(), parents.end(), std::back_inserter(parents_raw), [](const auto cmt) { return cmt->cmt.get(); });

	git_oid id;
	git_commit_create(&id, repo.get(), update_ref, &author, &committer, message_encoding, message, tree.trr.get(), parents.size(), parents_raw.data());
	return id;
}

git_oid git2pp::repository::commit_create(const git_signature & author, const git_signature & committer, const std::string & message, const commit_tree & tree,
                                          const std::vector<const commit *> & parents, const char * update_ref, const char * message_encoding) {
	return commit_create(author, committer, message.c_str(), tree, parents, update_ref, message_encoding);
}

git2pp::commit_tree git2pp::repository::tree_lookup(const git_oid & id) noexcept {
	git_tree * result;
	git_tree_lookup(&result, repo.get(), &id);
	return {result};
}

git2pp::commit_tree git2pp::repository::tree_lookup(const git_oid & id, std::size_t prefix_len) noexcept {
	git_tree * result;
	git_tree_lookup_prefix(&result, repo.get(), &id, prefix_len);
	return {result};
}

git2pp::blame git2pp::repository::blame_file(const char * path, git_blame_options opts) noexcept {
	git_blame * result;
	git_blame_file(&result, repo.get(), path, &opts);
	return {result};
}

git2pp::blame git2pp::repository::blame_file(const std::string & path, git_blame_options opts) noexcept {
	return blame_file(path.c_str(), opts);
}

git2pp::blob git2pp::repository::blob_lookup(const git_oid & id) noexcept {
	git_blob * result;
	git_blob_lookup(&result, repo.get(), &id);
	return {result};
}

git2pp::blob git2pp::repository::blob_lookup(const git_oid & id, std::size_t prefix_len) noexcept {
	git_blob * result;
	git_blob_lookup_prefix(&result, repo.get(), &id, prefix_len);
	return {result};
}

git_oid git2pp::repository::blob_create_from_working_directory(const char * relative_path) noexcept {
	git_oid id;
	git_blob_create_fromworkdir(&id, repo.get(), relative_path);
	return id;
}

git_oid git2pp::repository::blob_create_from_working_directory(const std::string & relative_path) noexcept {
	return blob_create_from_working_directory(relative_path.c_str());
}

git_oid git2pp::repository::blob_create_from_disk(const char * path) noexcept {
	git_oid id;
	git_blob_create_fromdisk(&id, repo.get(), path);
	return id;
}

git_oid git2pp::repository::blob_create_from_disk(const std::string & path) noexcept {
	return blob_create_from_disk(path.c_str());
}

git_oid git2pp::repository::blob_create_from_buffer(const void * buffer, std::size_t length) noexcept {
	git_oid id;

	git_blob_create_frombuffer(&id, repo.get(), buffer, length);
	return id;
}

git_oid git2pp::repository::blob_create_from_buffer(const std::string & buffer, std::size_t length) noexcept {
	return blob_create_from_buffer(buffer.c_str(), length);
}

git_oid git2pp::repository::blob_create_from_buffer(const std::string & buffer) noexcept {
	return blob_create_from_buffer(buffer.c_str(), buffer.size());
}


git2pp::repository::repository(git_repository * r, bool owning) noexcept : repo(r, {owning}) {}


std::string git2pp::discover_repository(const std::string & start, const std::string & ceiling_dirs, bool across_fs) {
	return discover_repository(start.c_str(), ceiling_dirs, across_fs);
}

std::string git2pp::discover_repository(const char * start, const std::string & ceiling_dirs, bool across_fs) {
	guard grd;

	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_repository_discover(&buf, start, across_fs, ceiling_dirs.c_str());

	return {buf.ptr, buf.size};
}
