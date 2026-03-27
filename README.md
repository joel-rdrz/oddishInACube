# Oddish In A Cube

Welcome to the **Oddish In A Cube**! Basically just oddish lowkey drowning but he kinda chill about it so that we like him. This is a C++ OpenGL application.

## Getting Started in Visual Studio

To build and run this project successfully using Visual Studio, please follow the setup instructions below.

### 1. Add Source Files
After cloning the repository and opening the project in Visual Studio, make sure to include the source files in your project:
- Go to the **Solution Explorer**.
- Right-click on your project -> **Add** -> **Existing Item...**
- Navigate to the `src` folder (and any other relevant directories) and select all `.cpp` files to add them to your project.

### 2. Configure Project Properties
You will need to configure your project properties so it can properly find and link the required OpenGL libraries (FreeGLUT, GLEW, OpenGL).

Right-click your project in the **Solution Explorer** and select **Properties**. Ensure that the **Configuration** is set to **All Configurations**.

#### C++ Settings (Include Directories)
- Navigate to **C/C++** -> **General** -> **Additional Include Directories**.
- Add the paths to all required include directories (such as the `Include/` folder in this repository, as well as the include folders for FreeGLUT and GLEW).

#### Linker Settings (Library Directories & Dependencies)
- Navigate to **Linker** -> **General** -> **Additional Library Directories**.
- Add the required `.lib` paths for FreeGLUT and GLEW.
- Navigate to **Linker** -> **Input** -> **Additional Dependencies**.
- Add the following exact string to link the necessary libraries:
  ```
  freeglut.lib;opengl32.lib;glew32.lib;$(CoreLibraryDependencies);%(AdditionalDependencies)
  ```

### 3. Build and Run
Once the source files are added and the properties are configured, you can build and run the project! If you encounter any missing DLL errors when running, ensure that `freeglut.dll` and `glew32.dll` are in your executable's output directory.