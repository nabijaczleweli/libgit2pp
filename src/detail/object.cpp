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


#include "libgit2++/object.hpp"
#include "libgit2++/detail/scope.hpp"
#include "libgit2++/repository.hpp"
#include <git2/buffer.h>
#include <git2/object.h>


bool git2pp::object_type_loose(git2pp::object_type type) noexcept {
	return git_object_typeisloose(static_cast<git_otype>(type));
}

std::string git2pp::object_type_to_name(git2pp::object_type type) noexcept {
	return git_object_type2string(static_cast<git_otype>(type));
}

git2pp::object_type git2pp::object_type_from_name(const char * name) noexcept {
	return static_cast<object_type>(git_object_string2type(name));
}

git2pp::object_type git2pp::object_type_from_name(const std::string & name) noexcept {
	return object_type_from_name(name.c_str());
}


void git2pp::object_deleter::operator()(git_object * obj) const noexcept {
	if(owning)
		git_object_free(obj);
}


git2pp::object git2pp::object::lookup_by_path(const char * path, object_type type) const noexcept {
	git_object * result;
	git_object_lookup_bypath(&result, obj.get(), path, static_cast<git_otype>(type));
	return {result};
}

git2pp::object git2pp::object::lookup_by_path(const std::string & path, object_type type) const noexcept {
	return lookup_by_path(path.c_str(), type);
}

const git_oid & git2pp::object::id() const noexcept {
	return *git_object_id(obj.get());
}

std::string git2pp::object::short_id() const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_object_short_id(&buf, obj.get());

	return buf.ptr;
}

git2pp::object_type git2pp::object::type() const noexcept {
	return static_cast<object_type>(git_object_type(obj.get()));
}

git2pp::repository git2pp::object::owner() const noexcept {
	return {git_object_owner(obj.get()), false};
}

git2pp::object git2pp::object::peel(git2pp::object_type target_type) const noexcept {
	git_object * result;
	git_object_peel(&result, obj.get(), static_cast<git_otype>(target_type));
	return {result};
}


git2pp::object::object(git_object * r, bool owning) noexcept : obj(r, {owning}) {}
