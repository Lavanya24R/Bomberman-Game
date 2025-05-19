# Bomberman-Style Game (Cocos2d-x)

A Bomberman-style game built using [Cocos2d-x](https://www.cocos.com/en/).

## 🚀 Features

- Player movement and bomb placement
- Destructible walls
- Enemies and scoring system
- Cross-platform project setup (Windows, Linux, Android, etc.)

## 🛠 Requirements

- [Cocos2d-x 4.0](https://github.com/cocos2d/cocos2d-x)
- CMake
- A C++ compiler (Visual Studio 2012, GCC, Clang, etc.)
- SDL2 (Linux only)
- Android NDK & SDK (for Android builds)
- Python 2.7.14

## 🧰 Setup Instructions

Follow these steps to set up and run the game on your machine:

### 1. Clone the Repository

```bash
git clone https://github.com/Lavanya24R/Bomberman-Game.git
cd bomberman-game

### 2. Initialize submodules(if required)

If the Cocos2d-x engine is included as a submodule:
git submodule update --init --recursive

### 3. Create a build directory

mkdir build
cd build

### 4. Generate build files using CMake

```Windows(Visual Studio)
cmake .. -G "Visual Studio 17 2022" -A x64

```macOS (Xcode):
cmake .. -G Xcode

```Linux
cmake ..
[Linux users may need to install SDL2 first:
sudo apt install libsdl2-dev]

### 5. Build the Project

```Windows
Open the generated .sln file in Visual Studio and build the solution.

```macOS/Linux:
make

### 6. Run the Game
After building, the executable will be located in the bin/ or build/ folder. Run it directly to play the game.

📱 Optional: Building for Android

To build for Android:

Ensure Android NDK and SDK are installed and properly set up.

Open the project in Android Studio or build via command line using CMake + NDK.

📌 Notes
This is a beginner project built using basic Cocos2d-x features.

Contributions, bug reports, or suggestions are welcome!
