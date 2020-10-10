# Slime

[![license](https://img.shields.io/badge/license-MIT-green)](LICENSE)
[![docs](https://img.shields.io/badge/docs-yes-brightgreen)](docs/README.md)

**Author**: Omar Babu - [`omar.m.babu@gmail.com`](mailto:omar.m.babu@gmail.com)

## Overview
- 2-D platformer game
- The main enemy is a slime, but additional snowballs fall from the sky
- Player can attack the slime or the snowball, but colliding with the enemies without attacking will result in losing hp.
- After losing all hp, the player will be invulnerable for another 10 seconds.
- The player has a "super move" which kills all enemies except the slime. This can be used up to twice per game.
- Player gets points for staying alive and killing enemies.

## Libraries Used
- Cinder (game engine and gl display)
- Box2D (physics of player and enemies and handling collisions)
