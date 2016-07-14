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


#include "libgit2++/blob.hpp"
#include "libgit2++/detail/scope.hpp"
#include "libgit2++/repository.hpp"


void git2pp::blob_deleter::operator()(git_blob * blb) const noexcept {
	if(owning)
		git_blob_free(blb);
}


bool git2pp::blob::binary() const noexcept {
	return git_blob_is_binary(blb.get());
}

const git_oid & git2pp::blob::id() const noexcept {
	return *git_blob_id(blb.get());
}

git2pp::repository git2pp::blob::owner() const noexcept {
	return {git_blob_owner(blb.get()), false};
}

std::pair<const void *, std::uint64_t> git2pp::blob::raw() const noexcept {
	return {git_blob_rawcontent(blb.get()), git_blob_rawsize(blb.get())};
}

std::string git2pp::blob::filtered(const char * as_path, bool check_for_binary_data) {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_blob_filtered_content(&buf, blb.get(), as_path, check_for_binary_data);

	return {buf.ptr, buf.size};
}

std::string git2pp::blob::filtered(const std::string & as_path, bool check_for_binary_data) {
	return filtered(as_path.c_str(), check_for_binary_data);
}


git2pp::blob::blob(git_blob * blb, bool owning) noexcept : blb(blb, {owning}) {}
