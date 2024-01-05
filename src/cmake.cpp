#include "cmake.h"

bool CMakeGo::ProcessOperation(argh::parser& cmdl, OperationType op_type) {
    switch (op_type) {
    case OperationType::Build: {

        // If build directory is specified use that, else use 'build'.
        std::string build_directory = "build";
        if (!cmdl[1].empty()) {
            build_directory = cmdl[1];
        } else {
            CGO_Warn("Build directory not specified, defaulting to 'build'");
        }

        std::string config;
        // Default config is Debug
        cmdl("config", "Debug") >> config;

        // Target
        std::string target;
        cmdl("target", "ALL_BUILD") >> target;

        CGO_Info("Attempting to build.");
        return (Build(config, build_directory, target));
    }
    case OperationType::Rebuild: {
        // If build directory is specified use that, else use 'build'.
        std::string build_directory = "build";
        if (!cmdl[1].empty()) {
            build_directory = cmdl[1];
        } else {
            CGO_Warn("Build directory not specified, defaulting to 'build'");
        }

        std::string config;
        // Default config is Debug
        cmdl("config", "Debug") >> config;

        // Rebuild is just clean + build ???
        if (!Clean(build_directory)) {
            return false;
        }

        return (Build(config, build_directory, "ALL_BUILD"));
    }
    case OperationType::Configure: {

        // If build directory is specified use that, else use 'build'.
        std::string build_directory = "build";
        if (!cmdl[1].empty()) {
            build_directory = cmdl[1];
        } else {
            CGO_Warn("Build directory not specified, defaulting to 'build'");
        }

        std::string toolchain_file = "C:/vcpkg/scripts/buildsystems/vcpkg.cmake";
        if (cmdl({"-tch", "--toolchain-file"})) {
            toolchain_file = cmdl({"-tch", "--toolchain-file"}).str();
        }

        CGO_Info("Attempting to configure.");
        return (Configure(build_directory, toolchain_file, false));
    }
    case OperationType::Clean: {
        // Needs CMake version > 3.0
        // If build directory is specified use that, else use 'build'.
					std::string build_directory = "build";
        if (!cmdl[1].empty()) {
            build_directory = cmdl[1];
        } else {
            CGO_Warn("Build directory not specified, defaulting to 'build'");
        }

        return Clean(build_directory);
    }
    case OperationType::Database: {
        std::string toolchain_file = "C:/vcpkg/scripts/buildsystems/vcpkg.cmake";
        if (cmdl({"-tch", "--toolchain-file"})) {
            toolchain_file = cmdl({"-tch", "--toolchain-file"}).str();
        }
        return BuildDatabase(toolchain_file);
    }
    case OperationType::None:
        break;
    }

    return true;
}

bool CMakeGo::Build(const std::string& config, const std::string& build_directory, const std::string& target) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    std::string cmdline =
        fmt::format("cmake.exe --build {0} --target {1} --config {2}", build_directory, target, config);
    if (!CreateProcessA(NULL, cmdline.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CGO_Error("[Build] CreateProcess failed: {0}", ::GetLastError());
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

bool CMakeGo::Configure(std::string const& build_directory, std::string const& toolchain_file,
                        bool export_compile_commands) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateDirectoryA(build_directory.c_str(), NULL);
    std::string cmdline = fmt::format(
        "cmake.exe --no-warn-unused-cli -DCMAKE_TOOLCHAIN_FILE:STRING={0} \"-B{1}\" -DVCPKG_TARGET_TRIPLET=x64-windows",
        toolchain_file, build_directory);
    if (!CreateProcessA(NULL, cmdline.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CGO_Error("[Configure] CreateProcess failed: {0}", ::GetLastError());
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

bool CMakeGo::Clean(const std::string& build_directory) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!std::filesystem::exists(build_directory)) {
        CGO_Error("[Clean] Directory {0} does not exist.", build_directory);
        return false;
    }

    std::string cmdline = fmt::format("cmake.exe --build {0} --target clean", build_directory);
    if (!CreateProcessA(NULL, cmdline.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CGO_Error("[Clean] CreateProcess failed: {0}", ::GetLastError());
        return false;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return true;
}

bool CMakeGo::BuildDatabase(std::string const& toolchain_file) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Create temporary directory
    std::filesystem::path const temp = "compile_commands_temp_dir";
    // Try & remove in case there are any directories
    std::filesystem::remove_all(temp);
    std::filesystem::create_directory(temp);

    // Configure the project. Currently(?) need to invoke this using VS Dev Command environment
    std::string cmdline =
        fmt::format("cmake.exe --no-warn-unused-cli -DCMAKE_TOOLCHAIN_FILE:STRING={0} \"-B{1}\" "
                    "-DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE -GNinja -DVCPKG_TARGET_TRIPLET=x64-windows",
                    toolchain_file, temp.string());
    if (!CreateProcessA(NULL, cmdline.data(), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CGO_Error("[DB] CreateProcess failed: {0}", ::GetLastError());
        return false;
    }

    // Wait for configuration to be done (?) Do we need to build?
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    bool success = std::filesystem::copy_file(temp / "compile_commands.json", "compile_commands.json",
                                              std::filesystem::copy_options::overwrite_existing);
    if (!success) {
        CGO_Error("[DB] Copy file has failed.");
    }
    CGO_Info("compile_commands.json was copied succesfully into the called working directory.");
    std::filesystem::remove_all(temp);
    CGO_Info("Temporary database directory was deleted.");
    return true;
}
