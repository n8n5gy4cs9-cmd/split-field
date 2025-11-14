# Split-Field - PSP Two-Player Cooperative Puzzle Game

Split-Field is a cooperative puzzle game for Sony PSP where two players share one device to solve puzzles together.

## Game Description

**Split-Field** is a unique two-player cooperative puzzle game designed for the PSP. Both players control characters on the same screen using different controls:

- **Player 1** uses the D-Pad (Up/Down/Left/Right)
- **Player 2** uses the Action buttons as directional controls:
  - Triangle = Up
  - Cross (X) = Down
  - Square = Left
  - Circle = Right

### Gameplay Mechanics

- **Cooperative Play**: Two players must work together to solve each level
- **Push Boxes**: Move boxes onto goal tiles (green)
- **Ghost Boxes**: Special purple boxes that only one player can move
- **Avoid Enemies**: Red tiles are dangerous - don't touch them!
- **Team Strategy**: Some puzzles require coordinated movement from both players

Perfect for playing with a friend on a single PSP device!

## Installation

### ⚠️ Important for macOS Monterey (2015 MacBook) Users

Due to compatibility issues with macOS Monterey, Xcode 13, and the inability to install Docker Desktop, there are **three options**:

### Option 1: GitHub Codespaces (Recommended ✅ FREE!)

The **easiest and most reliable** solution - develop in the cloud:

1. **Set up Git repository:**
   ```bash
   cd /Users/harriahola/Projects/PSP/helloworld
   git init
   git add .
   git commit -m "Initial PSP Hello World"
   ```

2. **Push to GitHub** (create a new repository on github.com first):
   ```bash
   git remote add origin https://github.com/YOUR_USERNAME/psp-helloworld.git
   git push -u origin main
   ```

3. **Open GitHub Codespaces:**
   - Go to your repository on github.com
   - Click **Code** → **Codespaces** → **Create codespace on main**
   - The environment will automatically set up with PSP toolchain!

4. **Build in Codespaces:**
   ```bash
   make
   ```

5. **Download EBOOT.PBP** to your Mac and test!

**Why Codespaces?**
- ✅ Free 60 hours/month
- ✅ Full Ubuntu Linux environment
- ✅ No local compatibility issues
- ✅ PSP toolchain pre-configured (using `.devcontainer/`)

### Option 2: Native Build (Try It)

Attempt to build with system compiler:

```bash
chmod +x install_native_minimal.sh
./install_native_minimal.sh
```

**Warning:** This may fail due to GCC compatibility issues on macOS Monterey.

### Option 3: Virtual Machine

Install VirtualBox and Ubuntu:
1. Install VirtualBox: `brew install --cask virtualbox`
2. Download Ubuntu 22.04 LTS
3. Follow standard PSP toolchain installation in Ubuntu

## Building the Game

### Using GitHub Codespaces (Recommended):

1. The build environment is automatically configured
2. Simply run:
```bash
make
```

### Using Docker:

```bash
docker run --rm -v "$PWD:/build" -w /build pspdev/pspdev:latest make
```

### Native Build:

If you have the PSP toolchain installed locally:

```bash
make
```

All methods will create an `EBOOT.PBP` file in the `build/` directory, which is the executable format for PSP.

## Running on PSP

### On a Real PSP:

1. Copy the `build/EBOOT.PBP` file to your PSP's memory stick:
   - Create a folder structure: `PSP/GAME/SplitField/`
   - Place `EBOOT.PBP` inside the `SplitField` folder

2. On your PSP:
   - Navigate to Game → Memory Stick
   - You should see "Split-Field"
   - Press X to launch it

3. In the game:
   - Press START to begin playing
   - Player 1 uses D-Pad
   - Player 2 uses Triangle/Cross/Square/Circle
   - Press SELECT during gameplay to return to menu

### On PPSSPP Emulator:

1. Download PPSSPP from [ppsspp.org](https://www.ppsspp.org/)
2. Open PPSSPP and navigate to the `build/EBOOT.PBP` file
3. Both players can use the same keyboard/controller or one player can use keyboard while the other uses a gamepad

## Game Controls

### Menu
- **START**: Start the game
- **X (Cross)**: Exit application

### In-Game
- **Player 1 (Red)**:
  - D-Pad Up/Down/Left/Right: Move character
  
- **Player 2 (Blue)**:
  - Triangle: Move Up
  - Cross (X): Move Down
  - Square: Move Left
  - Circle: Move Right

- **SELECT**: Return to main menu

## Gameplay Tips

1. **Work Together**: Both players must cooperate to solve puzzles
2. **Ghost Boxes**: The purple boxes can only be moved by one specific player
3. **Push Strategy**: Regular boxes (brown) can be pushed by either player
4. **Avoid Enemies**: Red tiles are dangerous - coordinate to avoid them
5. **Goal Placement**: Push boxes onto green goal tiles to progress

## Cleaning Build Files

To clean up compiled files:

```bash
make clean
```

## Project Structure

- `main.c` - Main menu and application entry point
- `game.c` - Core game logic and rendering
- `game.h` - Game structures and function declarations
- `Makefile` - Top-level build wrapper
- `Makefile.base` - PSP-specific build configuration
- `assets/` - Game icons and images
- `build/` - Compiled output directory

## Troubleshooting

### "psp-gcc: command not found"

Make sure the environment variables are set. Run:
```bash
source ~/.zshrc
```

Or check that these lines are in your `~/.zshrc`:
```bash
export PSPDEV="$HOME/pspdev"
export PSPSDK="$PSPDEV/psp/sdk"
export PATH="$PATH:$PSPDEV/bin:$PSPSDK/bin"
```

### Build Errors

Make sure all dependencies are installed:
```bash
brew install wget cmake autoconf automake libtool gcc make patch doxygen
```

## Credits

Split-Field - A two-player cooperative puzzle game for PSP
Developed using the PSP Homebrew SDK

## License

This is a homebrew project for educational and entertainment purposes.

## Next Steps

Now that you have a working PSP development environment, you can:
- Modify `main.c` to add more features
- Learn about PSP graphics with the `pspgu` library
- Add 3D graphics using the GU (Graphics Utility) library
- Implement game logic, audio, and more!

## Resources

- [PSP Development Wiki](https://www.ps2dev.org/psp/)
- [PSPSDK Documentation](https://github.com/pspdev/pspsdk)
- [PSP Homebrew Community](https://forums.ps2dev.org/)

Happy coding! 🎮
