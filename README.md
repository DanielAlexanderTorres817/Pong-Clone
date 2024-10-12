# Pong Clone

### Author: Daniel Torres

### Assignment Due Date: 2024-10-12, 11:59 PM

## Overview

This project is a clone of the classic Pong game, created as part of an assignment. The game is developed using C++, SDL2, OpenGL, and GLSL shaders, and it features textured sprites, player-vs-player or player-vs-AI modes, and multiple fireballs for added complexity.

## Features

- **Player vs. Player Mode:** Players can control their paddles using `W` and `S` keys for Player 1 and arrow keys for Player 2.
- **Player vs. AI Mode:** Switch to AI control for Player 2 by pressing the `T` key.
- **Multiple Fireballs:** Up to 3 fireballs can be activated, controlled by pressing keys `1`, `2`, or `3` during gameplay.
- **Pause and Start:** Game starts when the `Space` key is pressed and pauses when it is not.
- **Score Cap:** The first player to reach a score of 5 wins the game.
- **Textured Sprites and Animation:** Uses custom textures for paddles, fireballs, and the background, with rotation and collision detection for fireballs.

## Controls

| Key            | Function                                    |
|----------------|---------------------------------------------|
| `W`, `S`       | Move Player 1’s paddle up and down          |
| `Up`, `Down`   | Move Player 2’s paddle up and down          |
| `Space`        | Start the game                              |
| `T`            | Toggle between AI and Player 2 control      |
| `1`, `2`, `3`  | Activate 1, 2, or 3 fireballs, respectively |
| `Q`            | Quit the game                               |

## Game Logic

- **Fireballs:** Each fireball has its own speed and can collide with the paddles or the top/bottom of the screen, reflecting direction upon collision.
- **Collision Detection:** Fireballs and paddles use axis-aligned bounding box (AABB) collision detection to ensure accurate interactions.
- **AI Movement:** If AI mode is active, Player 2’s paddle moves automatically up and down at a fixed speed.

## How to Run

1. **Compile the Game:**
   Ensure you have installed SDL2, OpenGL, and GLSL libraries.
   Compile the project using a C++ compiler. If using a Makefile, ensure you link the necessary libraries.

2. **Run the Game:**
   After compiling, run the executable. The game window will open with the paddle controls and gameplay rules as outlined above.

3. **Texture Files:**
   Ensure the following texture files are in the same directory as the executable:
   - `Magic_sphere.png`
   - `Attack_2.png`
   - `fireball_transparent.png`
   - `pixelArena.png`
   - `paddle_1.png`
   - `font1.png`

## Technical Details

- **Libraries Used:** 
  - SDL2 for window management and input handling.
  - OpenGL for rendering and transformations.
  - GLSL for custom shaders.
  - STB Image for texture loading.

- **Collision Handling:**
  Paddle and fireball collisions are calculated using box-to-box collision detection. The fireball changes direction when it hits a paddle or the top/bottom boundary.

- **Textures and Sprites:**
  The game uses a custom shader to handle textured sprites. Each fireball and paddle is rendered using texture atlases.

## Notes to the Grader

- After a player wins, there is an implemented delay of 5 seconds on line 797, which can be changed for testing purposes
- The game is currently set to end when either player scores 5 points, this value can be changed by altering the value of the variable `CAP` on line 111

## Acknowledgments

This project was completed without collaborating with anyone else, in conformance with the NYU School of Engineering Policies and Procedures on Academic Misconduct.
