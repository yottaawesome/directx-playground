# DirectX 12 Playground

This is a repo for playing around with DirectX 12 and game programming in C++ -- something I don't do enough of.

## Repo structure

All source code of interest is located in `src`. Each project in `src` is contained in its own directory and explores a different facet of DirectX 12. A readme file is contained in each of these directories that gives additional information on what the project does.

## Using the code

All code is built with Visual Studio 2026 Community Edition and is targeting the latest Windows SDK. As long as you have these two installed, using the code should just be a case of cloning this repo, before opening, building and running the projects in VS2026. Some samples use vcpkg to pull in additional dependencies for things like meshes and texture loading.

Some projects are configured to use the preview MSVC toolset (as I like to use the bleeding edge), so will not compile without that toolset. You can either install the preview toolset via Visual Studio or simply revert to the regular release MSVC toolset in the affected project's settings.
