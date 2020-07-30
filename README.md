# Final Project

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](docs/README.md)

**Author**: Omar Babu - [`omar.m.babu@gmail.com`](mailto:omar.m.babu@gmail.com)

##Overview
- 2-D platformer game
- The main enemy is a slime, but additional snowballs fall from the sky
- Player can attack the slime or the snowball, but colliding with the enemies without attacking will result in losing hp.
- After losing all hp, the player will be invulnerable for another 10 seconds.
- The player has a "super move" which kills all enemies except the slime. This can be used up to twice per game.
- Player gets points for staying alive and killing enemies.

##Timeline
- Week 1: Set up cinder and external library, begin animating some objects to ensure cinder works properly.
- Week 2: Get familiar with the external library (i.1. making bodies, animating them, etc.)
- Week 3: Complete game logic and add additional features if possible. (e.x. smoothen player movement, change player icon, etc.)

##Libraries Used
- Cinder (for game engine and gl display)
- Box2D (for physics of player and enemies, and for handling collisions)