TODO
<!-- * Fix props to only display as LOD 0 -->
<!-- * Enable prop collisions for other players -->
<!-- * Tagging can only be done by throwing your cap at a prop? -->
* Decoy props
  * Working local and online displays, but you lose track of all decoys when you die, so if a seeker dies while searching they won't see decoys anymore. Need to fix this so decoys are cached
  * Also decoys don't check stage data yet, so if the decoy owner is in a different stage it'll likely mess up
  * Also there's a bug that when you die you don't automatically become seeker, you stay a hider. Idk why