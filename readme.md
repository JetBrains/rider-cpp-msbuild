RiderMSBuildCPP is a cross-platform MSBuild-style framework for C++ projects in JetBrains Rider. It enables using .sln, .vcxproj, .targets, and .props on macOS, Linux, and Windows.

Overview
- Goal: Provide a minimal, cross-platform MSBuild layer for C++ in Rider that keeps existing MSBuild project files usable across OSes.
- Scope today: Lightweight targets/props to improve design-time experience and basic NMake builds outside of MSVC targets.

What works today (implemented)
- Design-time include discovery for Clang (targets/JetBrains.Rider.Cpp.targets)
  - During design-time evaluation (when DesignTimeBuild=true), the ResolveFrameworkReferences target probes clang++ for its system include directories and prepends them to NMakeIncludeSearchPath. This improves header resolution and code insight in Rider.
- NMake substitution for basic build lifecycle (targets/nmake.substitution.props)
  - Defines Build, Rebuild, and Clean targets that call NMake*CommandLine properties (NMakeBuildCommandLine, NMakeReBuildCommandLine, NMakeCleanCommandLine) in the specified NMakeWorkingDirectory. This allows invoking typical NMake actions via MSBuild when MSVC targets are not present.
- Architecture-to-linker mapping (targets/nmake.substitution.props)
  - Maps Arch property values to the Link.TargetMachine setting: arm64→MachineARM64, x86_64→MachineX64, x86_32→MachineX86.
- Conservative toolset default
  - Sets PlatformToolset to Unknown to satisfy code analysis expectations when a concrete toolset is not selected.

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

Getting started (very basic)
- Ensure clang++ is installed and on PATH (macOS/Linux) if you want Clang include discovery.
- For NMake-based projects without MSVC targets, import targets/nmake.substitution.props into your .vcxproj to wire Build/Rebuild/Clean to your NMake*CommandLine properties.
- Rider will evaluate targets during design-time to enhance code insight; no manual steps are needed for the Clang include discovery aside from having clang++ available.


Samples
- Sample solution: samples/HelloNMake.sln
  - Contains an NMake-based C++ project (HelloNMake/HelloNMake.vcxproj) that imports this repository's targets/nmake.substitution.props and targets/JetBrains.Rider.Cpp.targets.
  - Build commands are driven by a simple Makefile in the project directory. The project maps Platform x64 to Arch=x86_64 to demonstrate the Link.TargetMachine mapping from nmake.substitution.props.
  - Open samples/HelloNMake.sln in Rider. On macOS/Linux, ensure clang++ (or any C++ compiler set via CXX) is installed and on PATH.
