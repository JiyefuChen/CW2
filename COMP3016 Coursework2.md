youtube：https://youtu.be/0mcgfdCi_TE

------

## 1. How does the user interact with your executable? How do you open and control the software you wrote (exe file)?

The 3D plane control application mainly allows the user to interact with the plane control operation, and the user can control the position and attitude of the plane through the keyboard keys. Specifically:

- **W key**: acceleration
- **S key**: deceleration
- **Q key**: pull up
- **E key**: dive
- **A key**: left turn
- **D key**: right turn

The plane speed reaches more than 200 can use Q key pull up and fly, using A and D steering will slightly drive the plane to the corresponding direction side. The initial plane state is stationary on the ground, and the user can perform the acceleration described above during takeoff control.

First open CW2 file, find x64 file, open it and click Debug. Debug it contains CW2.exe. You can double-click the `CW2.exe` file to open the application program. The application contains many resource files. Therefore, ensure that the resource file and `CW2.exe` are in the same path. The resource files include the model, plane folder, and the `Assimp64.dll` model load library to this path.

If you want to configure the environment, there is a file called learnOpenGL-other.zip in the package, unzip it to C:\Users\Public\OpenGl, this is the library I use, which contains many libraries such as glew, glfw, assimp, etc. Open the CW2 file, search cw2.sln directly, and let it run. Following this process, you can open the file.

## 2. How does the program code work? How do the classes and functions fit together and who does what?

This program is a 3D aircraft control application developed by OpenGL. The OpenGL version is 4.6.0, the window library uses glfw, and the model loading uses assimp. The following will introduce the core classes of the program development:

1. **Mesh**: Completes the analysis and construction of 3D data, including vertices, indexes, textures, etc., and drives VAO, VBO and EBO to build bindings.
2. **Model**: Completes the analysis of model data into Mesh.
3. **CSpeed**: Used to calculate airplane speed, airplane Angle, airplane rendering.
4. **CWindow**: Encapsulates OpenGL window handle, window message corresponding operations.
5. **Camera**: Completes the realization of scene camera, including the interactive interface of camera position, attitude and user control.
6. **Shader**: Used for OpenGL Shader creation and compilation.
7. The main function draws and controls rendering of all scene objects, including aircraft, ground, sky box, camera, text object rendering, etc.

## 3. What makes your program special and how does it compare to similar things?

In the beginning, when I wasn't sure what to do. I looked up some existing programs online, and I saw different kinds of aircraft simulators, and I was inspired by that. At first, I learned from the existing program to understand the code, but most of the code is very huge, it takes me a long time to learn, in the learning, I understand the terrain is very important to this program, but I did not perfect grasp this knowledge. I think my special point is that the environment rendered is the mountain and the plane can be controlled with the keyboard.

I first looked up the opengl knowledge I should be able to use, from the basic API, how to create Windows, handle user input, draw graphics, etc. Later, I went to the Internet to learn about the libraries I might use, and I generally used GLFW, GLEW, GLM, and ASSIMP. GLEW is needed to create Windows, handle output, and manage OpenGL context libraries. GLM is required to provide the required mathematical operations, such as vector and matrix operations, transformations, etc. Assimp seems to be the most important for importing models in various formats. This library is almost a necessity if I intend to use complex 3D models in the simulator.