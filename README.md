# TornadoV.cpp (TornadoV++)

A high-performance, native C++ rewrite of the TornadoV mod for Grand Theft Auto V. Marketed as **TornadoV++**, this project is a complete overhaul of the original C# implementation, designed specifically for the ScriptHookV SDK to provide superior stability, physics, and visual fidelity.

## 🌪️ Key Features

- **Native Performance**: Built from the ground up in C++ to minimize frame time impact and maximize execution efficiency.
- **Enhanced Physics Engine**: Completely rewritten force application logic (`ENTITY::APPLY_FORCE_TO_ENTITY`) for more realistic entity pulling, rotation, and lofting.
- **Optimized PTFX System**: Improved Particle FX management with handle validation and smart cleanup, resolving the "invisible funnel" issues common in script-based versions.
- **Intelligent Spawning**: Dynamic spawning system that reacts to weather conditions (storms) with configurable delays and distance checks.
- **Seamless UI**: Built-in menu system for real-time configuration of vortex strength, movement, and visual styles.
- **F6 Toggle**: Quick spawn/despawn functionality for immediate action.

## 🛠️ Technical Improvements (vs. C# Version)

- **Frame-Sync Logic**: Uses `GAMEPLAY::GET_FRAME_TIME()` and `GAMEPLAY::GET_GAME_TIMER()` to ensure consistent simulation regardless of frame rate.
- **Memory Management**: Utilizes modern C++ smart pointers and RAII principles to prevent memory leaks and script crashes.
- **Optimized Math**: Custom `MathEx` class for high-performance Vector3 and Quaternion operations, ensuring 1:1 parity with the original's movement logic while running at native speeds.
- **SDK Compliance**: 100% compliant with ScriptHookV SDK 1.0.617.1a+, using direct native calls for maximum compatibility.

## 🚀 Installation

1. Ensure you have [ScriptHookV](http://www.dev-c.com/gtav/scripthookv/) installed.
2. Place `TornadoV.asi` and `TornadoV.ini` into your GTA V main directory.
3. (Optional) Customize settings in the `TornadoV.ini` file.

## 🎮 Controls

- **F6**: Toggle Tornado (Spawn/Despawn)
- **Menu Key (Default: F5)**: Open the TornadoV++ configuration menu.

## 🏗️ Building from Source

- **Requirements**: Visual Studio 2022 (with C++ Desktop Development workload).
- **SDK**: ScriptHookV SDK.
- **Configuration**: Target `Release | x64` for the optimized ASI build.

## ⚖️ Credits

- **Original Logic**: TornadoVEnhanced (C#)
- **C++ Rewrite & Optimization**: TornadoV.cpp / TornadoV++
- **Dependencies**: Alexander Blade (ScriptHookV SDK)

---
*Note: This project is intended for Single Player use only.*
