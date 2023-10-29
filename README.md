# Based on CraftyBoss/SuperMarioOdysseyOnline **v1.4.0**
Huge thanks to CraftyBoss, Sanae, and all the other folks who made SMO online! This mod uses that code as a base and replaces capture sync with prop sync and adds a bit of additional logic on the client for props. 

The SMOO server is unchanged so running any SMOO server (that supports client v1.4.0) should work, though you should ensure all users are using Prop Hunt and none are using a standard hide and seek build, otherwise it could cause issues. The commit `dd0de0d build binary files via docker` from SmoOnlineServer has been tested to work with this build (tho any build should work as long as it supports client v1.4.0 that this is based on)

### Controls (same as SMO online with these additions)
* Randomize Prop (switch between hider and seeker)
* Rotate thru Props (ZR + DpadRight)
* Freeze in mid-air (R + DpadRight), useful for getting the exact positioning for a prop to blend in

### :warning: Known Issues
* Switching kingdoms while a prop is active crashes the game. For now, please turn off props when switching kingdoms (either go into seeker mode or turn off the Hide&Seek game mode) 
* Sometimes props take a sec to appear, try moving around or switching between hider and seeker

### Kingdoms Supported
* Cap
* Cascade
* Sand
* Wooded (limited prop set due to crashes, probably memory limit related)
* Lake
* Lost
* Metro (has the most props, so will probably be the best experience)
* Snow
* Seaside
* Luncheon (working but could use some more objects)
* Bowser's
* Moon
* Mushroom  


Props for each kingdom are defined in `include/algorithms/CaptureTypes.h`


# Super Mario Odyssey - Online Multiplayer Mod

Welcome to the official repository for the Super Mario Odyssey Online mod! Have fun exploring kingdoms with friends, playing gamemodes, or beating the game as fast as possible! This mod is still early in development, so expect bugs and un-refined aspects as we work hard to improve it and make the mod as polished as possible.

## Features

* Explore Kingdoms together with up to 10 People
* Almost every capture in the game is synced between players
* Full 2D and Costume models syncing
* Moon Collection is shared between all players
* Custom Configuration Menu (Accessible by holding ZL and selecting any option in the pause/start menu)
* Support for custom gamemodes (WIP)
	

## Installation and Usage
For the typical installation along with how to setup and use muliplayer/hide and seek, please visit the [Super Mario Odyssey Online website](https://smoo.it).

<details>

<summary>Developer build instructions</summary>

  ### Building Prerequisites

  - [devkitPro](https://devkitpro.org/) 
  - Python 3
  - The [Keystone-Engine](https://www.keystone-engine.org/) Python Module

  ### Building

  Build has only been tested on WSL2 running Ubuntu 20.04.1.

  Just run:
  ```
  DEVKITPRO={path_to_devkitpro} make
  ```

  On Ubuntu (and other Debian-based systems), devkitPro will be installed to `/opt/devkitpro` by default:

  ```
  DEVKITPRO=/opt/devkitpro/ make
  ```

  ### Installing (Atmosphère)

  After a successful build, simply transfer the `atmosphere` folder located inside `starlight_patch_100` to the root of your switch's SD card.
</details>

## Troubleshooting

The [Super Mario Odyssey Online website](https://smoo.it) has a FAQ section that should solve many issues.
However, for any further questions or help not covered by the site, please visit the [CraftyBoss Community Discord Server](discord.gg/jYCueK2BqD) and ask in the `help`/`help-2` channel. 

---

# Contributors

- [Sanae](https://github.com/sanae6) Wrote the majority of the server code
- [Shadow](https://github.com/shadowninja108) original author of starlight, the tool used to make this entire mod possible
- [GRAnimated](https://github.com/GRAnimated)

# Credits
- [OdysseyDecomp](https://github.com/shibbo/OdysseyDecomp)
- [OdysseyReversed](https://github.com/shibbo/OdysseyReversed)
- [open-ead](https://github.com/open-ead/sead) sead Headers
