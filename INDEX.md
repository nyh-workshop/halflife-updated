# Custom Weapon: Hammer from Rich Whitehouse's *Scientist Hunt*

Rich Whitehouse's [Scientist Hunt](https://richwhitehouse.com/index.php?content=inc_projects.php&showproject=11) is a hilarious Half-Life 1 mod. It has been there since 1999-2000, I remembered playing it as a teenager and had a lot of fun with it in rainy weekend nights. 

This small mod adds a ridiculous hammer from that mod, and most of the behaviour of the hammer is approximated, and successfully replicated.

Unfortunately, the `p_hammer.mdl` is not rendering well - possibly the swinging animation (from the 3rd person view) is not included. The `v_hammer.mdl` is working fine as always.

## Extra add-ons:
- The hammer can be charged for a maximum 2 seconds, by holding primary or secondary attack.

- The Primary Attack swings the hammer, and punts the opponent into the air, spinning in a mad fashion. If the opponent touches the wall when punted by the hammer, it gibs immediately and leaving the wall with the blood decals. The longer the charge, the further the punt. (Currently the code targets only scientists - use the `FClassnameIs` to check the entity for your desired character or `monster` in the SDK's language)

- The Secondary Attack slams the hammer and any unlucky thing near it would be instantly gibbed. The longer the charge, the more the damage.

## Testing:
For testing, you can use the scihunt's `field.bsp`.

## Issues:
- Client-side hammer code has not been written yet. It is not tested in multiplayer.