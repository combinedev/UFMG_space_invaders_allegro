# 👾 Space Invaders C Allegro rebuild
- Arcade game remake with C language and Allegro Vivace lib. 
- Programming and Developing [PDS] 1 subject assignment, from my Computing Science Bachelor's course.

# Building Space Invaders with Allegro

This project is built using **CMake** and uses the **Allegro 5** game programming library. Below is a complete step-by-step guide for building the project from source.

## 🔧 Dependencies

* **CMake** ≥ 3.10
* **GCC** or **Clang** (for compiling)
* **Allegro 5** and required add-ons:

  * `allegro`
  * `allegro_audio`
  * `allegro_acodec`
  * `allegro_font`
  * `allegro_ttf`
  * `allegro_image`
  * `allegro_primitives`

## 🐧 On Linux (Debian/Ubuntu)

Install all dependencies with:

```bash
sudo apt update
sudo apt install cmake g++ \
    liballegro5-dev liballegro-image5-dev \
    liballegro-audio5-dev liballegro-acodec5-dev \
    liballegro-font5-dev liballegro-ttf5-dev \
    liballegro-primitives5-dev
```

## 🪟 On Windows

You can either:

* Use **MSYS2**, **vcpkg**, or **conan** to install Allegro 5.
* Or, build Allegro 5 from source.
* Then configure your compiler (e.g., MSVC or MinGW) and make sure it's accessible in your `PATH`.
----
### How to Build Allegro5 Projects on Windows Using MSYS2 and MinGW

**1. Install MSYS2 and MinGW-w64**

* Download and install MSYS2 from https://www.msys2.org.

* Open the MSYS2 MinGW 64-bit shell from the Start Menu.

* Update MSYS2 packages (run these commands; restart MSYS2 if prompted):

```msys2
pacman -Syu
pacman -Su
```

* Install the MinGW-w64 toolchain and Allegro5 libraries:

```msys2
pacman -S mingw-w64-x86_64-toolchain
pacman -S mingw-w64-x86_64-allegro
```

**2. Set System Environment Variables**

* To build from PowerShell or any terminal, add these environment variables:

    * Add `C:\msys64\mingw64\bin` to your `PATH` variable.
    * Add a new variable `PKG_CONFIG_PATH` with value: `C:\msys64\mingw64\lib\pkgconfig`
    *  Add a new variable `CMAKE_PREFIX_PATH` with value: `C:\msys64\mingw64`

After adding, restart your terminal sessions to apply changes.

**3. Build Your Project Using CMake**

Open PowerShell or MSYS2 MinGW 64-bit shell and navigate to your project folder:

```powershell
cd "C:\Users\yourusername\path\to\your\project"
```

Create and enter the build directory:

```powershell
mkdir build
cd build
```

Run CMake configuration:

```powershell
cmake .. -G "MinGW Makefiles"
```

Build the project:

```powershell
mingw32-make
```

**4. Copy Assets and Allegro DLLs to Build Directory**

* From your project root, copy the assets folder to the build directory:

```powershell
Copy-Item -Recurse ..\assets .\assets
```

* Copy all Allegro DLLs from MSYS2 mingw64 bin folder to your build folder:

```powershell
Copy-Item C:\msys64\mingw64\bin\allegro_*.dll . -Force
```
This copies all DLL files starting with allegro_ which your program needs to run.

**5. Run Your Executable**

Now inside the build folder, run your program:
```powershell
.\space_invaders.exe
```
Your program should run and load assets correctly.

---

## Dev logs
### >> [08/06/2025]
Studying the game's logic for basic state setup. Notes:
- Player controlled ship only moves horizontally left/right. Continuous movement until the A or D key is no longer down. (smooth moveset). Player's height is fixed on the screen bottom.
- Three types of enemies: A = 100 pts, B = 50 pts and C = 25 pts. Enemies are **30px** spaced between.
- Initial enemy matrix: 4x5, first line is only A type, second and third is only B, forth and fifth is only C.
- Monsters move all together. ->, then, when they touch the border, they switch directions, <- and lower down the entire matrix height every time nearer the ground (jump).
- A monster only shoots down if its below column is totally free or if its column is the last one.
- Player shooting has a cooldown: the last projectile must extinguish first before shooting another.
- Projectiles can extinguish by colliding with: player (death), monster (explodes and dies, leaving a spot in the matrix), another projectile, screen boundaries.

- Game end conditionals:
   * Killing all the aliens in the matrix
   * One of the aliens touches the ground
   * One of the aliens touches the player
   * Player run out of lives [3 in total]
- High score functionality:
   * Basically is a int score variable, which is visible by a text on the top and increases during the game.
   * In the end, a screen should show the player score and the highest score (initially 0, based on the file "highscore.txt").
   * If player score > highest score, highest score is updated: the number is written in the file called "highscore.txt" (overwrite)
 
 The assignment implies the deployment of a state machine:
- Menu state
- Game state
- Score state

To consider extras:
- [Optional] Occasional ufo passing above the first line of the matrix. It is deleted when is completely off the screen. Moves ->. Player projectile can explode it. Ufo = 200pts.
- Player shields, they collide with alien projectiles and are eventually completely destroyed (own hp var).
- Physics, like arrest and torque within the ship movement.

### >> [16/06/2025]
- Game is finished up.
  
- Extra mechanics added:
   * Shields
   * Player lives UI
   * State-machine (menu, game, gameover)
   * Cross-enemy projectiles
   * UFO passing probability (can you shoot'em down?)
   * Highscore.txt file register.
     
- **Controls:**
   * *[ A ]* moves the ship to the left
   * *[ D ]* moves the ship to the right
   * *[ SPACEBAR ]* shoots a projectile.
     
- **Menu navigation:**
   * *[ S ]* selects down
   * *[ W ]* selects up
   * *[ ENTER ]* loads your selection.
<img height=350em width=500em src="gameplay.png">
