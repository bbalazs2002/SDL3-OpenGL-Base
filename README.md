# SDL3-OpenGL-Base

A lightweight, object-oriented starter template for high-performance graphical applications. This boilerplate handles the tedious setup of SDL3 and OpenGL, allowing you to jump straight into creative coding within a structured environment.

---

## Key Features

- **Modern SDL3 Integration:** Leveraging the latest SDL3 features for window management and event handling.
- **OpenGL 4.6 Ready:** Pre-configured graphics context with a robust debug logger.
- **OOP Architecture:** Clean separation of concerns. `main.cpp` manages the lifecycle, while `MyApp` encapsulates your application logic (`Update`, `Render`, `Resize`, etc.).
- **Asset Management:** Includes a ready-to-use image loading utility (STB-style via SDL_image) with support for PNG, JPG, and more.
- **3D Model Loading:** Integrated tinyobjloader support for loading `.obj` files with full `.mtl` material handling, enabling rendering of complex 3D models.
- **Dockable ImGui UI:** Dear ImGui with docking support — panels can be freely rearranged, resized, and docked to any edge of the window.
- **Vcpkg Compatible:** Optimized for easy dependency management.

---

## Tech Stack

- **Windowing / Input:** SDL3
- **Graphics API:** OpenGL 4.6
- **Image Loading:** SDL3_image
- **Model Loading:** tinyobjloader
- **Mathematics:** GLM (OpenGL Mathematics)
- **UI:** Dear ImGui (Docking branch)
- **Package Manager:** vcpkg

---

## Architecture

The project is structured to separate low-level initialization from high-level application logic, while maintaining a clean and scalable directory layout.

### Core Structure

- **`main.cpp`:** Initializes SDL3, creates the OpenGL 4.6 context, configures ImGui, and manages the main event loop.
- **`MyApp`:** Encapsulates application-specific logic with `Update()`, `Render()`, `Resize()`, etc. methods.

### Project Layout

- **`Assets/`:** Contains static resources such as textures and 3D models (`.obj`, `.mtl`).

- **`src/`:** Main source directory
  - **`Headers/`:** C++ header files. Most components are header-only for simplicity and compile-time optimization.
  - **`Interfaces/`:** Abstract interfaces defining core contracts and promoting decoupled design.
  - **`Shaders/`:** GLSL shader programs used in the OpenGL rendering pipeline.
  - **`Sources/`:** Implementation files (`.cpp`) for non-header-only components.
  - **`Utils/`:** Utility modules, including:
    - Texture and shader loading helpers
    - Camera system (`Camera`, `CameraManipulator`)
    - Logging system (`Log`) for debug and runtime diagnostics

### Utilities

- **Asset Loading:** UTF-8 compliant texture loading and `.obj` model loading with full material (`.mtl`) support.
- **Rendering Support:** Predefined helpers for shader compilation and OpenGL resource management.

### Shaders

The project includes a modular shader system designed for reuse and clarity.

- **`Shaders/`:** Contains all GLSL programs used in the rendering pipeline.

- **`Shaders/Modules/`:** Reusable GLSL modules shared across multiple shaders (e.g., common functions, material utilities).  
  - Each module is documented separately. See: **[Shader Modules README](./src/Shaders/Modules/GLSL_modules_readme.md)**  

- **Built-in Shaders:**
  - **Model Shader:** A simple built-in shader for rendering 3D models with basic material support.
  - **Skybox Shader:** Simple environment rendering for background and scene context.
  - **Axes Shader:** Renders orientation axes at the screen center, useful for spatial reference and debugging.

---

## ImGui Docking

The project uses the ImGui docking branch, providing a professional multi-panel UI system.

### How it works

`MyApp::RenderGUI()` delegates dock space setup to a dedicated `RenderImGuiDockSpace()` helper, which creates a fullscreen invisible host window and registers a `DockSpace` over the entire viewport.

### Adding new panels

Any `ImGui::Begin()` / `ImGui::End()` block placed after `RenderImGuiDockSpace()` inside `RenderGUI()` is automatically dockable. To assign a new panel to a default position, add a `DockBuilderDockWindow()` call inside the layout initialization block in `RenderImGuiDockSpace()`:

```cpp
ImGui::DockBuilderDockWindow("My New Panel", dock_right);
```

### Viewport flag (optional)

The `IMGUI_VIEWPORTSENABLE` preprocessor flag enables multi-viewport support, allowing panels to be dragged outside the application window and placed on a second monitor. It is disabled by default. To enable it, uncomment the corresponding line in `CMakeLists.txt`:

```cmake
# IMGUI_VIEWPORTSENABLE
```

---

## Dependencies

This project uses vcpkg to manage its libraries. To ensure all features (like PNG and JPEG support) work correctly, you need the following packages.

### Required Packages

- **SDL3:** Core framework for windowing and input.
- **SDL3_image:** Extension for loading various image formats.
- **GLEW:** OpenGL Extension Wrangler for modern GL function loading.
- **GLM:** Mathematics library for graphics (vectors, matrices).
- **ImGui (docking branch):** Immediate mode UI with dockable panels.
- **tinyobjloader:** Lightweight OBJ loader with MTL material support.

---

## Installation

Run the following command in your terminal to install all necessary dependencies with the correct features:

```bash
vcpkg install sdl3 sdl3-image[png,jpeg,tiff,webp] glew glm imgui[sdl3-binding,opengl3-binding,docking-experimental] tinyobjloader
```

### Upgrading from a previous version

If you previously installed `imgui` without the `docking-experimental` feature, reinstall it:

```bash
vcpkg remove imgui
vcpkg install imgui[sdl3-binding,opengl3-binding,docking-experimental]
```

---

## Keyboard Shortcuts

| Shortcut | Action |
|---|---|
| `CTRL + F5` | Hot-reload all shaders |
| `ALT + Enter` | Toggle fullscreen |
| `CTRL + F1` | Toggle ImGui visibility |
| `Escape` | Quit |
