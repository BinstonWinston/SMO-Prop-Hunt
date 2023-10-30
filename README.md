# Based on CraftyBoss/SuperMarioOdysseyOnline **v1.4.0**
* [SuperMarioOdysseyOnline](https://github.com/CraftyBoss/SuperMarioOdysseyOnline)
* [SmoOnlineServer](https://github.com/Sanae6/SmoOnlineServer)  
  
Huge thanks to CraftyBoss, Sanae, and all the other folks who made SMO online! This mod uses that code as a base and replaces capture sync with prop sync and adds a bit of additional logic on the client for props. 

### Additional romfs files
Please download the mod [here](https://gamebanana.com/mods/476551) for additional required larger romfs files.

### :warning: Note
This mod uses SMO online as a base and includes that code in this mod package. If you want to play this mod, please remove any other copies of SMO online as it will conflict.

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

# Contributors (from SMO online base repo)
- [Crafty](https://github.com/CraftyBoss)
- [Sanae](https://github.com/sanae6) Wrote the majority of the server code
- [Shadow](https://github.com/shadowninja108) original author of starlight, the tool used to make this entire mod possible
- [GRAnimated](https://github.com/GRAnimated)

# Credits
- [OdysseyDecomp](https://github.com/shibbo/OdysseyDecomp)
- [OdysseyReversed](https://github.com/shibbo/OdysseyReversed)
- [open-ead](https://github.com/open-ead/sead) sead Headers
