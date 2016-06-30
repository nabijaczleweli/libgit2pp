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
#include <git2/oid.h>
#include <git2/types.h>
#include <memory>
#include <string>


namespace git2pp {
	class repository;


	enum class object_type {
		any             = GIT_OBJ_ANY,
		bad             = GIT_OBJ_BAD,
		commit          = GIT_OBJ_COMMIT,
		tree            = GIT_OBJ_TREE,
		blob            = GIT_OBJ_BLOB,
		annotated_tag   = GIT_OBJ_TAG,
		delta_offset    = GIT_OBJ_OFS_DELTA,
		delta_object_id = GIT_OBJ_REF_DELTA,
	};

	bool object_type_loose(object_type type) noexcept;
	std::string object_type_to_name(object_type type) noexcept;
	object_type object_type_from_name(const char * name) noexcept;
	object_type object_type_from_name(const std::string & name) noexcept;


	class object_deleter {
	public:
		bool owning;

		void operator()(git_object * obj) const noexcept;
	};


	class object : public guard {
		friend class reference;
		friend class repository;


	public:
		object lookup_by_path(const char * path, object_type type) const noexcept;
		object lookup_by_path(const std::string & path, object_type type) const noexcept;

		const git_oid & id() const noexcept;
		std::string short_id() const;
		object_type type() const noexcept;

		repository owner() const noexcept;
		object peel(object_type target_type) const noexcept;

	private:
		object(git_object * obj, bool owning = true) noexcept;

		std::unique_ptr<git_object, object_deleter> obj;
	};
}
