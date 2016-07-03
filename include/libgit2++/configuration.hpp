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
#include <git2/config.h>
#include <memory>


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

	class configuration_entry_deleter {
	public:
		void operator()(git_config_entry * config) const noexcept;
	};


	class configuration_entry;

	class configuration : public guard {
	public:
		// not `default` because it's a keyword
		configuration default_config() noexcept;


		void add_file(const char * path, configuration_priority_level priority, bool force = false) noexcept;
		void add_file(const std::string path, configuration_priority_level priority, bool force = false) noexcept;

		configuration open_level(configuration_priority_level priority) const noexcept;
		configuration open_global() noexcept;
		configuration snapshot() noexcept;

		configuration() noexcept;
		configuration(const char * path) noexcept;
		configuration(const std::string & path) noexcept;
		configuration(const configuration & other) noexcept;

	private:
		friend class repository;

		configuration(git_config * cfg) noexcept;

		std::unique_ptr<git_config, configuration_deleter> cfg;
	};


	class configuration_entry : public guard {
	public:
		std::string name() const;
		std::string value() const;
		configuration_priority_level priority_level() const noexcept;

	private:
		friend class configuration;

		configuration_entry(git_config_entry * ent) noexcept;

		std::unique_ptr<git_config_entry, configuration_entry_deleter> ent;
	};


	std::string global_configuration_path();
	std::string xdg_configuration_path();
	std::string system_configuration_path();
}
