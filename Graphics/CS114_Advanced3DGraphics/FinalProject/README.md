CS 114 Projects in Advanced 3D Computer Graphics 
Spring 2018 Final Project by Jeremy Taylor

3D Renderer framework that renders static meshes using physically based
shaders.

This code has been ran succesfully on the Windows 10 x64 platform.
The source code dependencies (in the /libs folder) are all either
header-only libraries, which will compile on any platform, or 
prebuilt binaries for x64 platforms. If you want to compile on 
a Win32/x86 platform with or without Visual Studio you will need 
to download the source code or prebuilt binaries for your platform.
Also it should be noted that glad.c (in libs/glad/src) must be compiled 
alongside the  project source code; if you use Visual Studio this is 
already set up but will need to be set up for other compilers. The directories
are set up in such a way that setting up and using some build tool like 
CMake should be straightforward.

The source code or binaries for the project's dependencies can be found at:
-Open Asset Importer Lib: http://www.assimp.org/
-GLFW: http://www.glfw.org/
The following dependencies are built as part of the project or included as 
header-only libraries:
OpenGL Loader-Generator: http://glad.dav1d.de/
OpenGL Mathematics: https://glm.g-truc.net/0.9.9/index.html
STB (Image functions): https://github.com/nothings/stb

If you want to modify the code to load your own assets, the best way to do so
is to import a model into Autodesk Maya, and use a Phong shader as the material.
The setup for base color and normal maps (use as tangent space normals) should be 
straightforward. Roughness maps should be set to Specular Color and metallic maps
should be set to Cosine Power (both are under Specular Shading). 
If you use another tool similar to Maya with an analagous material system, it may
be loaded properly if set up in this way. Most programs don't seem to export PBR materials
in model file formats (fbx, 3ds, etc.) so using something like Maya's Stingray PBS material
won't work. 
