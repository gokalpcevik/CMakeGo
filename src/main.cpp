#include "argh/argh.h"
#include "cmake.h"
#include "logger.h"
#include "minimal_windows.h"
#include <iostream>

using namespace CMakeGo;

void ProcessOperation(argh::parser& cmdl, OperationType op_type);

int main(int argc, char** argv) {
    CMakeGo::InitLogger();
    argh::parser cmdl(argv);

    OperationType type = OperationType::None;

		if (cmdl[{"--help", "-h"}])
		{
				CGO_Info("\n --clean or -cl: Invoke clean target(Requires CMake ver.  3.0). \n Positional Args.: <build directory>");
				CGO_Info("\n --configure or -cfg: Configure CMake project. \n Positional Args.=<build directory> \n Parameters: --toolchain-file or -tch=<path to toolchain file>");
				return EXIT_SUCCESS;
		}

    // Main operations passed
    if (cmdl[{"--clean", "-cl"}]) {
        type = OperationType::Clean;
    } else if (cmdl[{"--configure", "-cfg"}]) {
        type = OperationType::Configure;
    } else if (cmdl[{"--database", "-db"}]) {
        type = OperationType::Database;
    } else if (cmdl[{"--build", "-b"}]) {
        type = OperationType::Build;
    } else if (cmdl[{"--rebuild", "-rb"}]) {
        type = OperationType::Rebuild;
    } 
    if (type == OperationType::None) {
        CGO_Error("Desired operation cannot be determined.");
        return EXIT_FAILURE;
    }

    CMakeGo::ProcessOperation(cmdl, type);

    return EXIT_SUCCESS;
}
