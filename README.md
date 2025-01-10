# IBModeler 2.0

IBModeler (Image-Based Modeler) is a software application designed to create 3D models from photographs of real-world objects. It enables users—such as 3D artists, game designers, and movie animators—to generate 3D objects efficiently.

## Overview

The software operates by allowing users to photograph an object from multiple angles. These images are then processed to construct a 3D model, which can be exported to various 3D editing software for further refinement, animation, or 3D printing.

IBModeler addresses the challenge of quickly and easily creating 3D models without the need for extensive manual modeling. By automating the conversion of photographs into 3D representations, it streamlines the workflow for professionals in computer graphics and multimedia, reducing the time and effort required to produce accurate 3D models.

The software has been recognized in the industry, with over 3,000 downloads and features in publications like 3D World Magazine.

## System Requirements

- Microsoft Windows (Windows 95/98/NT/2000/XP)
- Microsoft Visual Studio 6.0
- Microsoft Foundation Classes (MFC)

## Installation Instructions

1. **Prerequisites**

   - Ensure Microsoft Visual Studio 6.0 is installed on your system
   - Verify MFC libraries are installed with Visual Studio

2. **Project Setup**

   - Clone or download this repository to your local machine
   - Open Visual Studio 6.0
   - Go to File -> Open Workspace
   - Navigate to the project directory and select `IBModeler.dsw`

3. **Build Configuration**

   - The project should be configured for Win32 platform
   - Select either Debug or Release configuration from the Build menu
   - Ensure all required dependencies are properly set in the project settings

4. **Building the Project**

   - Build -> Build Solution (or press F7)
   - If you encounter any include path errors:
     - Go to Tools -> Options -> Directories
     - Add the path to your MFC include directories
     - Add the path to your Windows SDK include directories

5. **Running the Application**
   - Press F5 to run in debug mode
   - Or navigate to the Debug/Release folder and run the executable

## Project Structure

- `definitions.cpp` - Core definitions and utility functions
- `Dialogs.cpp` - Dialog implementations
- `errorConstants.h` - Error code definitions
- `cameraorntn.h/cpp` - Camera orientation handling

## Common Issues

- If you encounter "#include errors" for `afxext.h`:
  1. Open Tools -> Options -> Directories
  2. Select "Include files" from the "Show directories for:" dropdown
  3. Add the path to your MFC include directory (typically `C:\Program Files\Microsoft Visual Studio\VC98\MFC\Include`)

## Notes

- This is a legacy project built for Visual Studio 6.0
- The project uses MFC (Microsoft Foundation Classes) for the UI
- Compatibility with modern Visual Studio versions is not guaranteed

## License

MIT License

Copyright (c) 2024 IBModeler Contributors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

## Required Libraries

### Included Libraries

This project depends on several pre-compiled libraries that are already included in the Release and Debug folders. The source code for these libraries is maintained in separate repositories.
