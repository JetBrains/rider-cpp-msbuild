Cross-plafrom JetBrains Rider, C++, and MSBuild integration.

Overview
- Goal: Provide a minimal, cross-platform MSBuild layer for C++ in Rider that keeps existing MSBuild project files usable across OSes.
- Scope today: Lightweight targets/props to improve design-time experience and basic NMake builds outside of MSVC targets.

What works today (implemented)
- Design-time include discovery for C++ STD headers (targets/JetBrains.Rider.Cpp.targets)
  - During evaluation time the ResolveFrameworkReferences target does the following:
    - probes clang/gcc for its system include directories and prepends them to NMakeIncludeSearchPath
    - probes gcc for pre-processor defines and prepends them to NMakePreprocessorDefinitions
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

How this differs from similar projects
- Chuyu-Team/MSBuildCppCrossToolset (https://github.com/Chuyu-Team/MSBuildCppCrossToolset)
  - A modified copy of Microsoft's whole VCTargets tree with GCC/Clang toolsets; MSBuild itself compiles and links (ClCompile/Link mapped to gcc/clang flags).
  - Distributed as a release zip; activated by setting the VCTargetsPath environment variable, then building with dotnet msbuild.
  - Targets command-line builds; does not mention any IDE and does not provide design-time include/define discovery.
- roozbehid/dotnet-vcxproj (https://github.com/roozbehid/dotnet-vcxproj)
  - Makes dotnet build compile regular .vcxproj files with GCC/Clang/Emscripten, keeping them usable in Visual Studio.
  - Distributed as the GCCBuildTargets NuGet package (PackageReference) plus a dotnet new template.
  - Visual Studio is the only IDE it targets; it disables itself during design-time builds (DesignTimeBuild == true), so the IDE sees the normal MSVC setup.
- This repository
  - Focuses on the Rider IDE experience on macOS/Linux: during design-time it queries the real compiler (clang++/g++) for system include directories and predefined macros and feeds them into NMakeIncludeSearchPath/NMakePreprocessorDefinitions.
  - Does not replace VCTargets or compile via MSBuild: the build is delegated to your own build system through NMake*CommandLine properties; only minimal Build/Rebuild/Clean targets are provided.
  - Tiny and plain-XML, so it can be combined with the projects above if you need MSBuild-driven compilation.

Opportunities / Potential future
- github CI, 
  - check compilation targeting different platforms Mac, Linux, Windows, iOS, Android, Web, etc
  - inspectcode validate Rider code analysis results are similar as compilation
- Cross-toolchain discovery
  - Extend include/library path discovery to MSVC and GCC on all platforms.
  - Validate toolchain availability before the evaluation time.
- Broader cross-platform support
  - Provide Windows-friendly discovery that does not depend on POSIX utilities.

Distribution (not solved yet)
- There is no official distribution channel yet.
- The MIT license (see LICENSE) allows you to copy the files from `targets/` into your own repository and import them.