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


#include "libgit2++/message.hpp"
#include "libgit2++/detail/scope.hpp"
#include "libgit2++/guard.hpp"


std::string git2pp::message_prettify(const char * message, bool strip_comments, char comment_char) {
	guard grd;

	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_message_prettify(&buf, message, strip_comments, comment_char);

	return buf.ptr;
}

std::string git2pp::message_prettify(const std::string & message, bool strip_comments, char comment_char) {
	return message_prettify(message.c_str(), strip_comments, comment_char);
}
