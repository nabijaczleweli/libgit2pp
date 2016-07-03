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


#include "libgit2++/configuration.hpp"
#include "libgit2++/detail/scope.hpp"


template <class F, F func>
static inline std::string configuration_path() {
	git2pp::guard grd;

	git_buf buf{};
	git2pp::detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	func(&buf);

	return buf.ptr;
}


git2pp::configuration git2pp::configuration::default_config() noexcept {
	guard grd;

	git_config * result;
	git_config_open_default(&result);
	return {result};
}


void git2pp::configuration::add_file(const char * path, configuration_priority_level priority, bool force) noexcept {
	git_config_add_file_ondisk(cfg.get(), path, static_cast<git_config_level_t>(priority), force);
}

void git2pp::configuration::add_file(const std::string path, configuration_priority_level priority, bool force) noexcept {
	add_file(path.c_str(), priority, force);
}

git2pp::configuration git2pp::configuration::open_level(git2pp::configuration_priority_level priority) const noexcept {
	git_config * result;
	git_config_open_level(&result, cfg.get(), static_cast<git_config_level_t>(priority));
	return {result};
}

git2pp::configuration git2pp::configuration::open_global() noexcept {
	git_config * result;
	git_config_open_global(&result, cfg.get());
	return {result};
}

git2pp::configuration git2pp::configuration::snapshot() noexcept {
	git_config * result;
	git_config_snapshot(&result, cfg.get());
	return {result};
}


git2pp::configuration::configuration() noexcept {
	git_config * result;
	git_config_new(&result);
	cfg.reset(result);
}

git2pp::configuration::configuration(const char * path) noexcept {
	git_config * result;
	git_config_open_ondisk(&result, path);
	cfg.reset(result);
}

git2pp::configuration::configuration(const std::string & path) noexcept : configuration(path.c_str()) {}

git2pp::configuration::configuration(git_config * r) noexcept : cfg(r) {}


void git2pp::configuration_deleter::operator()(git_config * cfg) const noexcept {
	git_config_free(cfg);
}

void git2pp::configuration_entry_deleter::operator()(git_config_entry * ent) const noexcept {
	git_config_entry_free(ent);
}


std::string git2pp::configuration_entry::name() const {
	return ent->name;
}

std::string git2pp::configuration_entry::value() const {
	return ent->value;
}

git2pp::configuration_priority_level git2pp::configuration_entry::priority_level() const noexcept {
	return static_cast<configuration_priority_level>(ent->level);
}


git2pp::configuration_entry::configuration_entry(git_config_entry * r) noexcept : ent(r) {}


std::string git2pp::global_configuration_path() {
	return ::configuration_path<decltype(&git_config_find_global), git_config_find_global>();
}

std::string git2pp::xdg_configuration_path() {
	return ::configuration_path<decltype(&git_config_find_xdg), git_config_find_xdg>();
}

std::string git2pp::system_configuration_path() {
	return ::configuration_path<decltype(&git_config_find_system), git_config_find_system>();
}
