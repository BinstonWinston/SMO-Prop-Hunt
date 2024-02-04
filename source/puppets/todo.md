TODO
<!-- * Fix props to only display as LOD 0 -->
<!-- * Enable prop collisions for other players -->
<!-- * Tagging can only be done by throwing your cap at a prop? -->
* Decoy props. Hacky, just track decoy on client side and place it exactly where the player is currentlyl standing and the same prop type. And send tpackets  as a new fake player(just flip a single bit in the players UUID). Don't even need display logic client side, it'll come in from the server as a new player even on the decoy owners client.  You're allowed one decoy ata time, can be placed with R+click left stick. If you place  another deocy the previous one dissapears