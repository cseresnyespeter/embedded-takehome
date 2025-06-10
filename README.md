## Prerequisites for Windows

To build and run this project, you will need the following tools installed on your system:

1.  **CMake**
2.  **MinGW-w64** (GCC, G++, and make)

## Limitations

1.  **The elevator is always assumed to be on a floor.**
2.  **The door position is always assumed to be fully opened or closed.**

## How to Build on Windows

The build process is managed by CMake.

1.  **Open a Command Prompt (cmd)**.
2.  **Navigate to the project's root directory** (where the `CMakeLists.txt` file is located).
3.  **Create a build directory.**
    ```
    mkdir build
    cd build
    ```
4.  **Run CMake to generate the build files**. You must specify the "MinGW Makefiles" generator.
    ```
    cmake -G "MinGW Makefiles" ..
    ```
    This step will also automatically download and configure the Google Test framework for the unit tests.

5.  **Compile the project** using `make`.
    ```
    make
    ```

After this process is complete, you will find two executables inside the `build` directory:
* `elevator_emulator.exe` - the elevator simulator
* `run_tests.exe` - the unit tests

## How to Run on Windows
1.  **Open a Command Prompt (cmd) in the build directory**.
2.  **To run the emulator, execute `elevator_emulator.exe`**.
2.  **To run the unit tests, execute `run_tests.exe`**.
