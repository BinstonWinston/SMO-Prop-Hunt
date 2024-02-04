TODO
<!-- * Fix props to only display as LOD 0 -->
<!-- * Enable prop collisions for other players -->
<!-- * Tagging can only be done by throwing your cap at a prop? -->
* Decoy props
  <!-- * Send decoy prop info as PlayerInfPacket with a flag set every 60 frames, instead of caching on clients or something -->
  * Also decoys don't check stage data yet, so if the decoy owner is in a different stage it'll likely mess up. When receiving a new prop packet, use this to check decoy stage data
  <!-- * Local player's decoy prop doesn't reappear after death -->
  <!-- * Decoy prop stays in position, but changes type when the owner switches prop type. Only visible on other clients, not the local client -->
  * Also there's a bug that when you die you don't automatically become seeker, you stay a hider. Idk why
  <!-- * Disable decoy props when maxPlayers > 4 -->