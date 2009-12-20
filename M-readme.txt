=============================================================================
      AQ2: The Next Generation - M0.22  --- M-readme.txt
=============================================================================



--------------------------------------
     What is this stuff?
--------------------------------------
This package will give you a more advanced and less bugged version of the
AQ2 Modification (mod) "The Next Generation" (TNG) 2.81.
TNG 2.81 is the latest version of the TNG-Team and hasn't been updated since
ages, namely years. See their SF site here: http://aq2-tng.sourceforge.net/

TNG has proven being a really stable AQ2 and innovative version but there
were still some bugs and feature requests around. It was time to get rid of
them - so this is the result!

We're proud to announce the..

-- Unofficial Update M0.22 to AQ2: The Next Generation 2.81 --
-- First public releasedate: 2006-02-04. (Woooo 2006 and still kicking!)
by Maniac. (maniac_@luukku.com)

This readme will tell you about the new features that have been applied. 
If you're looking for the old TNG documentation of commands etc. then 
you may want to read the good ol' TNG-manual, which is included in this 
package as well. If you still have trouble visit the TNG-website. 
http://aq2-tng.sourceforge.net/


For more details about all changes and fixed bugs read up in our changelog!

Have Fun!



--------------------------------------
             Installation
--------------------------------------
This one is easy: just replace the old server mod library in your action
directory (quake2/action/) with the new one. If you have the q2admin running
proxy-mod running, then the actual game-library may be named 
"gamei386.real.so".

You can either use the pre-built gamelibrary or build a new one from source.

Since this only is a library update you will need all other stuff (datafiles
etc.) from the TNG website http://aq2-tng.sourceforge.net/.



--------------------------------------
            New Features
--------------------------------------


--------------------------
Team Deathmatch Mode (TDM)
--------------------------
A new playing mode! Sure, there has always been a possible way to play 
"team deathmatch" by chosing teams over skin/model and a certain dmflags 
value for "chosing team by skin". It was a pain in the ass to manage nice
games with that.

Now you don`t have to play with those silly things anymore. You can just join
teams as if it were the teamplay mode. This even works in matchmode!
Team score is calculated by summing up all frags done by the team players.

The Team Deathmatch gameplay is derived from the Quake series (Quakeworld 
Kteams Pro, Quake2 Battleground, Quake3 Arena OSP) gaming experience. So 
there are a few things that you should know:

- Enable spawnsounds for better direction hearing! (see below too!)
  set respawn_effect "1"
- Friendly Fire must be ENABLED. Means: dmflags without 256.
  - teamkill kickbanning must be disabled. It is punishment enough to get
    teamfrags decreased. If someone screws it up, votekick him.
- dmflags should be with 1024 to force spawning. Thus, there is no cheating 
  by preventing spawns.
- For more action: "set items 1" and "set weapons 2" and
  - set item_respawn "20"
  - set weapon_respawn "25"
  - set ammo_respawn "15"

These ARE settings tried and figured out by testing sessions! Anyways
feel free to fuddle with them. :P Perhaps suggest some settings to us.


New TDM-Cvars:
--------------
set teamdm "1"          - To enable Team Deathmatch mode. Set to "2" for 
                          deathmatch bonus. Defaults to "0". Recommended "1".
set teamdm_respawn "2"  - Time until respawn, in seconds. Defaults to "2".


-------------------------
New features in Matchmode
-------------------------

- Matchmode can now be used by "CTF", "Team Deathmatch" and "3-Teams"
---
This is cool when you want to play clanwars with those modes. No more waiting
in spectator-mode and all players synchronously joining the teams when 
everyone is ready.
Now you can just join the teams and become "captain". As captain, type "ready"
when your team is ready. As a captain, you may change your "teamskin" and
"lock" your team.
You know that already if you have played normal roundbased matchmode teamplay.

- Pausing the game
---
Since CTF and TDM are no roundbased modes, you can't just "unready" your team.
This would kill all players alive and they would lose all their weapons. That
would be a shame! No?!
For this purpose, there is a new command: "pausegame".

The command allows a team to pause the game for a certain time and only a member
of the same team may unpause the small break.
Note!: This command doesn't need captain privileges!

New Matchmode Commands:
-----------------------
pausegame   - To pause
unpausegame - Guess what... to unpause :)

New Matchmode Cvars:
--------------------
set mm_pausecount "3"  - Controls how often a team may issue a pause per map. 
                         Default: 3.
set mm_pausetime "2"   - Controls for how long the pause will last (in minutes). 
                         Default: "2".

- Resetting scores
---
Sometimes you want to play a map again. Before, you had to use the votemap <map>
or the rcon map <map> commands to reset scores. Those times are gone.

Now in matchmode, you can reset scores just by using the new 
command: "resetscores" 

This will reset all teamscores/time and player statistics. The command can be 
used by matchadmins or captains. When captains are using it, it will need to 
be accepted by the other captain as well. He will be asked if he wants to 
accept "resetscores". 
Also, with rcon you can just type 'sv resetscores' - making life a bit easier
for leagueadmins.

New Matchmode Commands:
-----------------------
resetscores    - To reset scores as matchadmin or captain
sv resetscores - To reset scores over remote console (rcon)


------------------
Other new settings
------------------

New Deathmatch and Team Deathmatch Cvars:
-----------------------------------------
set item_respawn "59"    - item respawntime when it got dropped. Default: "59"
set weapon_respawn "74"  - weapon respawntime when it got dropped. Default: "74"
set ammo_respawn "39"    - ammo respawntime after it got picked up. Default: "39"

set respawn_effect "0"   - Adding an effect+sound when respawning. Disabled by 
                          default. But ENABLE THIS for Team Deathmatch Mode!

set 'item_respawnmode' 1 - Change item respawning. Normally item would only spawn at random deathmatch spawn.
  			   The new mode disables it and spawns item only if map has a spawn point for this item.

Other:
------------------------
cvar 'wave_time' 	- Specify delay between each wave commands in seconds. Default "5"


--------------------------------------
            Contact
--------------------------------------
Maniac - maniac_@luukku.com 



--------------------------------------
            Credits
--------------------------------------
- AQ2: The Next Generation Team, for the great updates to this mod. Read the 
  credits from TNG manual!

- Haudrauf for ideas and testing gameserver
- Stric for ideas
- all others who helped testing this in action.
- All AQ2 players out there who still keep this mod alive!!

