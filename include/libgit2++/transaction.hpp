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
#include "signature.hpp"
#include <git2/oid.h>
#include <git2/types.h>
#include <memory>

#include <git2/transaction.h>


namespace git2pp {
	class transaction_deleter {
	public:
		void operator()(git_transaction * tx) const noexcept;
	};


	class repository;
	class reflog;

	class transaction : public guard {
	public:
		void lock_reference(const char * name) noexcept;
		void lock_reference(const std::string & name) noexcept;

		void set_target(const char * name, const git_oid & target, const char * message) noexcept;
		void set_target(const char * name, const git_oid & target, const char * message, const git_signature & sig) noexcept;
		void set_target_symbollic(const char * name, const char * target, const char * message) noexcept;
		void set_target_symbollic(const char * name, const char * target, const char * message, const git_signature & sig) noexcept;
		void set_target_symbollic(const std::string & name, const std::string & target, const std::string & message) noexcept;
		void set_target_symbollic(const std::string & name, const std::string & target, const std::string & message, const git_signature & sig) noexcept;

		void set_reference_reflog(const char * name, const reflog & log) noexcept;
		void set_reference_reflog(const std::string & name, const reflog & log) noexcept;
		void remove_reference(const char * name) noexcept;
		void remove_reference(const std::string & name) noexcept;

		void commit() noexcept;

		transaction(repository & repo) noexcept;

	private:
		friend class configuration;

		transaction(git_transaction * tx) noexcept;

		std::unique_ptr<git_transaction, transaction_deleter> tx;
	};
}
