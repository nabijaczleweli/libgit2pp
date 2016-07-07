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


#include "libgit2++/transaction.hpp"
#include "libgit2++/detail/scope.hpp"
#include "libgit2++/reflog.hpp"
#include "libgit2++/repository.hpp"


void git2pp::transaction_deleter::operator()(git_transaction * tx) const noexcept {
	git_transaction_free(tx);
}


void git2pp::transaction::lock_reference(const char * name) noexcept {
	git_transaction_lock_ref(tx.get(), name);
}

void git2pp::transaction::lock_reference(const std::string & name) noexcept {
	lock_reference(name.c_str());
}

void git2pp::transaction::set_target(const char * name, const git_oid & target, const char * message) noexcept {
	git_transaction_set_target(tx.get(), name, &target, nullptr, message);
}

void git2pp::transaction::set_target(const char * name, const git_oid & target, const char * message, const git_signature & sig) noexcept {
	git_transaction_set_target(tx.get(), name, &target, &sig, message);
}

void git2pp::transaction::set_target_symbollic(const char * name, const char * target, const char * message) noexcept {
	git_transaction_set_symbolic_target(tx.get(), name, target, nullptr, message);
}

void git2pp::transaction::set_target_symbollic(const char * name, const char * target, const char * message, const git_signature & sig) noexcept {
	git_transaction_set_symbolic_target(tx.get(), name, target, &sig, message);
}

void git2pp::transaction::set_target_symbollic(const std::string & name, const std::string & target, const std::string & message) noexcept {
	set_target_symbollic(name.c_str(), target.c_str(), message.c_str());
}

void git2pp::transaction::set_target_symbollic(const std::string & name, const std::string & target, const std::string & message,
                                               const git_signature & sig) noexcept {
	set_target_symbollic(name.c_str(), target.c_str(), message.c_str(), sig);
}

void git2pp::transaction::set_reference_reflog(const char * name, const reflog & log) noexcept {
	git_transaction_set_reflog(tx.get(), name, log.log.get());
}

void git2pp::transaction::set_reference_reflog(const std::string & name, const reflog & log) noexcept {
	set_reference_reflog(name.c_str(), log);
}

void git2pp::transaction::remove_reference(const char * name) noexcept {
	git_transaction_remove(tx.get(), name);
}

void git2pp::transaction::remove_reference(const std::string & name) noexcept {
	remove_reference(name.c_str());
}

void git2pp::transaction::commit() noexcept {
	git_transaction_commit(tx.get());
}


git2pp::transaction::transaction(git2pp::repository & repo) noexcept {
	git_transaction * result;
	git_transaction_new(&result, repo.repo.get());
	tx.reset(result);
}


git2pp::transaction::transaction(git_transaction * r) noexcept : tx(r) {}
