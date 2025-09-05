RiderMSBuildCPP is a cross-platform MSBuild-style framework for C++ projects in JetBrains Rider. It enables using .sln, .vcxproj, .targets, and .props on macOS, Linux, and Windows.

Overview
- Goal: Provide a minimal, cross-platform MSBuild layer for C++ in Rider that keeps existing MSBuild project files usable across OSes.
- Scope today: Lightweight targets/props to improve design-time experience and basic NMake builds outside of MSVC targets.

What works today (implemented)
- Design-time include discovery for C++ STD headers (targets/JetBrains.Rider.Cpp.targets)
  - During design-time evaluation (when DesignTimeBuild=true), the ResolveFrameworkReferences target probes clang++ for its system include directories and prepends them to NMakeIncludeSearchPath. This improves header resolution and code insight in Rider.
- NMake substitution for basic build lifecycle (targets/nmake.substitution.props)
  - Defines Build, Rebuild, and Clean targets that call NMake*CommandLine properties (NMakeBuildCommandLine, NMakeReBuildCommandLine, NMakeCleanCommandLine) in the specified NMakeWorkingDirectory. This allows invoking typical NMake actions via MSBuild when MSVC targets are not present.
- Architecture-to-linker mapping (targets/nmake.substitution.props)
  - Maps Arch property values to the Link.TargetMachine setting: arm64→MachineARM64, x86_64→MachineX64, x86_32→MachineX86.

Notes
- The Clang include discovery currently relies on clang++ being available on PATH and uses standard POSIX tools (sed/grep/tail), so it is primarily applicable on macOS/Linux.

Opportunities / Future work
- Cross-toolchain discovery
  - Extend include/library path discovery to MSVC and GCC on all platforms.
  - Validate toolchain availability and surface actionable messages during design-time.
- Broader cross-platform support
  - Provide Windows-friendly discovery that does not depend on POSIX utilities.
- Richer MSBuild integration
  - Additional targets/props for configuration/platform handling, custom compile/link steps, and diagnostics.
- Smart PlatformToolset detection
  - In the example project PlatformToolset is hard-coded to Clang_Mac.

Getting started (very basic)
- Ensure clang++ is installed and on PATH (macOS/Linux) if you want Clang include discovery.
- For NMake-based projects without MSVC targets, import targets/nmake.substitution.props into your .vcxproj to wire Build/Rebuild/Clean to your NMake*CommandLine properties.
- Rider will evaluate targets during design-time to enhance code insight; no manual steps are needed for the Clang include discovery aside from having clang++ available.


Samples
- Sample solution: samples/HelloNMake.sln
  - Contains an NMake-based C++ project (HelloNMake/HelloNMake.vcxproj) that imports this repository's targets/nmake.substitution.props and targets/JetBrains.Rider.Cpp.targets.
  - Build commands are driven by a simple Makefile in the project directory. The project maps Platform x64 to Arch=x86_64 to demonstrate the Link.TargetMachine mapping from nmake.substitution.props.
  - Open samples/HelloNMake.sln in Rider. On macOS/Linux, ensure clang++ (or any C++ compiler set via CXX) is installed and on PATH.

Clarification: NMake* properties vs GNU Make
- You will see NMake* properties (for example: NMakeBuildCommandLine, NMakeReBuildCommandLine, NMakeCleanCommandLine, NMakeOutput, NMakeWorkingDirectory) used throughout the samples and targets. This is intentional: MSBuild evaluates these properties and Rider reads them during design-time to know how to run Build/Rebuild/Clean and to wire up debugging and other IDE features.
- The actual compilation/link step is performed by GNU Make via the Makefile in the project directory. The NMake*CommandLine properties simply invoke make (e.g., make -f Makefile all). We are not using MSVC NMake here; the provided Makefile uses GNU Make–specific syntax and should be executed by GNU make.
- In short: NMake* is the MSBuild/Rider integration surface; GNU Make is the tool that builds the executable.

Where to see this in the sample
- samples/HelloNMake/HelloNMake.vcxproj defines NMakeBuildCommandLine, NMakeReBuildCommandLine, and NMakeCleanCommandLine to call GNU make.
- samples/HelloNMake/Makefile is a GNU Make file (uses ?=, :=, order-only prerequisites, etc.).
- targets/nmake.substitution.props provides minimal Build/Rebuild/Clean targets that forward to the NMake*CommandLine properties so MSBuild can invoke the same commands when building from Rider.
