# SDL3-OpenGL-Base

A lightweight, object-oriented starter template for high-performance graphical applications. This boilerplate handles the tedious setup of SDL3 and OpenGL, allowing you to jump straight into creative coding within a structured environment.

---

## Key Features

- **Modern SDL3 Integration:** Leveraging the latest SDL3 features for window management and event handling.  
- **OpenGL 4.6 Ready:** Pre-configured graphics context with a robust debug logger.  
- **OOP Architecture:** Clean separation of concerns. `main.cpp` manages the lifecycle, while `MyApp` encapsulates your application logic (`Update` / `Render` / `Resize`).  
- **Asset Management:** Includes a ready-to-use image loading utility (STB-style via SDL_image) with support for PNG, JPG, and more.  
- **Vcpkg Compatible:** Optimized for easy dependency management.

---

## Tech Stack

- **Windowing / Input:** SDL3  
- **Graphics API:** OpenGL 4.6  
- **Image Loading:** SDL3_image  
- **Mathematics:** GLM (OpenGL Mathematics)  
- **UI:** Dear ImGui (Optional / Integrated)  
- **Package Manager:** vcpkg

---

## Architecture

The project is structured to separate the low-level initialization from the high-level application logic:

- **`main.cpp`:** Initializes SDL3, creates the OpenGL 4.6 context, and manages the main event loop.  
- **`MyApp`:** An abstract-like class where you implement your project-specific logic. It contains methods for `Update()`, `Render()`, and `Resize()`.  
- **Utility:** Includes helper functions like `ImageFromFile` for robust, UTF-8 compliant asset loading.

---

## Dependencies

This project uses vcpkg to manage its libraries. To ensure all features (like PNG and JPEG support) work correctly, you need the following packages:

### Required Packages

- **SDL3:** Core framework for windowing and input.  
- **SDL3_image:** Extension for loading various image formats.  
- **GLEW:** OpenGL Extension Wrangler for modern GL function loading.  
- **GLM:** Mathematics library for graphics (vectors, matrices).  
- **ImGui:** Immediate mode UI for debugging and tools.

---

## Installation

Run the following command in your terminal to install all necessary dependencies with the correct features:

```bash
vcpkg install sdl3 sdl3-image[png,jpeg,tiff,webp] glew glm imgui[sdl3-binding,opengl3-binding]
```
