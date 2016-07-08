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


#include "libgit2++/reflog.hpp"
#include "libgit2++/detail/scope.hpp"
#include <sstream>
#include <stdexcept>


void git2pp::reflog_deleter::operator()(git_reflog * log) const noexcept {
	git_reflog_free(log);
}


void git2pp::reflog::write() noexcept {
	git_reflog_write(log.get());
}

void git2pp::reflog::append(const git_oid & id, const git_signature & committer, const char * message) noexcept {
	git_reflog_append(log.get(), &id, &committer, message);
}

void git2pp::reflog::append(const git_oid & id, const git_signature & committer, const std::string & message) noexcept {
	append(id, committer, message.c_str());
}

void git2pp::reflog::drop(size_t idx, bool rewrite_previous_entry) noexcept {
	git_reflog_drop(log.get(), idx, rewrite_previous_entry);
}

std::size_t git2pp::reflog::size() noexcept {
	return git_reflog_entrycount(log.get());
}

git2pp::reflog_entry git2pp::reflog::operator[](std::size_t idx) const {
	if(const auto entry = git_reflog_entry_byindex(log.get(), idx))
		return {entry};
	else
		throw std::out_of_range(static_cast<std::stringstream &>(std::stringstream{} << "idx=" << idx << " >= size()=" << git_reflog_entrycount(log.get())
		                                                                             << " when accessing a reflog entry")
		                            .str());
}


git2pp::reflog::reflog(git_reflog * r) noexcept : log(r) {}


const git_oid & git2pp::reflog_entry::old_oid() const noexcept {
	return *git_reflog_entry_id_old(ent);
}

const git_oid & git2pp::reflog_entry::new_oid() const noexcept {
	return *git_reflog_entry_id_new(ent);
}

const git_signature & git2pp::reflog_entry::committer() const noexcept {
	return *git_reflog_entry_committer(ent);
}

const char * git2pp::reflog_entry::message() const noexcept {
	return git_reflog_entry_message(ent);
}


git2pp::reflog_entry::reflog_entry(const git_reflog_entry * ent) noexcept : ent(ent) {}
