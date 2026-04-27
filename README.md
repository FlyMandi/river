## Introduction

This was originally a project about going through the vulkan tutorial and implementing a
prototype 3D engine in C++. After gaining some insight into application design from my
2D engine, game and map editor, I am now refactoring this codebase to utilize no third
party libraries over libc and the Operating System.

### State (DEMO):
- there's a fully resizeable window with correct and proper vulkan instantiation
- it shows a rotating, textured 3D model, loaded from .obj/.jpg files

![Screenshot of the river test project](assets/preview.png)

### What I've learned
- synchronization primitives (using fences & semaphores to sync GPU/CPU work)
- vulkan rasterization, viewports, scissors, multisampling
- queue families, the swapchain & present modes
- vertex & index buffers, vertex & fragment shaders
- uniform buffer objects and model view projection
- loading & sampling textures, texels, mipmaps
- depth buffering
- loading 3D models

### Next up:
- multisampling
- normals
- materials
- camera control
- a GUI (imgui / clay)
- a default project.
- project management, custom filetype

### Plans:
- add a wireframe view via hotkey
- multi-threading
- alternative renderers, like OpenGL / DirectX, maybe even Metal (mac support)
