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


#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "util.hpp"
#include <string>


#ifdef _WIN32
#include <shellapi.h>


void remove_directory(const char * path) {
	std::string out_path = path;
	out_path.push_back('\0');

	SHFILEOPSTRUCT op{};
	op.wFunc  = FO_DELETE;
	op.pFrom  = out_path.c_str();
	op.fFlags = FOF_NOCONFIRMATION | FOF_NOCONFIRMMKDIR | FOF_NOERRORUI | FOF_SILENT;
	SHFileOperationA(&op);
}
#else
#include <cstdlib>


using namespace std::literals;


// Processes are cheap enough on Linux
void remove_directory(const char * path) {
	static_cast<void>(system(("rm -rf '"s + path + '\'').c_str()));
}
#endif
