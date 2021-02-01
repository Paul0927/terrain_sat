This is an implementation of the algorithm described in "Multi-resolution Terrain Rendering Using Summed-Area Tables".

Requirements
1.Our demo has been tested on Windows 7/10 platforms with Nvidia Geforce 1080Ti GPU. The RAM of the computer would be recommended for 8/16 GB.
2.We develop these codes with C++ and OpenGL on the Microsoft Visual Studio 2017.
3.We use CMake to set the environments of our codes.

Quickstart
Download Terrain Data:
1. Download https://drive.google.com/file/d/15AOcK8hyQrmJJiwIErNMNe0FurP25jc6/view?usp=sharing
2. Unzip the "data.rar" file and put the data in the "terrain_sat/Large_Terrain" directory. The terrain data directory path is "terrain_sat/Large_Terrain/data".

Build
1. command: git clone https://github.com/Paul0927/terrain_sat.git
2. command: cd terrain_sat
3. command: mkdir build (The new directory path: terrain_sat/build)
4. command: cd build
5. command: cmake ../ -A x64 -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 15 2017"
6. Open "Large_Terrain.sln" in the "build" directory and set the environents with "Release" and "x64".
7. Click the "Debug" button in the the Microsoft Visual Studio 2017.

Camera control:
Key W: front
Key A: Left
Key D: right
Key S: back
Key C: bottom
Key Space: up

The initial scene will be the representative images, and also Figure 4, 7(bottom) in our paper.