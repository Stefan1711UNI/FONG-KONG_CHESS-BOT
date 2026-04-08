# ♟️ FONG KONG Chess Bot

An autonomous robotic chessboard powered by Arduino, designed to combine artificial intelligence with a real physical chess experience.

This project allows a player to play chess alone while an AI opponent moves the pieces automatically — creating a “magical” experience where the board plays against the user.

---

## 🚀 Overview

The Fong Kong Chess Bot is a standalone system that integrates:

- 🤖 Embedded AI running on Arduino  
- ⚙️ Robotic XY gantry system  
- 🧲 Magnetic piece movement  
- 📍 Coordinate-based board (X, Y axes)  
- 🎮 Physical interface (buttons + LCD)  

Unlike digital chess applications, this system preserves the physical experience of chess while introducing intelligent automation.

---

## 🧠 Features

- ♟️ Play a full chess game against an AI  
- 🤖 Automatic piece movement  
- 📍 X/Y coordinate-based board system  
- 🧲 Electromagnet-controlled movement  
- 🧠 Embedded micro-Max chess engine  
- ⚡ Fully offline (no PC or internet required)  

---

## 🛠️ Technologies

### Software
- Arduino (C/C++)
- Embedded chess engine (micro-Max)
- Minimax algorithm with alpha-beta pruning
- Modular architecture (AI, logic, sensors, movement)

### Hardware
- Arduino UNO R4  
- 2x NEMA 17 Stepper Motors  
- XY-Gantry (Cartesian system)  
- 64x Hall Effect Sensors  
- Electromagnet + Neodymium magnets  
- CNC Shield + A4988 drivers  
- I2C communication (LCD + sensors)  

---

## ⚙️ How It Works

### Board Representation
The chessboard is mapped using X and Y coordinates, where each square corresponds to a physical position.

### AI Decision
The Arduino runs a lightweight chess engine using minimax with alpha-beta pruning to evaluate moves.

### Movement System
A hidden XY gantry moves under the board. An electromagnet attaches to pieces and drags them across squares.

### Game Loop
1. The player makes a move  
2. Sensors detect the move  
3. The system validates it  
4. The AI calculates a response  
5. The robot executes the move  

---

## ▶️ Setup & Installation

### Requirements
- Arduino CLI or Arduino IDE  
- Arduino UNO R4

   
### Installation

```bash
arduino-cli core install arduino:zephyr

arduino-cli lib install MsgPack@0.4.2
arduino-cli lib install DebugLog@0.8.4
arduino-cli lib install ArxContainer@0.7.0
arduino-cli lib install "ArxTypeTraits@0.3.1"
Compile & Upload
arduino-cli compile --profile default sketch/
arduino-cli upload --profile default -p /dev/ttyACM0 sketch/

```
## 📁 Project Structure


- sketch.ino # Main loop
- ai.ino # Chess AI (micro-Max)
- chess_logic.ino # Rules & validation
- CoreXYController.ino # Movement control
- sensors.ino # Sensor system
- lcd.ino # Display UI
- types.h # Data structures


---

## 👥 Team

- Stefan Sonderling — Hardware & Team Lead  
- Jan Adamski — Software & Integration  
- Kamyab Rayganshirazinejad — AI Logic  
- Diogo Alves — Electronics & Integration  

---

## 💡 Challenges

- Integration between hardware and software proved to be complex  
- The sensor system required redesign (2 → 8 I2C expanders)  
- Hardware instability required multiple adjustments  
- Ensuring reliable full-system performance was difficult  

---

## 🔮 Future Improvements

- Improve movement accuracy  
- Replace wiring with a custom PCB  
- Reintroduce audio system  
- Enhance AI strength  

---

## 💭 Final Thoughts

This project combines hardware, embedded systems, and AI into one complete system.

Although the system is not perfect, it successfully plays a full game of chess autonomously on a physical board, demonstrating the successful integration of multiple complex subsystems into a single working product.
