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


#include "libgit2++/reference.hpp"
#include "libgit2++/repository.hpp"


void git2pp::reference_deleter::operator()(git_reference * ref) const noexcept {
	if(owning)
		git_reference_free(ref);
}


std::experimental::optional<git_oid> git2pp::reference::target(bool peel) const noexcept {
	const git_oid * sha;
	if(peel)
		sha = git_reference_target(ref.get());
	else
		sha = git_reference_target_peel(ref.get());

	if(sha)
		return {*sha};
	else
		return std::experimental::nullopt;
}

std::experimental::optional<std::string> git2pp::reference::target_symbollic() const {
	if(const auto name = git_reference_symbolic_target(ref.get()))
		return {name};
	else
		return std::experimental::nullopt;
}

git2pp::reference_type git2pp::reference::type() const noexcept {
	return static_cast<reference_type>(git_reference_type(ref.get()));
}

std::string git2pp::reference::name() const {
	return git_reference_name(ref.get());
}

git2pp::reference git2pp::reference::resolve() const noexcept {
	git_reference * result;
	git_reference_resolve(&result, ref.get());
	return {result};
}

git2pp::repository git2pp::reference::owner() const noexcept {
	return {git_reference_owner(ref.get()), false};
}

void git2pp::reference::set_target_symbollic(const char * target, const char * log_message) noexcept {
	git_reference * new_ref;
	git_reference_symbolic_set_target(&new_ref, ref.get(), target, log_message);
	ref.reset(new_ref);
}

void git2pp::reference::set_target_symbollic(const std::string & target, const std::string & log_message) noexcept {
	set_target_symbollic(target.c_str(), log_message.c_str());
}

void git2pp::reference::set_target(const git_oid & id, const char * log_message) noexcept {
	git_reference * new_ref;
	git_reference_set_target(&new_ref, ref.get(), &id, log_message);
	ref.reset(new_ref);
}

void git2pp::reference::set_target(const git_oid & id, const std::string log_message) noexcept {
	set_target(id, log_message.c_str());
}

git2pp::reference git2pp::reference::rename(const char * new_name, const char * log_message, bool force) noexcept {
	git_reference * result;
	git_reference_rename(&result, ref.get(), new_name, force, log_message);
	return {result};
}

git2pp::reference git2pp::reference::rename(const std::string & new_name, const std::string & log_message, bool force) noexcept {
	return rename(new_name.c_str(), log_message.c_str(), force);
}

void git2pp::reference::remove() noexcept {
	git_reference_delete(ref.get());
}


git2pp::reference::reference(git_reference * r, bool owning) noexcept : ref(r, {owning}) {}
