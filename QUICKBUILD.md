# Quick Start Guide - Split-Field

## Build in 3 Steps

### Step 1: Build the Game
```bash
make clean
make
```

This will:
- Compile `main.c` and `game.c`
- Link with PSP libraries
- Create `build/EBOOT.PBP`

### Step 2: Get the File
The compiled game is in: `build/EBOOT.PBP`

### Step 3: Install on PSP
1. Connect PSP to computer
2. Navigate to: `PSP/GAME/`
3. Create folder: `SplitField/`
4. Copy `EBOOT.PBP` into `SplitField/`
5. Disconnect PSP
6. Launch from Game → Memory Stick

## Testing with PPSSPP

### Download PPSSPP
- Website: https://www.ppsspp.org/
- Available for: Windows, Mac, Linux, Android, iOS

### Run the Game
1. Open PPSSPP
2. File → Load
3. Navigate to `build/EBOOT.PBP`
4. Click to launch

### Configure Controls (Optional)
- Settings → Controls
- Map keyboard keys for both players
- Or use gamepad for one player

## Quick Gameplay Test

### Menu Test
- ✓ See fancy "SPLIT FIELD" title
- ✓ Press START → game loads
- ✓ Press X → exits to menu

### Movement Test
**Player 1:**
- D-Pad Up → character moves up
- D-Pad Down → character moves down
- D-Pad Left → character moves left
- D-Pad Right → character moves right

**Player 2:**
- Triangle → character moves up
- Cross → character moves down
- Square → character moves left
- Circle → character moves right

### Box Test
- Move player next to brown box
- Push into box → box moves
- Push box into green goal → success!

### Ghost Box Test
- Try to push purple box with Player 1
- Try to push same box with Player 2
- Only one should work (based on position)

### Enemy Test
- Move player onto red tile
- Game over screen should appear

## Common Build Issues

### Issue: "psp-gcc: command not found"
**Solution:** Make sure you're in the dev container or have PSP toolchain installed

### Issue: "No such file or directory: game.h"
**Solution:** Make sure all files are in the project root:
- main.c
- game.c
- game.h

### Issue: "pack-pbp: ERROR"
**Solution:** Check that assets exist:
- assets/icon0.png
- assets/pic1.png

### Issue: Build succeeds but file is missing
**Solution:** Check the `build/` directory:
```bash
ls -la build/
```

## File Sizes (Approximate)

- Source files: ~15 KB total
- Object files: ~30 KB
- EBOOT.PBP: ~200-300 KB (with assets)

## Performance Notes

### On Real PSP
- 60 FPS rendering
- Instant input response
- No lag or stuttering
- Works on all PSP models

### On PPSSPP
- Depends on host system
- Should run at full speed
- May have input latency on slow systems
- Use "Fast Forward" if needed for testing

## Development Workflow

### Make Changes
1. Edit source files (main.c, game.c, game.h)
2. Save changes
3. Run `make`
4. Test in PPSSPP
5. Repeat!

### Clean Build
If something seems wrong:
```bash
make clean
make
```

### Debug Output
The game doesn't have debug output by default, but you can add:
```c
printf("Debug: Player 1 at (%d, %d)\n", ctx->player1.x, ctx->player1.y);
```

## Next Steps

### Modify the Game
- Edit `game.c` to change mechanics
- Adjust `FIELD_WIDTH` and `FIELD_HEIGHT` for different sizes
- Add new tile types in `TileType` enum
- Create new level layouts in `game_init()`

### Add Features
- New levels (create level array)
- Score system (track moves/time)
- Sound effects (use PSP audio libs)
- Better graphics (sprite system)

### Share Your Creation
- Test thoroughly on real PSP
- Package with icon and background
- Share on PSP homebrew communities
- Upload to GitHub

## Resources

### PSP Homebrew
- PSPDev Forums: https://forums.ps2dev.org/
- PSP Homebrew Discord communities
- GitHub PSP projects

### Learning More
- PSPSDK Documentation
- PSP Programming tutorials
- Open source PSP games

## Troubleshooting Checklist

Before asking for help:
- [ ] Ran `make clean && make`
- [ ] Checked all source files exist
- [ ] Verified PSP toolchain is installed
- [ ] Looked for error messages in build output
- [ ] Tested EBOOT.PBP exists in build/
- [ ] Tried running in PPSSPP first

---

**Happy Gaming!** 🎮 Enjoy Split-Field!
