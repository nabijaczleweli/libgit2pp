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
#include <experimental/optional>
#include <git2/refs.h>
#include <memory>
#include <string>


namespace git2pp {
	class repository;


	enum class reference_type {
		direct   = GIT_REF_OID,
		symbolic = GIT_REF_SYMBOLIC,
	};


	class reference_deleter {
	public:
		void operator()(git_reference * ref) const noexcept;
	};


	class reference : public guard {
		friend class repository;


	public:
		std::experimental::optional<git_oid> target(bool peel = false) const noexcept;
		std::experimental::optional<std::string> target_symbollic() const;

		reference_type type() const noexcept;
		std::string name() const;

		reference resolve() const noexcept;
		repository owner() const noexcept;

		void set_target_symbollic(const char * target, const char * log_message) noexcept;
		void set_target_symbollic(const std::string & target, const std::string & log_message) noexcept;
		void set_target(const git_oid & id, const char * log_message) noexcept;
		void set_target(const git_oid & id, const std::string log_message) noexcept;

		reference rename(const char * new_name, const char * log_message, bool force = false) noexcept;
		reference rename(const std::string & new_name, const std::string & log_message, bool force = false) noexcept;

		// not `delete` because it's a keyword
		void remove() noexcept;

	private:
		reference(git_reference * ref) noexcept;

		std::unique_ptr<git_reference, reference_deleter> ref;
	};
}
