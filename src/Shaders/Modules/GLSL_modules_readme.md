# Shader Modules Documentation

## Shader Module Structure and Conventions

To ensure efficient inclusion and usage of shader modules, each module is organized into separate files. This design allows only the necessary components to be loaded when a module is used. All modules adhere to the following conventions:

### 1. Module File Structure
Each module consists of **two files**:

- **`[module]_uniforms.glsl`** – Contains all uniforms, buffers, and preprocessor macros required for the module to function.  
  - If a uniform is a non-primitive type, its definition is included in this file.

- **`[module].glsl`** – Contains the module’s functions.  
  - If a function’s parameters or return type are non-primitive types, their definitions are included here.

### 2. Buffer Binding Conventions
If a module uses any buffers, the corresponding **binding points must be defined as preprocessor macros**. This ensures consistency and allows easy configuration when including the module in shaders.

---

## Table of Contents

- [Camera Module](#camera-module)
- [Color Module](#color-module)
- [Material Module](#material-module)
- [Math Module](#math-module)
- [Transform Module](#transform-module)

---

## Camera Module

The **Camera** module transforms vertices from world space into screen space.

### Include path
- `./Camera/Camera_uniforms.glsl`
- `./Camera/Camera.glsl`

### Structs
**CameraUniforms**
- viewProj : mat4
- at : vec3
- up : vec3
- eye : vec3

### Uniform Instances
- `cameraData` : `CameraUniforms`

### Functions
- `CameraViewProj(pos : vec4) : vec4`

---

## Color Module

The **Color** module is a simple utility.  
The `Color()` function returns the color provided through its uniform.

### Include path
- `./Color/Color_uniforms.glsl`
- `./Color/Color.glsl`

### Structs
**ColorUniforms**
- color : vec3

### Uniform Instances
- `colorData` : `ColorUniforms`

### Functions
- `Color() : vec4`

---

## Material Module

The **Material** module handles receiving and preparing a model's material for use by the **Light** module, including texturing.

### Functionality
- The `MaterialPrepare()` function returns data in a layout that matches exactly what the `LightCalculate()` function in the **Light** module expects.

### Include path
- `./Material/Material_uniforms.glsl`
- `./Material/Material.glsl`

### Structs
**MaterialUniforms**
- diffuseColorTex : vec4
- specularColorTex : vec4
- ambientColorEmissionTex : vec4
- shininess : float
- hasNormalTex : int

### Uniform Instances
- `materialDiffuseTex` : `sampler2D`
- `materialSpecularTex` : `sampler2D`
- `materialEmissionTex` : `sampler2D`
- `materialNormalTex` : `sampler2D`
- `materialData` : `MaterialUniforms`

### Functions
- MaterialPrepare(texCoord : vec2) : float[13]
  - \[0-2\]:  ambient color
  - \[3-5\]:  diffuse color
  - \[6-8\]:  specular color
  - \[9-11\]: emission color
  - \[12\]: shininess

---

## Math Module

The **Math** module does not use any uniforms, so the `Math_uniforms.glsl` file does not exist.  
This module is simple and provides utility functions, primarily to support computations related to the Bernstein basis.

### Include path
- `./Math.glsl`

### Functions
- `binomialCoeff(n : int, k : int) : float`
- `ipow(base : float, exponent : int) : float`

## Transform Module

The **Transform** module is responsible for positioning the model within the scene.  

### Structs
**TransformUniforms**
- world : mat4

### Uniform Instances
- `transformData` : `TransformUniforms`

### Functions
- `Transform(pos : vec4) : vec4`

---
