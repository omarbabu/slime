Development
---
4/16 
- Copied into cinder directory, trying to get everything compiling
- Choreograph cinderblock is difficult to set up, documentation online is all wack


4/18 
- More research on choreograph cinderblock, understand usage of timelines, phrases, and sequences.
- Will probably use box2d for player and environment movement/interaction

4/20
- Thinking of switching focus to setting up functions for cinder rather than setting up choreograph.

4/21
- Work on basic drawing with cinder. 
- Added DrawPlayer and DrawBackground functions, which currently just draw a white square and purple background.

4/23
- Using box2d for physics logic, basic shapes being drawn
- Using cinder for game engine instead of sfml, to save myself from unnecessary complications. 
- Issue with including box2d, did everything found on piazza but the issue still exists


4/24
- Finally resolved the include error, could have been done in 5 seconds but took an atrocious amount of time to find the problem.
- Currently working on displaying bodies and textures using cinder.

4/26
- Box2D objects not printing with cinder, need to find a way to display these bodies, but I cannot find any functions within the Box2D library to have any functions to draw on the screen.

4/27
- Found way to get position of Box2D object, I think I can use this to draw with cinder.
- Yes, I can. Objects drawing with cinder, but sometimes falling through the ground. Should fix this issue before moving on and adding more platforms.

4/29
- Trying to add image to enemy; however, program keeps crashing.
- Giving up on the image for now, working on moving the player using box2D logic rather than with simple cinder cordinates.

4/30
- Player x and y velocity now change with keyboard inputs, need to make the jumping motion look and feel more natural. The physics is awkward for the player to deal with in a platformer game, researching ways to fix this.
- Added external library (minibpm) that will analyze tempo of backround music, will use the tempo to spawn enemies at a dynamic rate.

5/2
- Immplemented jumping, changed background to black with stars, will change structure of project to finish on time.
- Need to give player and enemy images instead of just squares.

5/3 
- Thinking of implementing a spritesheet for the player, need to think of how this can be done
- Added image to enemy

5/5
- Added player sprite idle animation.
- Added sprite running animation, both left and right.
- Added sprite jumping animation.
- Slightly changed the game - see ReadMe.md.
- Implemented point system and display of point system.
- Handled collisions between player and enemies.
- Manually edited spritesheets to use them properly with c++
