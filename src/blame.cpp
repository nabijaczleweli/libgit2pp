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


#include "libgit2++/blame.hpp"
#include <cstring>
#include <sstream>


git2pp::blame_options::blame_options() noexcept : flags{}, min_match_characters{}, newest_commit{}, oldest_commit{}, min_line{}, max_line{} {}
git2pp::blame_options::blame_options(const git_blame_options & opts) noexcept : flags(static_cast<blame_flags>(opts.flags)),
                                                                                min_match_characters(opts.min_match_characters),
                                                                                newest_commit(opts.newest_commit),
                                                                                oldest_commit(opts.oldest_commit),
                                                                                min_line(opts.min_line),
                                                                                max_line(opts.max_line) {}

git2pp::blame_options::operator git_blame_options() const noexcept {
	return {GIT_BLAME_OPTIONS_VERSION, static_cast<uint32_t>(flags), min_match_characters, newest_commit, oldest_commit, min_line, max_line};
}


void git2pp::blame_deleter::operator()(git_blame * blm) const noexcept {
	if(owning)
		git_blame_free(blm);
}


std::uint32_t git2pp::blame::size() noexcept {
	return git_blame_get_hunk_count(blm.get());
}

const git_blame_hunk & git2pp::blame::operator[](std::uint32_t idx) noexcept {
	if(const auto hnk = git_blame_get_hunk_byindex(blm.get(), idx))
		return *hnk;
	else
		throw std::out_of_range(
		    static_cast<std::stringstream &>(std::stringstream{} << "idx=" << idx << " >= size()=" << size() << " when accessing a blame hunk").str());
}

const git_blame_hunk & git2pp::blame::at_line(std::uint32_t line_number) noexcept {
	if(const auto hnk = git_blame_get_hunk_byline(blm.get(), line_number))
		return *hnk;
	else
		throw std::out_of_range(
		    static_cast<std::stringstream &>(std::stringstream{} << "line_number=" << line_number << " nonexistant when accessing a blame hunk").str());
}

git2pp::blame git2pp::blame::buffer(blame & ref, const char * buf, std::size_t buffer_len) noexcept {
	git_blame * result;
	git_blame_buffer(&result, ref.blm.get(), buf, buffer_len);
	return {result};
}

git2pp::blame git2pp::blame::buffer(blame & ref, const char * buf) noexcept {
	return buffer(ref, buf, std::strlen(buf));
}

git2pp::blame git2pp::blame::buffer(blame & ref, const std::string & buf, std::size_t buffer_len) noexcept {
	return buffer(ref, buf.c_str(), buffer_len);
}

git2pp::blame git2pp::blame::buffer(blame & ref, const std::string & buf) noexcept {
	return buffer(ref, buf.c_str(), buf.size());
}


git2pp::blame::blame(git_blame * blm, bool owning) noexcept : blm(blm, {owning}) {}
