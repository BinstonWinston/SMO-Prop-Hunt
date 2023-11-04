## Bugs
* Other players' props not visible except when looking at odyssey? Unclear on repro

## Fixed/Done
* Props not syncing on other clients when cycling thru props
  * Fix: `e2b1e99`
  * New capture/prop packets weren't being sent out because capture sync assumes you must uncapture before switching captures (which is true for captures, but not props). Fixed by forcing a packet update when cycling thru props
* Crash when switching kingdoms and local player's prop is active
  * Fix: `9b6f897`
  * In stageInit if the world/kingdom ID changes, disable the current prop and switch to seeker to avoid crashing
* Re-bind Dpad-right controls to another button since it conflicts with hider time controls
  *  Updated hold in air and prop cycling to different buttons (see readme)
