Cross-plafrom JetBrains Rider, C++, and MSBuild integration.

Overview
- Goal: Provide a minimal, cross-platform MSBuild layer for C++ in Rider that keeps existing MSBuild project files usable across OSes.
- Scope today: Lightweight targets/props to improve design-time experience and basic NMake builds outside of MSVC targets.

What works today (implemented)
- Design-time include discovery for C++ STD headers (targets/JetBrains.Rider.Cpp.targets)
  - During evaluation time, the ResolveFrameworkReferences target probes clang++ for its system include directories and prepends them to NMakeIncludeSearchPath.
- NMake substitution for basic build lifecycle (targets/nmake.substitution.props)
  - Defines Build, Rebuild, and Clean targets that call NMake*CommandLine properties (NMakeBuildCommandLine, NMakeReBuildCommandLine, NMakeCleanCommandLine) in the specified NMakeWorkingDirectory. This allows invoking typical actions via MSBuild, when MSVC targets are not present.

Notes
- The Clang include discovery currently relies on clang++ being available on PATH and uses standard POSIX tools (sed/grep/tail), so it is primarily applicable on macOS/Linux.

Prerequisites:
- Ensure clang++ is installed and on PATH (macOS/Linux) if you want Clang include discovery.

Samples
- Sample solution: samples/HelloGnuMake.sln
  - Contains an NMake-based C++ project (HelloGnuMake/HelloGnuMake.vcxproj) that imports this repository's targets/nmake.substitution.props and targets/JetBrains.Rider.Cpp.targets.
  - Build commands are driven by a simple Makefile in the project directory. The project maps Platform x64 to Arch=x86_64 to demonstrate the Link.TargetMachine mapping from nmake.substitution.props.
  - Open samples/HelloGnuMake.sln in Rider. On macOS/Linux, ensure clang++ (or any C++ compiler set via CXX) is installed and on PATH.

Clarification: NMake* properties vs GNU Make
- You will see NMake* properties (for example: NMakeBuildCommandLine, NMakeReBuildCommandLine, NMakeCleanCommandLine, NMakeOutput, NMakeWorkingDirectory) used throughout the samples and targets. This is intentional: MSBuild evaluates these properties and Rider reads them during design-time to know how to run Build/Rebuild/Clean and to wire up debugging and other IDE features.
- The actual compilation/link step is performed by GNU Make via the Makefile in the project directory. The NMake*CommandLine properties simply invoke make (e.g., make -f Makefile all). We are not using MSVC NMake here; the provided Makefile uses GNU Make–specific syntax and should be executed by GNU make.
- In short: NMake* is the MSBuild/Rider integration surface; GNU Make is the tool that builds the executable.

Opportunities / Potential future
- Cross-toolchain discovery
  - Extend include/library path discovery to MSVC and GCC on all platforms.
  - Validate toolchain availability before the evaluation time.
- Broader cross-platform support
  - Provide Windows-friendly discovery that does not depend on POSIX utilities.
- Smart PlatformToolset detection
  - In the example project PlatformToolset is hard-coded to Clang_Mac.
