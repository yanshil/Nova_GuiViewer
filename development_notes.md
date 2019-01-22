# Note on Using Nova's openGL file

* ViewportManager: Partly use. Almost Rewrite based on VM's structure.
* Shader: Only modify the namespace from `Nova` to `opengl_gui_viewer` and the `#ifndef OGL_SHADER_H__`
* ResourceManager: Only modify the namespace.
* ShaderManager: `#ifndef OGL_SHADER_MANAGER_H__`
    * Delete the `ApplicationFatory` constructor dependency
    * Use in ViewportManager: `PrependSearchPaths` and `LoadFromFiles("camera")`
    * `camera.vs` and `camera.fs` renamed to `.vert` and `frag`. Also delete the version and MVP declaration because the following header has been included in Shader.h
```
const char* common_header = R"lang::GLSL(
#version 330 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
)lang::GLSL";
```

---

# ImGui

##  Widgets
* Basic
    * input float3
    * Checkbox: wireframe/not
* Columns
    * With border: Display Holes ID
* Popup
    * Modals: For holes modification
