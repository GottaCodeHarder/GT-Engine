# GT-Engine

LINK TO REPOSITORY - (https://github.com/GottaCodeHarder/GT-Engine)
LINK TO TRELLO - (https://trello.com/b/i8PU5gIu/motors)

GT Engine is a Game Engine for educational purposes made by Samuel Navarro Soto and Pau Serra Colomer.
Using SDL (2.0.3), ImGui (1.50), PhysFs, Bullet (2.84), MathGeoLib (1.5), gl3w and parson (latest 2017)
MIT License. Copyright (c) 2017
LICENSE (https://github.com/GottaCodeHarder/GT-Engine/blob/master/LICENSE)

#UI EXPLANATION

###File 
- Exit (Esc)
- Load File: To load fbx
- Reset Scene: Clean Scene

###Tools 
- Random: To generate a random num between the values required
- Geometry: To test MathGeoLib
- Style Editor

###Windows 
- Configuration: Tools and information for the engine
		For example Application section to change the engine name or Renderer to watch FBX in wireframe
- Console: Output of the engine
- Heriarchy: Shows heriarchy of the game objects
- Properties: Shows components of the selected game objecr

###Help 
- About the engine, report a bug, documentation and latest release link.

#CAMERA CONTROL:

WASD + right click- for the FPS movement
Alt left+left click - To pivotate around the selected game object
F - To focus on the selected game object

#RELEASES

###Gt Engine v0.1.2:
Internal Structure
- Components Mesh, Material, Transform working properly
- Heriarchy working properly
- Clenaning extra code
- Some BUGS fixed like when loaded fbx resize camera

Menu Functions
- Added a load file option
- Added a reset scene option
- Added style editor

UI Heriarchy
- Shows all heriarchy of the meshes loaded
- Double click to select a game object
- Double click or click the arrow to show childs
- If mesh doesn't have a name numerate "Default #number"

UI Properties
- Checkbox to enable and disable game object
- Shows active components of the selected game object
- Mesh option shows num of triangles, num of vertex, checkbox to see aabb, aabb information
- Transform option shows local and global transform, dragbox to apply transformations to mesh
- Material option have a button to load textures to selected game object, texture information

Camera
- If mouse is hovering UI mousewheel+left and right click doesn't affect camera
- Press F focus the selected game object
- Alt+Right click pivotates around the selected game object

###Gt Engine v0.1.1:

UI
- Added new window to show all the heriarchy of the meshes
- Restructure UI windows.

Internal Structure
- Now working with heriarchy
- Using GameObjects + Components

###Gt Engine v0.1.0:

Added functionality: Adding a texture will replace the previous one.
Added Properties window.
Several features polish.
Several bug fixes.

###Gt Engine v0.0.4:

New main functionality: Now it can render 3D scenes with textures.
Configuration panel with lots of new functionality which include:
- Performance Display
- Window Options
- Theme Selector with 3 basic themes: Blue, Forest and Arc
- Hardware information (GPU and CPU)
- OpenGL renderer options
Several UI enhancements.
Added early stage Camera functionality: movement and focus.
Drag&Drop file loading functionality.
Currently, the Engine can recognize the file formats: bmp, ico, gif, jpg, jpeg, png, tif, tga, fbx, obj...

###Gt Engine v0.0.3

Added Parson.
Added PhysFS.
Added runtime console.
Hardware information shown.
Fixed issues with Bullet.
Information about the Engine.

###Gt Engine v0.0.2

Added MathGeoLib.
Added Geometry intersection test.
Added Random Generator.

###Gt Engine v0.0.1

Initial release.
ImGui fully functional.