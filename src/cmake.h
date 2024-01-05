#pragma once
#include "logger.h"
#include "argh/argh.h"
#include <iostream>
#include <filesystem>
#include "minimal_windows.h"

namespace CMakeGo {

enum class OperationType : uint8_t { Build = 0, Rebuild, Configure, Clean, Database, None };

struct Operation {
    OperationType type = OperationType::Build;
};

bool ProcessOperation(argh::parser& cmdl, OperationType op_type);

bool Build(std::string const& config, std::string const& build_directory, std::string const& target);
bool BuildDatabase(std::string const& toolchain_file);
bool Clean(std::string const& build_directory);
bool Configure(std::string const& build_directory, std::string const& toolchain_file, bool export_compile_commands);

} // namespace CMakeGo
