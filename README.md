# Chip-8 Viz

## What this project does

I built Chip-8 Viz as an interactive educational Chip-8 emulator in C++ with SDL2.
It emulates the Chip-8 virtual machine and shows:

- 64x32 monochrome display output
- keypad input
- delay/sound timers
- a live right-side dashboard for CPU state (PC, I, SP, opcode, DT/ST, registers)

My goal is both correctness and visibility: I can run games and also see internal VM state while they run.

## Tech stack

- C++17
- CMake
- SDL2 (video, events, audio)

## Build

From project root:

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

## Run

### Built-in demo ROM

```bash
./build/chip8
```

### Run any game ROM

```bash
./build/chip8 path/to/rom.ch8
```

From project root, I can run:

```bash
./build/chip8 c8games/tictac
```

In my project, ROMs are under `games/c8games/`, so I use:

```bash
./build/chip8 games/c8games/TICTAC
```

For quick usage help:

```bash
./build/chip8 --help
```

## Controls

### Chip-8 keypad mapping

| Chip-8 key | Keyboard  |
| ---------- | --------- |
| `1 2 3 C`  | `1 2 3 4` |
| `4 5 6 D`  | `Q W E R` |
| `7 8 9 E`  | `A S D F` |
| `A 0 B F`  | `Z X C V` |

### Emulator controls

| Key   | Action                                           |
| ----- | ------------------------------------------------ |
| `Esc` | Quit                                             |
| `P`   | Pause / resume                                   |
| `N`   | Single-step one opcode while paused              |
| `T`   | Toggle instruction trace logs                    |
| `[`   | Toggle memory quirk (`FX55/FX65` increments `I`) |
| `]`   | Toggle shift quirk (`8XY6/8XYE` use `Vy`)        |

## Dashboard guide (right panel)

Top section:

- `PC`: Program counter
- `I`: Index register
- `SP`: Stack pointer
- `OP`: Last fetched opcode
- `DT` / `ST`: Delay and sound timers

Middle section:

- Registers `V0` through `VF` in two columns.

Bottom indicators:

- `MEM+` box: ON when memory quirk is enabled (`FX55/FX65` updates `I`)
- `SHVY` box: ON when shift quirk is enabled (`SHR/SHL` use `Vy`)

Window title intentionally stays short (status only, e.g. paused/wait key/trace); details are in the panel.

## What I check when testing games

1. Game draws and updates on the left display.
2. Input works (title/menu responds to mapped keys).
3. Dashboard values change while running (`PC`, `OP`, registers, `DT/ST`).
4. Pause/step works for debugging (`P`, then `N`).
5. If behavior is off, try quirk toggles (`[` and `]`) and test again.

## Audio behavior

- Sound plays when the ROM sets `ST > 0`.
- If no ROM sound opcodes are used, you may hear nothing.

## ROM credit / source

I used test ROMs from:

- Zophar's Domain, **Chip-8 Games Pack**: [https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html](https://www.zophar.net/pdroms/chip8/chip-8-games-pack.html)

These are listed there as public-domain Chip-8 games for interpreters.

## Troubleshooting

- **Black screen / unexpected behavior:** confirm ROM path, then try quirk toggles (`[` and `]`).
- **No sound:** ensure system audio is enabled and ROM actually uses `ST`.
- **Too much console output:** press `T` to turn trace off.
