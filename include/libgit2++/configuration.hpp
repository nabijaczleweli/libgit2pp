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
#include "transaction.hpp"
#include <experimental/optional>
#include <git2/config.h>
#include <memory>
#include <vector>


namespace git2pp {
	enum class configuration_priority_level {
		system_wide    = GIT_CONFIG_LEVEL_SYSTEM,
		xdg_compatible = GIT_CONFIG_LEVEL_XDG,
		global         = GIT_CONFIG_LEVEL_GLOBAL,
		local          = GIT_CONFIG_LEVEL_LOCAL,
		application    = GIT_CONFIG_LEVEL_APP,

		highest = GIT_CONFIG_HIGHEST_LEVEL,
	};


	class configuration_deleter {
	public:
		void operator()(git_config * config) const noexcept;
	};


	struct configuration_entry;

	class configuration : public guard {
	public:
		// not `default` because it's a keyword
		configuration default_config() noexcept;


		void add_file(const char * path, configuration_priority_level priority, bool force = false) noexcept;
		void add_file(const std::string path, configuration_priority_level priority, bool force = false) noexcept;

		configuration open_level(configuration_priority_level priority) const noexcept;
		configuration open_global() noexcept;

		configuration_entry operator[](const char * name) const;
		configuration_entry operator[](const std::string & name) const;

		std::int32_t int32(const char * name) const noexcept;
		std::int32_t int32(const std::string & name) const noexcept;
		void int32(const char * name, std::int32_t value) noexcept;
		void int32(const std::string & name, std::int32_t value) noexcept;

		std::int64_t int64(const char * name) const noexcept;
		std::int64_t int64(const std::string & name) const noexcept;
		void int64(const char * name, std::int64_t value) noexcept;
		void int64(const std::string & name, std::int64_t value) noexcept;

		bool boolean(const char * name) const noexcept;
		bool boolean(const std::string & name) const noexcept;
		void boolean(const char * name, bool value) noexcept;
		void boolean(const std::string & name, bool value) noexcept;

		std::string path(const char * name) const;
		std::string path(const std::string & name) const;

		std::string string(const char * name) const;
		std::string string(const std::string & name) const;
		void string(const char * name, const char * value);
		void string(const std::string & name, const char * value);
		void string(const char * name, const std::string & value);
		void string(const std::string & name, const std::string & value);

		std::vector<configuration_entry> multivar(const char * name, const char * matching = nullptr) const;
		std::vector<configuration_entry> multivar(const std::string & name, std::experimental::optional<std::string> matching = std::experimental::nullopt) const;
		void multivar(const char * name, const char * value, const char * matching = nullptr);
		void multivar(const std::string & name, const std::string & value, std::experimental::optional<std::string> matching = std::experimental::nullopt);
		void multivar_delete(const char * name, const char * matching = nullptr);
		void multivar_delete(const std::string & name, std::experimental::optional<std::string> matching = std::experimental::nullopt);

		template <class F>
		void foreach(F && func) const;
		template <class F>
		void foreach_matching(const char * match, F && func) const;
		template <class F>
		void foreach_matching(const std::string & match, F && func) const;

		transaction lock() noexcept;

		configuration() noexcept;
		configuration(const char * path) noexcept;
		configuration(const std::string & path) noexcept;
		configuration(const configuration & other) noexcept;

	private:
		friend class repository;

		configuration(git_config * cfg) noexcept;

		std::unique_ptr<git_config, configuration_deleter> cfg;
	};


	struct configuration_entry {
	public:
		std::string name;
		std::string value;
		configuration_priority_level priority_level;

	private:
		friend class configuration;

		configuration_entry(git_config_entry * ent);
		configuration_entry(const git_config_entry * ent);
	};


	std::string global_configuration_path();
	std::string xdg_configuration_path();
	std::string system_configuration_path();

	bool parse_configuration_boolean(const char * value) noexcept;
	bool parse_configuration_boolean(const std::string & value) noexcept;
	std::int32_t parse_configuration_int32(const char * value) noexcept;
	std::int32_t parse_configuration_int32(const std::string & value) noexcept;
	std::int64_t parse_configuration_int64(const char * value) noexcept;
	std::int64_t parse_configuration_int64(const std::string & value) noexcept;
	std::string parse_configuration_path(const char * value);
	std::string parse_configuration_path(const std::string & value);
}


template <class F>
void git2pp::configuration::foreach(F && func) const {
	git_config_foreach(cfg.get(), [](const git_config_entry * entry, void * func) { (*reinterpret_cast<F *>(func))(configuration_entry(entry)); }, &func);
}

template <class F>
void git2pp::configuration::foreach_matching(const char * match, F && func) const {
	git_config_foreach_match(cfg.get(), match, [](const git_config_entry * entry, void * func) { (*reinterpret_cast<F *>(func))(configuration_entry(entry)); },
	                         &func);
}

template <class F>
void git2pp::configuration::foreach_matching(const std::string & match, F && func) const {
	foreach_matching(match.c_str(), std::forward<F>(func));
}
