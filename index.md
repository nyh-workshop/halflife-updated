# Separate ammunitions for MP5 and Glock 17

Been playing the game for more than 25 years, I noticed one little funny thing - the Glock handgun and the MP5 submachine gun was **sharing the same pool of ammunition**.

For this one and based on the [tutorial](https://gamebanana.com/tuts/14811), the ammunition is separated to MP5's 9mm, and Glock's 40 cal. As I'm not a weapons expert, it is assumed that the Glock in this change is a fictional one.

## Issues
You have to enable some console commands there:
- `cl_lw 0` -> This prevents the frequent non-animations of the reloads of some weapons like mp5 and glock. It is not tested in multiplayer so I'm still finding out if there are side effects on this one.
 
## Skill.cfg
In the `skill.cfg` you need to insert this inside, or else the weapon doesn't register damage:
(*Note: Again, I do not know what kind of damage these ammunition do. It is a rough guess after searching around!*)
```
 // 40 cal Round
sk_plr_40_bullet1 "21"
sk_plr_40_bullet2 "21"
sk_plr_40_bullet3 "21"
```

## Future improvements
Well, why not change the appearances of the clip, or maybe add another similar handgun?