##############################################################################
#
# This is the OLD changelog for the PG Bund Edition Release, on which AQ2-TNG
# is based.
#
##############################################################################

Action Quake: Edition
Version 1.25 as of April 2, 2001

by Ruediger Bund ("PG Bund[Rock]"), Benjamin Greiner ("=DIE=TempFile"),
Sven Herrmann ("=DIE=Woodoo") and Stefan Giesen ("Igor[Rock]").

Changes 1.24 -> 1.25 (04/02/2001)

* Several bug fixes

+ M4 spread resetted to original value (300), instead reduced the spread for
  MP5 (240 instead 250) and Akimbos (275 instead 300).

+ grenades are a little bit more powerful, so they're not as useless as
  they were sinces AQ 1.52 (250 instead 170 - in AQ2 1.51 they were even
  stronger) - should stop pepole using grenade jumps in aq2 (after all, it
  should be an ACTION mod - have you EVER seen someone doing grenade jumps
  in a real life action film? (exclude Matrix - it's a science fiction...)

+ fixed the unlimited ammo exploit

+ Random map rotation

+ Message "You've started bandaging" added

+ Kevlar Helmet added (get the client archive for the model and skin)

+ 3 teams mode added

+ Voterotation added (computes the all time votes for maps and changes
  the maplist as wanted. If a map is played the amount of players is
  subtracted form the all time votes value.

+ Lowlag sounds implemented (http://aqdt.fear.net/, also in the
  pgbund-1.25-client.zip) (Use llsound 0 to disable)

+ Additional Sounds and Messages added ("Accuracy", "Impressive",
  "Excellent", "Team X wins", "3-2-1 Frags left", "3-1 minutes left") - get
  the pgbund client file from http://action.gametown.de/

+ added "Leave Team" to the menu

+ added the total kills per player (without subtracting teamkills or
  own death)

+ added maplistfile feature

+ added configfile feature

+ added 40 seconds vote block after map reload (to protect against fast
  connecting players which vote for a new map if the one starting doesn't
  please them - so some maps actually never get played, even if most people
  on the server would like to play it) 

+ added config voting (list available configs in "configlist.ini" without
  extension)

+ Starting weapon for DM mode (dmweapon) - you get the weapon with full
  ammo as in TP.

Changes 1.23 -> 1.24 (01/22/2000)

* Several bug fixes

+ sv_allowcrlf server cvar: if 0, clients are not allowed to issue CR/LF
  control characters in chat messages. This is done by proxies to hide the
  chatter's name in order to replace it with an abbreviation, e.g. in a
  clanwar, but very often misused in normal games. The default setting is 1,
  so if you want to disable the CR/LF use just set it to 0.

+ The M4 spread has been slighty increased so there shouldn't be far too
  much headshots now.

Changes 1.22 -> 1.23 (11/02/1999)

+ Notification when a player changes name

* Soundlist overflow bug fixed

+ Server-side cvar "sv_gib" toggles gibbing, this can improve lagginess


Changes 1.21 -> 1.22 (8/25/1999)

* RADIO: When you survived a round, the kill count wasn't reset when the
  new round started. Led to wrong autonumbering when reporting enemyd. Fixed.

* LCA time increased by .2 sec.

* Fixed format string bug in kicklist and maplist commands.

* "game type" MOTD entry always reported tourney even in normal teamplay; fixed.

* Team kills no longer add to damage points.


Changes 1.20b2 -> 1.21 (8/17/1999)

+ TEAMPLAY: If you kill several enemies before doing "radio enemyd", the
  number of enemies killed is prefixed to your radio message.


Fixes 1.20b1 -> b2

* Live players don't get death messages anymore if they aren't involved in the
  killing

* Ignore Spamming is no longer possible

* "You'll get to your weapon after bandaging" flood bug fixed


Changes 1.10 -> 1.20b1

* Updated codebase to Action 1.52, that implies all changes done by the
  A-Team in that revision.

* Listen servers didn't work because the local IP "loopback" was treated as 
  invalid and got banned; fixed.

- Friendly Fire: Because this feature is now in original Action Quake, all ff-related
  cvars including banning and suicide limits were transformed to the format introduced
  by the A-Team. See the Action 1.52 CHANGES file.

* Improved location detection

* With a script, one was able to suppress the gun firing sound using the punch
  command; fixed.
  The way this bug was fixed also makes "punchspamming" impossible, there is now
  a .65 sec delay after each punch.

* Locations are now defined by cubes. See adf.txt.

- Completely dropped support for .pg files.

* Gibbing is back for the sniper rifle! See for yourself. :)

* Fixed smaller bugs.

+ Emotions: If the message begins with "%me", this sequence is replaced by
  the player's name and the "name:" part is cut off, just like in IRC.
  Note that this works with %, since / is ignored by Quake.

* Various detail improvements.

+ The lens command is back, and it comes with arguments (Sniper Rifle only):
  lens		zooms in (with overflow, means 6x will become 1x)
  lens in	zooms in as well, but without overflow
  lens out	zooms out, no overflow
  lens 1	unzooms
  lens 2	zooms to 2x
  lens 4	zooms to 4x
  lens 6	zooms to 6x

+ If you reload while using dual pistols and carrying only one pistol clip,
  you automatically change to single pistol, then reload.
