# 👾 Space Invaders C Allegro rebuild
- Arcade game remake with C language and Allegro Vivace lib. 
- Programming and Developing [PDS] 1 subject assignment, from my Computing Science Bachelor's course.

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
