# 🎲 Monopoly Game – C++ with Terminal UI

A fully playable **Monopoly** board game implemented in **C++ programming**, running in the terminal window with ASCII-style rendering. This project includes game logic for basic player turns, dice rolls, property management, card effects, fate/chance events, and an extra minigame system.

---

## 📘 Course Information

- **Course Code:** `CS1010302`  
- **Course Title:** `物件導向程式設計實習 Lab for Object Oriented Programming`  
- **Semester:** `Spring / Fall 113`

---

## 👥 Team Members

- B11315014 劉施伲
- B11315015 張銘元
- B11315032 曾湘晴

---

## 🎮 Features

- 🗺️ Fully functional 8x8 Monopoly board layout with various event tiles
- 💰 Property buying, upgrading, and toll-paying
- 🎴 Card system: Dice, Destroy, Rocket, Fate, etc.
- 🎲 Fate and Chance tiles with config-based randomized effects and minigames
- 🏥 Hospital logic, players can skip turns or pay to exit the tile
- 🎮 Built-in mini-games (Horse Race, Maze Escape, Treasure Chest etc.)
- 💾 Save/load system for both board state and players
- 🧩 Configurable board via config.json configurations

---

## 🕹️ Game Flow Overview

1. Players are initialized with starting money and cards.
2. Each player rolls dice and move around a square 8×8 board (28 tiles perimeter (numbered 0-27)).
3. The landed tile triggers:
   
| Tile Type   | Symbol/Icon | Description                                                 |
|-------------|-------------|-------------------------------------------------------------|
| `start`     | 🏁          | Grants a bonus when passed                                  |
| `property`  | 🏠          | Buy, upgrade, pay toll, or sell                             |
| `store`     | 🏬          | Use or purchase cards like Dice, Rocket, Destroy, etc.      |
| `fate`      | 🎴          | 25% minigame, 70% money event, 5% nothing                   |
| `chance`    | 🃏          | 35% minigame, 55% money event, 10% nothing                  |
| `hospital`  | 🏥          | Skip up to 3 turns unless you roll ≥ 10 or pay to leave     |

5. 🚀 **Special Cards**: like Rocket, Destroy, Dice Control cards, allow strategic gameplay and interactions
6. Players use cards strategically or trigger minigames.
7. The game ends when:
  - A player reaches a specified amount of money (`winMoney`)
  - All other players are bankrupt

---

## 🛠️ Setup Instructions

### 🔧 Requirements

- **C++17 or higher**
- **Visual Studio 2019+ / g++ / clang++**
- Terminal (CMD or compatible console that supports ANSI escape codes for color)

---

### 📁 File Structure Overview
```
monopoly/
├── Game/              # Core logic (Game, Config, Map, Player)
├── include/           # 3rd party headers (nlohmann/json in this project)
├── json/              # Configuration, Commands, and Dialogue data
├── WelcomingScreen/   # ASCII-style Welcoming Screen renders
├── Card.cpp           # Cards and their effects
├── Command.cpp        # Cheat commands
├── Event.cpp          # Tile effects
├── Minigame.cpp       # Various minigames
├── Store.cpp          # Item Shop
├── Tile.cpp           # Initialisation of board game tiles
├── Utils.cpp          # Miscellaneous commands and utilities
├── main.cpp           # Entry point
├── testing.json       # Saved game data
```

---

## 🚀 How to Compile and Run

### On Windows (Visual Studio):

1. Open the `.sln` file.
2. Build in Debug/Release mode.
3. Press `F5` or `Local Windows Debugger` to run the game.

* Or else, just run the `.exe` file to start the game.

### On Linux/macOS:

```bash
g++ -std=c++17 -o monopoly main.cpp -I./include -lsomeDependencies
./monopoly
```

> Make sure to set up relative paths for `json/dialogue.json` and `config.json` to be available during runtime.

---

## 🧪 Configuration Overview

### `config.json`
- Sets player icons, colors, tile width, and map size
- Defines tile types: `start`, `property`, `fate`, `chance`, `store`, `hospital`
- Controls event value ranges:
```json
"eventValueRange": {
  "FATE": [500, 1000],
  "CHANCE": [2000, 5000]
}
```
- Includes modes: `DEBUG`, `DUEL`, `CUSTOM`, `RELEASE` with player setup and win conditions
- Lists 28 board tiles and 5 types of cards with price and effects

### `dialogue.json`
- Handles input prompts and in-game action menus
- Context-sensitive responses for locations: `start`, `store`, `hospital`, `jail`, `property_owned`, etc.
- Defines default player interaction flow with options like: `Roll`, `Buy`, `Sell`, `Pass`, `View Info`

### `command.json`
- Implements slash commands for game control
```plaintext
/move [location], /give [player] [amount], /card [name], /info, /minigame, /gamestate [state]
```
- Each command includes usage, example, and response prompt
- Supports admin actions like `/refresh`, `/list`, and error handling

---

## 🧩 Possible Future Improvements

- 💡 AI/Computer-played opponents
- 📡 LAN Multiplayer
- 🎨 Graphical User Interface (GUI)
- 🔄 Undo/Redo system

---

## ✨ Acknowledgements

- [nlohmann/json](https://github.com/nlohmann/json) – for clean JSON parsing
- Project inspired by Monopoly by Hasbro™
