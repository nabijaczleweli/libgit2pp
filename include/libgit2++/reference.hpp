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
#include "object.hpp"
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

	enum class normalisation_opts {
		normal          = GIT_REF_FORMAT_NORMAL,
		allow_one_level = GIT_REF_FORMAT_ALLOW_ONELEVEL,

		refspec_pattern   = GIT_REF_FORMAT_REFSPEC_PATTERN,
		refspec_shorthand = GIT_REF_FORMAT_REFSPEC_SHORTHAND,
	};

	constexpr normalisation_opts operator&(normalisation_opts lhs, normalisation_opts rhs);
	constexpr normalisation_opts operator|(normalisation_opts lhs, normalisation_opts rhs);


	class reference_deleter {
	public:
		bool owning;

		void operator()(git_reference * ref) const noexcept;
	};


	class reference : public guard {
		friend class repository;


	public:
		std::experimental::optional<git_oid> target(bool peel = false) const noexcept;
		std::experimental::optional<std::string> target_symbollic() const;

		reference_type type() const noexcept;
		std::string name() const;
		std::string shorthand() const;

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

		bool is_branch() const noexcept;
		bool is_remote() const noexcept;
		bool is_tag() const noexcept;
		bool is_note() const noexcept;

		object peel(object_type type) noexcept;

		reference branch_rename(const char * new_name, bool force = false) noexcept;
		reference branch_rename(const std::string & new_name, bool force = false) noexcept;
		std::string branch_name() const;

		reference branch_upstream() const noexcept;
		void branch_upstream(const char * name) noexcept;
		void branch_upstream(const std::string & name) noexcept;

	private:
		reference(git_reference * ref, bool owning = true) noexcept;

		std::unique_ptr<git_reference, reference_deleter> ref;
	};


	std::string normalize_reference_name(const char * name, normalisation_opts flags = normalisation_opts::normal);
	std::string normalize_reference_name(const std::string & name, normalisation_opts flags = normalisation_opts::normal);

	bool reference_name_valid(const char * name);
	bool reference_name_valid(const std::string & name);
}


constexpr git2pp::normalisation_opts git2pp::operator&(git2pp::normalisation_opts lhs, git2pp::normalisation_opts rhs) {
	return static_cast<normalisation_opts>(static_cast<unsigned int>(lhs) & static_cast<unsigned int>(rhs));
}

constexpr git2pp::normalisation_opts git2pp::operator|(git2pp::normalisation_opts lhs, git2pp::normalisation_opts rhs) {
	return static_cast<normalisation_opts>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
}
