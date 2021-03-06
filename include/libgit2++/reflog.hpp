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
#include <git2/reflog.h>
#include <memory>
#include <string>


namespace git2pp {
	class reflog_deleter {
	public:
		void operator()(git_reflog * log) const noexcept;
	};


	class repository;
	class reflog_entry;

	class reflog : public guard {
	public:
		void write() noexcept;

		void append(const git_oid & id, const git_signature & committer, const char * message) noexcept;
		void append(const git_oid & id, const git_signature & committer, const std::string & message) noexcept;

		void drop(size_t idx, bool rewrite_previous_entry) noexcept;

		std::size_t size() noexcept;
		reflog_entry operator[](std::size_t idx) const;

	private:
		friend class repository;
		friend class transaction;

		reflog(git_reflog * log) noexcept;

		std::unique_ptr<git_reflog, reflog_deleter> log;
	};

	class reflog_entry : public guard {
	public:
		const git_oid & old_oid() const noexcept;
		const git_oid & new_oid() const noexcept;
		const git_signature & committer() const noexcept;
		const char * message() const noexcept;

	private:
		friend class reflog;

		reflog_entry(const git_reflog_entry * ent) noexcept;

		const git_reflog_entry * ent;
	};
}
