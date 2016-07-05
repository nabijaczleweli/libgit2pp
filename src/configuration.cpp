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

git2pp::configuration_entry git2pp::configuration::operator[](const char * name) const {
	git_config_entry * result;
	git_config_get_entry(&result, cfg.get(), name);
	return {result};
}

git2pp::configuration_entry git2pp::configuration::operator[](const std::string & name) const {
	return operator[](name.c_str());
}

std::int32_t git2pp::configuration::int32(const char * name) const noexcept {
	std::int32_t result;
	git_config_get_int32(&result, cfg.get(), name);
	return result;
}

std::int32_t git2pp::configuration::int32(const std::string & name) const noexcept {
	return int32(name.c_str());
}

void git2pp::configuration::int32(const char * name, std::int32_t value) noexcept {
	git_config_set_int32(cfg.get(), name, value);
}

void git2pp::configuration::int32(const std::string & name, std::int32_t value) noexcept {
	int32(name.c_str(), value);
}

std::int64_t git2pp::configuration::int64(const char * name) const noexcept {
	std::int64_t result;
	git_config_get_int64(&result, cfg.get(), name);
	return result;
}

std::int64_t git2pp::configuration::int64(const std::string & name) const noexcept {
	return int64(name.c_str());
}

void git2pp::configuration::int64(const char * name, std::int64_t value) noexcept {
	git_config_set_int64(cfg.get(), name, value);
}

void git2pp::configuration::int64(const std::string & name, std::int64_t value) noexcept {
	int64(name.c_str(), value);
}

bool git2pp::configuration::boolean(const char * name) const noexcept {
	int result;
	git_config_get_bool(&result, cfg.get(), name);
	return result;
}

bool git2pp::configuration::boolean(const std::string & name) const noexcept {
	return boolean(name.c_str());
}

void git2pp::configuration::boolean(const char * name, bool value) noexcept {
	git_config_set_bool(cfg.get(), name, value);
}

void git2pp::configuration::boolean(const std::string & name, bool value) noexcept {
	boolean(name.c_str(), value);
}

std::string git2pp::configuration::path(const char * name) const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_config_get_path(&buf, cfg.get(), name);

	return buf.ptr;
}

std::string git2pp::configuration::path(const std::string & name) const {
	return path(name.c_str());
}

std::string git2pp::configuration::string(const char * name) const {
	git_buf buf{};
	detail::quickscope_wrapper buf_cleanup{[&]() { git_buf_free(&buf); }};

	git_config_get_string_buf(&buf, cfg.get(), name);

	return buf.ptr;
}

std::string git2pp::configuration::string(const std::string & name) const {
	return string(name.c_str());
}

void git2pp::configuration::string(const char * name, const char * value) {
	git_config_set_string(cfg.get(), name, value);
}

void git2pp::configuration::string(const std::string & name, const char * value) {
	string(name.c_str(), value);
}

void git2pp::configuration::string(const char * name, const std::string & value) {
	string(name, value.c_str());
}

void git2pp::configuration::string(const std::string & name, const std::string & value) {
	string(name.c_str(), value.c_str());
}

std::vector<git2pp::configuration_entry> git2pp::configuration::multivar(const char * name, const char * matching) const {
	std::vector<configuration_entry> result;
	git_config_get_multivar_foreach(cfg.get(), name, matching,
	                                [](const git_config_entry * entry, void * result) {
		                                reinterpret_cast<std::vector<configuration_entry> *>(result)->push_back({entry});
		                                return 0;
		                              },
	                                &result);
	return result;
}

std::vector<git2pp::configuration_entry> git2pp::configuration::multivar(const std::string & name, std::experimental::optional<std::string> matching) const {
	if(matching)
		return multivar(name.c_str(), matching.value().c_str());
	else
		return multivar(name.c_str(), nullptr);
}

void git2pp::configuration::multivar(const char * name, const char * value, const char * matching) {
	git_config_set_multivar(cfg.get(), name, matching, value);
}

void git2pp::configuration::multivar(const std::string & name, const std::string & value, std::experimental::optional<std::string> matching) {
	if(matching)
		multivar(name.c_str(), value.c_str(), matching.value().c_str());
	else
		multivar(name.c_str(), value.c_str(), nullptr);
}

void git2pp::configuration::multivar_delete(const char * name, const char * matching) {
	git_config_delete_multivar(cfg.get(), name, matching);
}

void git2pp::configuration::multivar_delete(const std::string & name, std::experimental::optional<std::string> matching) {
	if(matching)
		multivar_delete(name.c_str(), matching.value().c_str());
	else
		multivar_delete(name.c_str(), nullptr);
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


git2pp::configuration_entry::configuration_entry(git_config_entry * r) : priority_level(static_cast<configuration_priority_level>(r->level)) {
	detail::quickscope_wrapper entry_free{[&]() { git_config_entry_free(r); }};

	name  = r->name;
	value = r->value;
}

git2pp::configuration_entry::configuration_entry(const git_config_entry * r)
      : name(r->name), value(r->value), priority_level(static_cast<configuration_priority_level>(r->level)) {}


std::string git2pp::global_configuration_path() {
	return ::configuration_path<decltype(&git_config_find_global), git_config_find_global>();
}

std::string git2pp::xdg_configuration_path() {
	return ::configuration_path<decltype(&git_config_find_xdg), git_config_find_xdg>();
}

std::string git2pp::system_configuration_path() {
	return ::configuration_path<decltype(&git_config_find_system), git_config_find_system>();
}
