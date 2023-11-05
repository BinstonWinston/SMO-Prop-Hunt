## Fixed/Done
* Props not syncing on other clients when cycling thru props
  * Fix: `e2b1e99`
  * New capture/prop packets weren't being sent out because capture sync assumes you must uncapture before switching captures (which is true for captures, but not props). Fixed by forcing a packet update when cycling thru props
* Crash when switching kingdoms and local player's prop is active
  * Fix: `9b6f897`
  * In stageInit if the world/kingdom ID changes, disable the current prop and switch to seeker to avoid crashing
* Re-bind Dpad-right controls to another button since it conflicts with hider time controls
  *  Updated hold in air and prop cycling to different buttons (see readme)
* Cascade and seaside kingdom have props that aren't visible to other players
  * Fix: `7883012`
  * Cause: CaptureEntry.className in HackModelHolder.hpp had a string size of 0x16, which wasn't long enough to include some longer prop names
  * Fix info: Updated all client-side prop string names to have a length of 0x40 to be safe, and updated the CaptureInf packet logic to just pack the Capture type enum into the first byte/char of the hackName string (so no server logic needs to be updated to handle longer strings)
* Prop visibility issues in some cases (mostly for your own prop when looking at different angles)
  * Fix: `899bddd`
  * Fix info: added al::invalidateOcclusionQuery to PropActor and PuppetHackActor initialization
