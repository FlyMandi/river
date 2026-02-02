## Introduction

This is a cross-platform 3D Game Engine that utilizes the Vulkan Graphics API.
Using this project to learn as I go about everything related to graphics & low-level engine programming.

I've used lots of resources to learn Vulkan with C++ but left out a lot of the (frankly unnecessary) OOP-ness of common implementations.

I want this to be my engine, not to reinvent the wheel, but to write it for the sake of learning and write a simple game or two with it. It'll not only teach me about engine programming, but force me to use my own product, so to say.

### Concepts this project has taught me so far:
- building a graphical application ""from scratch""
- writing platform-agnostic code, no direct interface with WIN32/POSIX APIs
- debugging said code (currently via logging, raddbg and renderdoc)
- synchronization primitives (using fences & semaphores to sync GPU/CPU work)
- vulkan rasterization, viewports, scissors, multisampling
- queue families, the swapchain & present modes
- vertex & index buffers, vertex & fragment shaders
- uniform buffer objects and model view projection
- loading & sampling textures, texels
- depth buffering
- loading 3D models

### State (DEMO):
- there's a fully resizeable window with correct and proper vulkan instantiation
- it shows a rotating, textured 3D model, loaded from a .obj file

![Screenshot of the river test project](assets/preview.png)

### Next up:
- mipmaps
- multisampling
- camera control
- a GUI (imgui / clay)
- a default project.
- project management, custom filetype

### Plans:
- learn and implement material shaders, compute shaders, etc
- create an API to write games with (no scripting, pure C/C++)
- provide API callbacks (like glfwSetFramebufferSizeCallback);
- add a wireframe view via hotkey
- multi-threading
- anything else I'm motivated enough to implement, like for example:
    - replacing GLFW with my own cross-platform windowing implementation (Win32, macOS, X11, Wayland)
    - implementing OpenGL / DirectX / Metal rendering... maybe for the far future
    - replacing stb with my own image parser and tinyobj with my own obj parser
