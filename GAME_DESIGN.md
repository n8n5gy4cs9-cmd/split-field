# Split-Field - Game Design Document

## Concept

**Split-Field** is a unique two-player cooperative puzzle game designed specifically for the Sony PSP, where two players share a single device using different control schemes.

## Core Mechanics

### Dual Control System
The game's signature feature is its innovative split-control system:

- **Player 1 (Red)**: Uses the PSP D-Pad
  - Up/Down/Left/Right for movement
  - Traditional directional controls
  
- **Player 2 (Blue)**: Uses Action Buttons as directional input
  - △ (Triangle) = Up
  - ✕ (Cross) = Down
  - □ (Square) = Left
  - ○ (Circle) = Right

This creates a unique cooperative experience where both players must coordinate their movements while sharing one device.

## Gameplay Elements

### Tiles & Objects

1. **Empty Tiles** (Black)
   - Basic walkable space
   - Players can move freely

2. **Wall Tiles** (Gray)
   - Solid barriers
   - Block movement and box pushing

3. **Boxes** (Brown)
   - Can be pushed by either player
   - Must be moved onto goal tiles
   - Cannot be pulled, only pushed

4. **Ghost Boxes** (Purple)
   - Special mechanic: Only one player can interact
   - Determined by position (odd/even tiles)
   - Forces strategic cooperation
   - Appears on other player's screen but cannot be moved by them

5. **Goal Tiles** (Green)
   - Target locations for boxes
   - Level complete when all boxes reach goals

6. **Enemy Tiles** (Red)
   - Hazards to avoid
   - Touching causes game over
   - Forces careful route planning

### Player Characters

- **Player 1**: Red square character
- **Player 2**: Blue square character
- Characters cannot overlap (collision detection)
- Must cooperate to navigate around each other

## Level Design Philosophy

### Cooperative Puzzle Structure

Levels are designed to require both players:
- Simultaneous actions (pushing from both sides)
- Sequential actions (one player moves first)
- Ghost box mechanics (only one can interact)
- Enemy avoidance (coordinated movement)

### Difficulty Progression

1. **Tutorial Phase**: Simple box pushing
2. **Cooperation Phase**: Both players needed
3. **Ghost Box Phase**: Asymmetric abilities
4. **Challenge Phase**: Enemies + complex puzzles

## Technical Implementation

### Rendering
- Direct VRAM access for fast drawing
- 16x16 pixel tile system
- 20x14 tile playfield
- Centered on 480x272 PSP screen
- Support for both RGB565 and RGBA8888 formats

### Game Loop
```
Input Reading → Movement Logic → Collision Detection → Push Mechanics → Win/Lose Check → Rendering
```

### State Management
- Menu state (title screen)
- Game state (playing)
- Win state (level complete)
- Lose state (touched enemy)
- Quit state (return to menu)

## User Experience

### Menu System
- Clean ASCII art title
- Simple instructions
- Press START to play
- Press X to exit

### In-Game UI
- Level number display
- Control hints at bottom
- Clear visual feedback
- Instant response to input

### Movement Feel
- Grid-based movement
- Input delay between moves (prevents sliding)
- Button press detection (not hold)
- Responsive and precise

## Future Expansion Ideas

### Additional Mechanics
- Ice tiles (slippery movement)
- Switch tiles (door toggles)
- Teleporters (strategic repositioning)
- Timed challenges
- Multiple box types

### Level Features
- Level editor mode
- Save/load progress
- High scores
- Time trials
- Multiple worlds

### Polish
- Sound effects
- Background music
- Particle effects
- Screen transitions
- Better sprite graphics

## Development Notes

### Architecture
- Clean separation: main.c (menu) + game.c (gameplay)
- game.h defines all data structures
- Modular and maintainable code

### PSP Compatibility
- Works on PSP-1000/2000/3000/Go
- Compatible with PPSSPP emulator
- Tested on Homebrew firmware

### Build System
- Makefile-based build
- Docker container support
- GitHub Codespaces ready
- Cross-platform development

## Credits & Inspiration

Split-Field combines elements from:
- **Sokoban**: Box-pushing puzzles
- **Fireboy & Watergirl**: Dual-character cooperation
- **Portal 2**: Cooperative puzzle solving

Designed as a demonstration of:
- PSP Homebrew capabilities
- Innovative control schemes
- Local multiplayer design
- Tile-based game development

---

**Split-Field** - Making cooperation fun, one puzzle at a time! 🎮
