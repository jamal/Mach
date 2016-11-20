# Mach

Mach is a prototype project to create a multiplayer FPS game in Unreal Engine 4. The goal is to create an FPS that focuses on customization, strategy and teamplay. To do this, the game allows the player to choose from a variety of gear before entering a match including choices for Armor, Helmet, Weapon, and Secondary Item. Each choice unlocked certain abilities, allowing players to choose their play style and role in the team based on the choices. The game mode focused on strategy and map control, having multiple resource nodes around the map that allowed players to unlock upgrades for their gear boosting damage and unlocking certain abilities. The goal of the game was to work with the team to both defend your resource nodes, while going on the offensive to capture your enemies' resources and gain the upper hand to finally overpower your enemies.

## Gear

### Armor

Armor acted as the primary class selection for a character, providing the most drastic abilities and defining movement. 

 - Light armor: Offered faster movement speed and double-jumping, but offered less HP and Shield than other armor types. Also gives player blink ability which allows them to teleport a small distance in any direction.
 - Medium armor: Average walking speed, jetpack-style boosters which gave the ability to hover in the air for period of times or dash horizontally across the map.
 - Stealth armor: Average walking speed, allowed the player to stealth for a short period of time. Also has a grappling hook which allows the player to pull themselves across the map and latch to walls and ceilings.
 - Heavy armor: Slower walking speed, much higher health and shield.

### Primary Weapon

The primary weapon acts as the player's offensive ability. Each weapon has an energy supply.

 - Rifle: Primary: Standard fast-firing rifle with light spread. Secondary mode: Sniper scope with charged shots that deal higher damage. 
 - Shotgun: Primary: Shoots a cluster of bullets that spread away from the muzzle, at close range all bullets should hit the target, but at longer range the bullets will spread away from the center. Secondary: Focus shot over time which would not allow movement but increases accuracy.
 - Energy gun: Primary: Shoot an energy projectile that can be charged up by holding down fire. The more charge the projectile has, the more damage it will deal. Charging will slowly deplete your energy. Reloads passively when not in use.
 - Grenade launcher: Primary: Shoot a grenade projectile that flies slightly slowly but will explode and deal high spread damage to any player near the blast. The projectile should explode if it hits a damageable target, or after it stops moving. Secondary: Sticky grenades which can be thrown to the world and will stay in place until an enemy player gets nearby which will cause it to explode. This should not be fatal.

### Helmet

Helmet offers various visibility abilities. Only one helmet type has been implemented which provides a minimap displaying detected enemies near the player.
