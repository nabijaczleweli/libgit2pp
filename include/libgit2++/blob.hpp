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
#include <cstdint>
#include <git2/blob.h>
#include <memory>
#include <string>


namespace git2pp {
	class blob_deleter {
	public:
		bool owning;

		void operator()(git_blob * blb) const noexcept;
	};


	class repository;

	class blob : public guard {
	public:
		bool binary() const noexcept;
		const git_oid & id() const noexcept;
		repository owner() const noexcept;

		std::pair<const void *, std::uint64_t> raw() const noexcept;
		std::string filtered(const char * as_path, bool check_for_binary_data = true);
		std::string filtered(const std::string & as_path, bool check_for_binary_data = true);

	private:
		friend class repository;

		blob(git_blob * blb, bool owning = true) noexcept;

		std::unique_ptr<git_blob, blob_deleter> blb;
	};
}
