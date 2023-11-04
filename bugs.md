## Bugs
* Crash when switching kingdoms and local player's prop is active
* Other players' props not visible except when looking at odyssey?

## QOL
* Update Dpad-right prop cycling to another button since it conflicts with hider time controls

## Fixed/Done
* Props not syncing on other clients when cycling thru props
  * Fix: `e2b1e99`
  * New capture/prop packets weren't being sent out because capture sync assumes you must uncapture before switching captures (which is true for captures, but not props). Fixed by forcing a packet update when cycling thru props