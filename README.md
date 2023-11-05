# Based on CraftyBoss/SuperMarioOdysseyOnline **v1.4.0**
* [SuperMarioOdysseyOnline](https://github.com/CraftyBoss/SuperMarioOdysseyOnline)
* [SmoOnlineServer](https://github.com/Sanae6/SmoOnlineServer)  
  
Huge thanks to CraftyBoss, Sanae, and all the other folks who made SMO online! This mod uses that code as a base and replaces capture sync with prop sync and adds a bit of additional logic on the client for props. 

### :warning: Note
This mod uses SMO online as a base and includes that code in this mod package. If you want to play this mod, please remove any other copies of SMO online as it will conflict.

The SMOO server is unchanged so running any SMOO server (that supports client v1.4.0) should work, though you should ensure all users are using Prop Hunt and none are using a standard hide and seek build, otherwise it could cause issues. The commit `dd0de0d build binary files via docker` from SmoOnlineServer has been tested to work with this build (tho any build should work as long as it supports client v1.4.0 that this is based on)

### Controls (same as SMO online with these additions)
* Randomize Prop (switch between hider and seeker)
* Rotate thru Props (hold R + press left stick)
* Freeze in mid-air (hold L + press left stick), useful for getting the exact positioning for a prop to blend in

### Rules
* Standard Hide&Seek Rules
* No hiding or running away in Captures as they do not sync on the prop hunt build (using a capture to get to a place with your prop is okay)
* Be reasonable, don't pick a small prop and just hide in a tiny crevice, try to blend in with normal stage design. If people aren't finding you after too long, try moving around, switching props, or giving more detailed hints, have fun with it!
* Mid-air freezing is only allowed to line up props to make them look realistic, no using it to hide at a hard-to-reach spot high in the air or to gain infinite height
* Choose one of
    * Random props at the start of the round, no switching props, you get what you get
    * Hiders can choose their prop and cycle thru while escaping (e.g. run around a corner and turn into a different nearby object)

### :warning: Known Issues
* Sometimes props take a sec to appear locally, try moving around or switching between hider and seeker

## Best Kingdoms
* Metro
* Cap
* Cascade

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
