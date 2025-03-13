## Introduction

This project is intended to be a simple Snake Clone in C++ (Hence the name Cnake) for learning purposes, to get familiar with C++ Game Development, the Vulkan Graphics API and Game Engine Development. I plan to expand on it with some features and/or changes compared to the classic Snake gameplay loop. 

#### The plan
| Element                    | Status        |
| ---                        | ---           |
| Project Structure          | Done          |
| Snake Asset                | Done          |
| Set up Building w/ PreMake | Done          |
| Draw a Window with GLFW    | Done          |
| Basic Engine logic         | In Progress   |
| Basic Movement & collision | Not Started   |
| Scoring System             | Not Started   |
| Gameplay Expansion         | Not Started   |

## Build from source guide

In case you want to build this code from source:

1. Clone this repository into a local folder
2. Run the included `.\setup.ps1` to make sure you have all the prerequisites. This will install the VulkanSDK and the latest premake5 binary, if missing.
3. If you have Visual Studio 2022, you can skip steps 4&5 by running `.\build; .\run` (will use `MSBuild.exe` to compile)
4. Otherwise, compile shaders with `.\compile_shaders.ps1`(uses `glslc.exe` from the VulkanSDK)
5. Use `premake5` to generate any IDE project files you want (Visual Studio, gmake, xCode, CodeLite) and compile.

## Project Resources

These are all my tools of choice to build this project.

#### C++

C++ is not only a language that I was already partly familiar with, but also a great language that facilitates low-level control, while not making you manage all of the memory manually, so it's fairly easy to build a memory-safe application. I want to get better at C++ within Game Development, specifically.

#### Vulkan

I chose the Vulkan API since I'd read that you can gain quite a significant amount of control over the performance of the application. Plus, my goal is to learn Vulkan in order to use it to build larger-scale 3D applications.

#### Libraries

I'm using GFLW to draw and manage the window, since it makes my life easier and I don't have to touch Win32.

GLM is what I've ran with for graphics arithmetics operations.

#### Building with PreMake

Building cross-plaform and cross-IDE is a fairly simple task with PreMake and I like the fact you write in lua. Also I've heard many people prefer it over CMake, so I'll try it here. Also always good to know the ins and outs of a build system.

#### Text Editor/IDE

For editing text, I use neovim with [my custom configuration](https://github.com/FlyMandi/dotfiles/tree/main/nvim/), specifically for C++ I use `clang` as an LSP and write a simple `compile_flags.txt` with the include folders, which clang can pick up on. I compile via `.\build` (MSBuild.exe). For debugging/profiling, I simply run `.\vs2022`, which builds, compiles & opens the project in Visual Studio 2022 for me. 

