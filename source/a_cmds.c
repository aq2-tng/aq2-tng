//-----------------------------------------------------------------------------
// zucc
// File for our new commands.
//
// laser sight patch, by Geza Beladi
//
// $Id: a_cmds.c,v 1.33 2003/06/15 15:34:32 igor Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_cmds.c,v $
// Revision 1.33  2003/06/15 15:34:32  igor
// - removed the zcam code from this branch (see other branch)
// - added fixes from 2.72 (source only) version
// - resetted version number to 2.72
// - This version should be exactly like the release 2.72 - just with a few
//   more fixes (which whoever did the source only variant didn't get because
//   he didn't use the CVS as he should. Shame on him.
//
// Revision 1.32  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.31  2002/09/04 11:23:09  ra
// Added zcam to TNG and bumped version to 3.0
//
// Revision 1.30  2002/04/01 15:16:06  freud
// Stats code redone, tng_stats now much more smarter. Removed a few global
// variables regarding stats code and added kevlar hits to stats.
//
// Revision 1.29  2002/03/28 13:30:36  freud
// Included time played in ghost.
//
// Revision 1.28  2002/03/25 23:35:19  freud
// Ghost code, use_ghosts and more stuff..
//
// Revision 1.27  2002/03/25 18:32:11  freud
// I'm being too productive.. New ghost command needs testing.
//
// Revision 1.26  2002/03/25 16:34:39  freud
// Cmd_Time changes. When timelimit is 0 it says timelimit disables instead of
// displaying 0 minutes 0 seconds left.
//
// Revision 1.25  2002/03/24 19:38:05  ra
// Securityfix
//
// Revision 1.24  2002/01/24 11:29:34  ra
// Cleanup's in stats code
//
// Revision 1.23  2002/01/24 02:24:56  deathwatch
// Major update to Stats code (thanks to Freud)
// new cvars:
// stats_afterround - will display the stats after a round ends or map ends
// stats_endmap - if on (1) will display the stats scoreboard when the map ends
//
// Revision 1.22  2002/01/24 01:40:40  deathwatch
// Freud's AutoRecord
//
// Revision 1.21  2001/11/25 19:09:25  slicerdw
// Fixed Matchtime
//
// Revision 1.20  2001/11/03 17:21:57  deathwatch
// Fixed something in the time command, removed the .. message from the voice command, fixed the vote spamming with mapvote, removed addpoint command (old pb command that wasnt being used). Some cleaning up of the source at a few points.
//
// Revision 1.19  2001/10/26 00:40:07  ra
// Minor fix to time command
//
// Revision 1.18  2001/10/18 12:55:35  deathwatch
// Added roundtimeleft
//
// Revision 1.17  2001/10/18 12:45:47  ra
// Disable time command in matchmode
//
// Revision 1.16  2001/09/30 03:09:34  ra
// Removed new stats at end of rounds and created a new command to
// do the same functionality.   Command is called "time"
//
// Revision 1.15  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.14  2001/09/02 20:33:34  deathwatch
// Added use_classic and fixed an issue with ff_afterround, also updated version
// nr and cleaned up some commands.
//
// Updated the VC Project to output the release build correctly.
//
// Revision 1.13  2001/08/18 18:45:19  deathwatch
// Edited the Flashlight movement code to the Lasersight's movement code, its probably better
// and I added checks for darkmatch/being dead/being a spectator for its use
//
// Revision 1.12  2001/08/17 21:31:37  deathwatch
// Added support for stats
//
// Revision 1.11  2001/08/15 14:50:48  slicerdw
// Added Flood protections to Radio & Voice, Fixed the sniper bug AGAIN
//
// Revision 1.10  2001/08/06 12:13:07  slicerdw
// Fixed the Sniper Weapon plus reloading bug
//
// Revision 1.9  2001/07/30 16:04:37  igor_rock
// - changed the message for disallowed items (it said "Weapon not...")
//
// Revision 1.8  2001/07/28 19:30:05  deathwatch
// Fixed the choose command (replaced weapon for item when it was working with items)
// and fixed some tabs on other documents to make it more readable
//
// Revision 1.7  2001/07/25 23:02:02  slicerdw
// Fixed the source, added the weapons and items capping to choose command
//
// Revision 1.6  2001/07/16 19:02:06  ra
// Fixed compilerwarnings (-g -Wall).  Only one remains.
//
// Revision 1.5  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.4.2.1  2001/05/20 15:17:31  igor_rock
// removed the old ctf code completly
//
// Revision 1.4  2001/05/13 01:23:01  deathwatch
// Added Single Barreled Handcannon mode, made the menus and scoreboards
// look nicer and made the voice command a bit less loud.
//
// Revision 1.3  2001/05/11 16:07:25  mort
// Various CTF bits and pieces...
//
// Revision 1.2  2001/05/07 02:05:36  ra
//
//
// Added tkok command to forgive teamkills.
//
// Revision 1.1.1.1  2001/05/06 17:24:14  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"


extern void P_ProjectSource(gclient_t * client, vec3_t point,
			    vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);

/*----------------------------------------
 * SP_LaserSight
 *
 * Create/remove the laser sight entity
 *---------------------------------------*/

void SP_LaserSight(edict_t * self, gitem_t * item)
{
	vec3_t start, forward, right, end;

	if (!INV_AMMO(self, LASER_NUM)) {
		if (self->lasersight) {  // laser is on
			G_FreeEdict(self->lasersight);
			self->lasersight = NULL;
		}
		return;
	}
	//zucc code to make it be used with the right weapons

	switch (self->client->curr_weap) {
	case MK23_NUM:
	case MP5_NUM:
	case M4_NUM:
		break;
	default:
		// laser is on but we want it off
		if (self->lasersight) {
			G_FreeEdict(self->lasersight);
			self->lasersight = NULL;
		}
		return;
	}

	AngleVectors(self->client->v_angle, forward, right, NULL);
	VectorSet(end, 100, 0, 0);
	G_ProjectSource(self->s.origin, end, forward, right, start);
	self->lasersight = G_Spawn();
	self->lasersight->owner = self;
	self->lasersight->movetype = MOVETYPE_NOCLIP;
	self->lasersight->solid = SOLID_NOT;
	self->lasersight->classname = "lasersight";
	self->lasersight->s.modelindex = gi.modelindex("sprites/lsight.sp2");
	self->lasersight->s.renderfx = RF_TRANSLUCENT;
	self->lasersight->think = LaserSightThink;
	self->lasersight->nextthink = level.time + 0.01;
}

/*---------------------------------------------
 * LaserSightThink
 *
 * Updates the sights position, angle, and shape
 * is the lasersight entity
 *-------------------------------------------*/

void LaserSightThink(edict_t * self)
{
	vec3_t start, end, endp, offset;
	vec3_t forward, right, up, angles;
	trace_t tr;
	int height = 0;

	// zucc compensate for weapon ride up
	VectorAdd(self->owner->client->v_angle, self->owner->client->kick_angles, angles);
	AngleVectors(angles, forward, right, up);

	if (self->owner->lasersight != self) {
		self->think = G_FreeEdict;
	}

	if (self->owner->client->pers.firing_style == ACTION_FIRING_CLASSIC)
		height = 8;

	VectorSet(offset, 24, 8, self->owner->viewheight - height);
	P_ProjectSource(self->owner->client, self->owner->s.origin, offset, forward, right, start);
	VectorMA(start, 8192, forward, end);

	PRETRACE();
	tr = gi.trace(start, NULL, NULL, end, self->owner, CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER);
	POSTTRACE();

	if (tr.fraction != 1) {
		VectorMA(tr.endpos, -4, forward, endp);
		VectorCopy(endp, tr.endpos);
	}

	vectoangles(tr.plane.normal, self->s.angles);
	VectorCopy(tr.endpos, self->s.origin);
	gi.linkentity(self);
	self->nextthink = level.time + 0.1;
}

void Cmd_New_Reload_f(edict_t * ent)
{
//FB 6/1/99 - refuse to reload during LCA
	if (lights_camera_action)
		return;
	ent->client->reload_attempts++;
}

// Handles weapon reload requests
void Cmd_Reload_f(edict_t * ent)
{
	//+BD - If the player is dead, don't bother
	if (ent->deadflag == DEAD_DEAD) {
		//gi.centerprintf(ent, "I know you're a hard ass,\nBUT YOU'RE FUCKING DEAD!!\n");
		return;
	}

	if (ent->client->weaponstate == WEAPON_BANDAGING ||
	    ent->client->bandaging == 1 ||
	    ent->client->bandage_stopped == 1 ||
	    ent->client->weaponstate == WEAPON_ACTIVATING ||
	    ent->client->weaponstate == WEAPON_DROPPING || ent->client->weaponstate == WEAPON_FIRING) {
		return;
	}

	if (!ent->client->fast_reload)
		ent->client->reload_attempts--;
	if (ent->client->reload_attempts < 0)
		ent->client->reload_attempts = 0;

	//First, grab the current magazine max count...

	//Set the weaponstate...
	switch(ent->client->curr_weap) {
	case M3_NUM:
		if (ent->client->shot_rds >= ent->client->shot_max)
			return;

		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		// already in the process of reloading!
		if (ent->client->weaponstate == WEAPON_RELOADING &&
		    (ent->client->shot_rds < (ent->client->shot_max - 1)) &&
		    !(ent->client->fast_reload) &&
		    ((ent->client->pers.inventory[ent->client->ammo_index] - 1) > 0)) {
			// don't let them start fast reloading until far enough into the firing sequence
			// this gives them a chance to break off from reloading to fire the weapon - zucc
			if (ent->client->ps.gunframe >= 48) {
				ent->client->fast_reload = 1;
				(ent->client->pers.inventory[ent->client->ammo_index])--;
			} else {
				ent->client->reload_attempts++;
			}
		}
		break;
	case HC_NUM:
		if (ent->client->cannon_rds >= ent->client->cannon_max)
			return;

		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		if(hc_single->value)
		{
			if(ent->client->resp.hc_mode || ent->client->cannon_rds == 1)
			{	if(ent->client->pers.inventory[ent->client->ammo_index] < 1)
					return;
			}
			else if(ent->client->pers.inventory[ent->client->ammo_index] < 2)
				return;
		}
		else if (ent->client->pers.inventory[ent->client->ammo_index] < 2)
			return;
		break;
	case SNIPER_NUM:
		if (ent->client->sniper_rds >= ent->client->sniper_max)
			return;

		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		// already in the process of reloading!
		if (ent->client->weaponstate == WEAPON_RELOADING
		    && (ent->client->sniper_rds < (ent->client->sniper_max - 1))
		    && !(ent->client->fast_reload)
		    && ((ent->client->pers.inventory[ent->client->ammo_index] - 1) > 0)) {
			// don't let them start fast reloading until far enough into the firing sequence
			// this gives them a chance to break off from reloading to fire the weapon - zucc
			if (ent->client->ps.gunframe >= 72) {
				ent->client->fast_reload = 1;
				(ent->client->pers.inventory[ent->client->ammo_index])--;
			} else {
				ent->client->reload_attempts++;
			}
		}
		ent->client->ps.fov = 90;
		if (ent->client->pers.weapon)
			ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);
		break;
	case DUAL_NUM:
		if (ent->client->dual_rds == ent->client->dual_max)
			return;

		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		//TempFile change to pistol, then reload
		if (ent->client->pers.inventory[ent->client->ammo_index] == 1) {
			gitem_t *it;

			it = GET_ITEM(MK23_NUM);
			it->use(ent, it);
			ent->client->autoreloading = true;
			return;
		}

		break;
	case MP5_NUM:
		if (ent->client->mp5_rds == ent->client->mp5_max)
			return;
		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		break;
	case M4_NUM:
		if (ent->client->m4_rds == ent->client->m4_max)
			return;
		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		break;
	case MK23_NUM:
		if (ent->client->mk23_rds == ent->client->mk23_max)
			return;
		if(ent->client->pers.inventory[ent->client->ammo_index] <= 0) {
			gi.cprintf(ent, PRINT_HIGH, "Out of ammo\n");
			return;
		}
		break;
	default:
	//We should never get here, but...
	//BD 5/26 - Actually we get here quite often right now. Just exit for weaps that we
	//          don't want reloaded or that never reload (grenades)
		return;
	}

	ent->client->weaponstate = WEAPON_RELOADING;
}

//+BD END CODE BLOCK

//tempfile BEGIN
/*
   Function _SetSniper
   (move to g_weapon.c?)
   Arguments:
   ent: client edict for which to set sniper mode
   zoom: zoom level to set. Set to 1, 2, 4 or 6.

   Return value: none
 */
void _SetSniper(edict_t * ent, int zoom)
{
	int desired_fov, sniper_mode, oldmode;

	switch (zoom) {
	default:
	case 1:
		desired_fov = SNIPER_FOV1;
		sniper_mode = SNIPER_1X;
		break;
	case 2:
		desired_fov = SNIPER_FOV2;
		sniper_mode = SNIPER_2X;
		break;
	case 4:
		desired_fov = SNIPER_FOV4;
		sniper_mode = SNIPER_4X;
		break;
	case 6:
		desired_fov = SNIPER_FOV6;
		sniper_mode = SNIPER_6X;
		break;
	}

	oldmode = ent->client->resp.sniper_mode;

	if (sniper_mode == oldmode)
		return;

	//Moved here, no need to make sound if zoom isnt changed -M
	gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);

	ent->client->resp.sniper_mode = sniper_mode;
	ent->client->desired_fov = desired_fov;

	if (sniper_mode == SNIPER_1X && ent->client->pers.weapon)
		ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);
	//show the model if switching to 1x

	if (oldmode == SNIPER_1X && ent->client->weaponstate != WEAPON_RELOADING) {
		//do idleness stuff when switching from 1x, see function below
		ent->client->weaponstate = WEAPON_BUSY;
		ent->client->idle_weapon = 6;
		ent->client->ps.gunframe = 22;
	}
}

//tempfile END

void Cmd_New_Weapon_f(edict_t * ent)
{
	ent->client->weapon_attempts++;
	if (ent->client->weapon_attempts == 1)
		Cmd_Weapon_f(ent);
}


int _SniperMode(edict_t *ent)
{
	switch (ent->client->desired_zoom) { //lets update old desired zoom
	case 1:
		return SNIPER_1X;
	case 2:
		return SNIPER_2X;
	case 4:
		return SNIPER_4X;
	case 6:
		return SNIPER_6X;
	}
	return ent->client->resp.sniper_mode;
}
//TempFile BEGIN

void _ZoomIn(edict_t * ent, qboolean overflow)
{
	switch (_SniperMode(ent)) {
	case SNIPER_1X:
		ent->client->desired_zoom = 2;
		break;
	case SNIPER_2X:
		ent->client->desired_zoom = 4;
		break;
	case SNIPER_4X:
		ent->client->desired_zoom = 6;
		break;
	case SNIPER_6X:
		if (overflow)
			ent->client->desired_zoom = 1;
		break;
	}
}

void _ZoomOut(edict_t * ent, qboolean overflow)
{
	switch (_SniperMode(ent)) {
	case SNIPER_1X:
		if (overflow)
			ent->client->desired_zoom = 6;
		break;
	case SNIPER_2X:
		ent->client->desired_zoom = 1;
		break;
	case SNIPER_4X:
		ent->client->desired_zoom = 2;
		break;
	case SNIPER_6X:
		ent->client->desired_zoom = 4;
		break;
	}
}

void Cmd_NextMap_f(edict_t * ent)
{
	if (level.nextmap[0])
	{
		gi.cprintf (ent, PRINT_HIGH, "Next map in rotation is %s (%d/%d).\n", level.nextmap, cur_map+1, num_maps);
		return;
	}
	if ((cur_map+1) >= num_maps)
		gi.cprintf (ent, PRINT_HIGH, "Next map in rotation is %s (%d/%d).\n", map_rotation[0], 1, num_maps);
	else
		gi.cprintf (ent, PRINT_HIGH, "Next map in rotation is %s (%d/%d).\n", map_rotation[cur_map+1], cur_map+2, num_maps);

}

void Cmd_Lens_f(edict_t * ent)
{
	int nArg;
	char args[8];

	if (ent->client->curr_weap != SNIPER_NUM)
		return;

	nArg = atoi(gi.args());

	if (nArg == 0) {
		Q_strncpyz(args, gi.args(), sizeof(args));
		//perhaps in or out? let's see.
		if (Q_stricmp(args, "in") == 0)
			_ZoomIn(ent, false);
		else if (Q_stricmp(args, "out") == 0)
			_ZoomOut(ent, false);
		else
			_ZoomIn(ent, true);

		if(!ent->client->desired_zoom)
			return;
	}
	else if ((nArg == 1) || (!(nArg % 2) && (nArg <= 6)))
		ent->client->desired_zoom = nArg;
	else
		_ZoomIn(ent, true);

	if(ent->client->weapon_attempts > 0)
		return; //Already waiting to change the zoom, otherwise it
				//first change to desired zoom and then usual zoomin -M

	ent->client->weapon_attempts++;
	if (ent->client->weapon_attempts == 1)
		Cmd_Weapon_f(ent);
}

//TempFile END

// function to change the firing mode of weapons (when appropriate) 

void Cmd_Weapon_f(edict_t * ent)
{
	int dead;

	dead = (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD);

	ent->client->weapon_attempts--;
	if (ent->client->weapon_attempts < 0)
		ent->client->weapon_attempts = 0;

	if (ent->client->bandaging || ent->client->bandage_stopped) {
		if (!(ent->client->resp.weapon_after_bandage_warned)) {
			ent->client->resp.weapon_after_bandage_warned = true;
			gi.cprintf(ent, PRINT_HIGH, "You'll get to your weapon when you're done bandaging!\n");
		}
		ent->client->weapon_attempts++;
		return;
	}

	ent->client->resp.weapon_after_bandage_warned = false;

	if (ent->client->weaponstate == WEAPON_FIRING || ent->client->weaponstate == WEAPON_BUSY)
	{
		//gi.cprintf(ent, PRINT_HIGH, "Try again when you aren't using your weapon.\n");
		ent->client->weapon_attempts++;
		return;
	}

	switch(ent->client->curr_weap) {
	case MK23_NUM:
		if (!dead)
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);
		ent->client->resp.mk23_mode = !(ent->client->resp.mk23_mode);
		if (ent->client->resp.mk23_mode)
			gi.cprintf(ent, PRINT_HIGH, "MK23 Pistol set for semi-automatic action\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "MK23 Pistol set for automatic action\n");
		break;
	case MP5_NUM:
		if (!dead)
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);
		ent->client->resp.mp5_mode = !(ent->client->resp.mp5_mode);
		if (ent->client->resp.mp5_mode)
			gi.cprintf(ent, PRINT_HIGH, "MP5 set to 3 Round Burst mode\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "MP5 set to Full Automatic mode\n");
		break;
	case M4_NUM:
		if (!dead)
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);
		ent->client->resp.m4_mode = !(ent->client->resp.m4_mode);
		if (ent->client->resp.m4_mode)
			gi.cprintf(ent, PRINT_HIGH, "M4 set to 3 Round Burst mode\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "M4 set to Full Automatic mode\n");
		break;
	case SNIPER_NUM:
		if (dead)
			return;
		
		if (!ent->client->desired_zoom)
			_ZoomIn(ent, true);	// standard behaviour

		_SetSniper(ent, ent->client->desired_zoom);
		ent->client->desired_zoom = 0;
		break;
	case HC_NUM:
		// AQ2:TNG Deathwatch - Single Barreled HC
		if(!hc_single->value)
			return;

		if (!dead)
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);

		ent->client->resp.hc_mode = !(ent->client->resp.hc_mode);
		if (ent->client->resp.hc_mode)
			gi.cprintf(ent, PRINT_HIGH, "Single Barreled Handcannon\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "Double Barreled Handcannon\n");
		// AQ2:TNG End
		break;
	case KNIFE_NUM:
		if (dead)
			return;
		if (ent->client->weaponstate == WEAPON_READY) {
			ent->client->resp.knife_mode = !(ent->client->resp.knife_mode);
			ent->client->weaponstate = WEAPON_ACTIVATING;
			if (ent->client->resp.knife_mode) {
				gi.cprintf(ent, PRINT_HIGH, "Switching to throwing\n");
				ent->client->ps.gunframe = 0;
			} else {
				gi.cprintf(ent, PRINT_HIGH, "Switching to slashing\n");
				ent->client->ps.gunframe = 106;
			}
		}
		break;
	case GRENADE_NUM:
		if (ent->client->resp.grenade_mode == 0) {
			gi.cprintf(ent, PRINT_HIGH, "Prepared to make a medium range throw\n");
			ent->client->resp.grenade_mode = 1;
		} else if (ent->client->resp.grenade_mode == 1) {
			gi.cprintf(ent, PRINT_HIGH, "Prepared to make a long range throw\n");
			ent->client->resp.grenade_mode = 2;
		} else {
			gi.cprintf(ent, PRINT_HIGH, "Prepared to make a short range throw\n");
			ent->client->resp.grenade_mode = 0;
		}
		break;
	}

}

// sets variable to toggle nearby door status
void Cmd_OpenDoor_f(edict_t * ent)
{
	ent->client->doortoggle = 1;
	return;
}

void Cmd_Bandage_f(edict_t * ent)
{

	if ((ent->client->bleeding != 0 || ent->client->leg_damage != 0)
	    && ent->client->bandaging != 1)
		ent->client->reload_attempts = 0;	// prevent any further reloading

	if ((ent->client->weaponstate == WEAPON_READY || ent->client->weaponstate == WEAPON_END_MAG)
	    && (ent->client->bleeding != 0 || ent->client->leg_damage != 0)
	    && ent->client->bandaging != 1) {

		// zucc - check if they have a primed grenade

		if (ent->client->curr_weap == GRENADE_NUM
		    && ((ent->client->ps.gunframe >= GRENADE_IDLE_FIRST
			 && ent->client->ps.gunframe <= GRENADE_IDLE_LAST)
			|| (ent->client->ps.gunframe >= GRENADE_THROW_FIRST
			    && ent->client->ps.gunframe <= GRENADE_THROW_LAST)))
		{
			int damage;

			ent->client->ps.gunframe = 0;

			if (use_classic->value)
				damage = 170;
			else
				damage = GRENADE_DAMRAD;
			
			if(ent->client->quad_framenum > level.framenum)
				damage *= 1.5f;

			fire_grenade2(ent, ent->s.origin, vec3_origin, damage, 0, 2, damage * 2, false);

			INV_AMMO(ent, GRENADE_NUM)--;
			if (INV_AMMO(ent, GRENADE_NUM) <= 0) {
				ent->client->newweapon = GET_ITEM(MK23_NUM);
			}
		}

		ent->client->bandaging = 1;
		ent->client->resp.sniper_mode = SNIPER_1X;
		ent->client->ps.fov = 90;
		ent->client->desired_fov = 90;
		if (ent->client->pers.weapon)
			ent->client->ps.gunindex = gi.modelindex(ent->client->pers.weapon->view_model);
		gi.cprintf(ent, PRINT_HIGH, "You've started bandaging\n");

	} else if (ent->client->bandaging == 1)
		gi.cprintf(ent, PRINT_HIGH, "Already bandaging\n");
	//FIREBLADE 12/26/98 - fix inappropriate message
	else if (ent->client->bleeding == 0 && ent->client->leg_damage == 0)
		gi.cprintf(ent, PRINT_HIGH, "No need to bandage\n");
	else
		gi.cprintf(ent, PRINT_HIGH, "Can't bandage now\n");
	//FIREBLADE
}

// function called in generic_weapon function that does the bandaging

void Bandage(edict_t * ent)
{
	ent->client->leg_noise = 0;
	ent->client->leg_damage = 0;
	ent->client->leghits = 0;
	ent->client->bleeding = 0;
	ent->client->bleed_remain = 0;
	ent->client->bandaging = 0;
	ent->client->leg_dam_count = 0;
	ent->client->attacker = NULL;
	ent->client->bandage_stopped = 1;
	ent->client->idle_weapon = BANDAGE_TIME;
}

void Cmd_ID_f(edict_t * ent)
{
	if (!ent->client->resp.id) {
		gi.cprintf(ent, PRINT_HIGH, "Disabling player identification display.\n");
		ent->client->resp.id = 1;
	} else {
		gi.cprintf(ent, PRINT_HIGH, "Activating player identification display.\n");
		ent->client->resp.id = 0;
	}
	return;
}

static void loc_buildboxpoints(vec3_t p[8], vec3_t org, vec3_t mins, vec3_t maxs)
{
	VectorAdd(org, mins, p[0]);
	VectorCopy(p[0], p[1]);
	p[1][0] -= mins[0];
	VectorCopy(p[0], p[2]);
	p[2][1] -= mins[1];
	VectorCopy(p[0], p[3]);
	p[3][0] -= mins[0];
	p[3][1] -= mins[1];
	VectorAdd(org, maxs, p[4]);
	VectorCopy(p[4], p[5]);
	p[5][0] -= maxs[0];
	VectorCopy(p[0], p[6]);
	p[6][1] -= maxs[1];
	VectorCopy(p[0], p[7]);
	p[7][0] -= maxs[0];
	p[7][1] -= maxs[1];
}

qboolean loc_CanSee(edict_t * targ, edict_t * inflictor)
{
	trace_t trace;
	vec3_t targpoints[8];
	int i;
	vec3_t viewpoint;

// bmodels need special checking because their origin is 0,0,0
	if (targ->movetype == MOVETYPE_PUSH)
		return false;	// bmodels not supported

	loc_buildboxpoints(targpoints, targ->s.origin, targ->mins, targ->maxs);

	VectorCopy(inflictor->s.origin, viewpoint);
	viewpoint[2] += inflictor->viewheight;

	for (i = 0; i < 8; i++) {
		PRETRACE();
		trace = gi.trace(viewpoint, vec3_origin, vec3_origin, targpoints[i], inflictor, MASK_SOLID);
		POSTTRACE();
		if (trace.fraction == 1.0)
			return true;
	}

	return false;
}

// originally from Zoid's CTF
void SetIDView(edict_t * ent)
{
	vec3_t forward, dir;
	trace_t tr;
	edict_t *who, *best;
//FIREBLADE, suggested by hal[9k]  3/11/1999
	float bd = 0.9f;
	float d;
    int i;

	ent->client->ps.stats[STAT_ID_VIEW] = 0;

//FIREBLADE
	if (ent->solid != SOLID_NOT && !teamplay->value) {
		if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
			return;	// won't ever work in non-teams so don't run the code...

	}

	if (ent->client->chase_mode) {
		if (ent->client->chase_target && ent->client->chase_target->inuse) {
			ent->client->ps.stats[STAT_ID_VIEW] =
			    CS_PLAYERSKINS + (ent->client->chase_target - g_edicts - 1);
		}
		return;
	}
//FIREBLADE

	if (ent->client->resp.id == 1)
		return;

	AngleVectors(ent->client->v_angle, forward, NULL, NULL);
	VectorScale(forward, 8192, forward);
	VectorAdd(ent->s.origin, forward, forward);
	PRETRACE();
	tr = gi.trace(ent->s.origin, NULL, NULL, forward, ent, MASK_SOLID);
	POSTTRACE();
	if (tr.fraction < 1 && tr.ent && tr.ent->client) {
		ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS + (ent - g_edicts - 1);
		return;
	}

	AngleVectors(ent->client->v_angle, forward, NULL, NULL);
	best = NULL;
	for (i = 1; i <= maxclients->value; i++) {
		who = g_edicts + i;
		if (!who->inuse)
			continue;
		VectorSubtract(who->s.origin, ent->s.origin, dir);
		VectorNormalize(dir);
		d = DotProduct(forward, dir);
		if (d > bd && loc_CanSee(ent, who) &&
//FIREBLADE
		    (who->solid != SOLID_NOT || who->deadflag == DEAD_DEAD) &&
		    (ent->solid == SOLID_NOT || OnSameTeam(ent, who))) {
//FIREBLADE
			bd = d;
			best = who;
		}
	}
	if (best != NULL && bd > 0.90) {
		ent->client->ps.stats[STAT_ID_VIEW] = CS_PLAYERSKINS + (best - g_edicts - 1);
	}
}

void Cmd_IR_f(edict_t * ent)
{
	int band = 0;

	if (!ir->value) {
		gi.cprintf(ent, PRINT_HIGH, "IR vision not enabled on this server.\n");
		return;
	}
	if (INV_AMMO(ent, BAND_NUM))
		band = 1;

	if (ent->client->resp.ir == 0)
	{
		ent->client->resp.ir = 1;
		if (band)
			gi.cprintf(ent, PRINT_HIGH, "IR vision disabled.\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "IR vision will be disabled when you get a bandolier.\n");
	}
	else
	{
		ent->client->resp.ir = 0;
		if (band)
			gi.cprintf(ent, PRINT_HIGH, "IR vision enabled.\n");
		else
			gi.cprintf(ent, PRINT_HIGH, "IR vision will be enabled when you get a bandolier.\n");
	}
}

// zucc choose command, avoids using the menus in teamplay

void Cmd_Choose_f(edict_t * ent)
{
	char *s;
	int itemNum = NO_NUM;

	// only works in teamplay
	if (!teamplay->value || teamdm->value || ctf->value == 2)
		return;

	s = gi.args();

	// convert names a player might try (DW added a few)
	if (!Q_stricmp(s, "A 2nd pistol") || !Q_stricmp(s, "railgun") || !Q_stricmp(s, "akimbo") || !Q_stricmp(s, DUAL_NAME))
		itemNum = DUAL_NUM;
	else if (!Q_stricmp(s, "shotgun") || !Q_stricmp(s, M3_NAME))
		itemNum = M3_NUM;
	else if (!Q_stricmp(s, "machinegun") || !Q_stricmp(s, HC_NAME))
		itemNum = HC_NUM;
	else if (!Q_stricmp(s, "super shotgun") || !Q_stricmp(s, "mp5") || !Q_stricmp(s, MP5_NAME))
		itemNum = MP5_NUM;
	else if (!Q_stricmp(s, "chaingun") || !Q_stricmp(s, "sniper") || !Q_stricmp(s, SNIPER_NAME))
		itemNum = SNIPER_NUM;
	else if (!Q_stricmp(s, "bfg10k") || !Q_stricmp(s, KNIFE_NAME))
		itemNum = KNIFE_NUM;
	else if (!Q_stricmp(s, "grenade launcher") || !Q_stricmp(s, "m4") || !Q_stricmp(s, M4_NAME))
		itemNum = M4_NUM;
	else if (!Q_stricmp(s, "laser") || !Q_stricmp(s, LASER_NAME))
		itemNum = LASER_NUM;
	else if (!Q_stricmp(s, "vest") || !Q_stricmp(s, KEV_NAME))
		itemNum = KEV_NUM;
	else if (!Q_stricmp(s, "slippers") || !Q_stricmp(s, SLIP_NAME))
		itemNum = SLIP_NUM;
	else if (!Q_stricmp(s, SIL_NAME))
		itemNum = SIL_NUM;
	else if (!Q_stricmp(s, "helmet") || !Q_stricmp(s, HELM_NAME))
		itemNum = HELM_NUM;
	else if (!Q_stricmp(s, BAND_NAME))
		itemNum = BAND_NUM;


	switch(itemNum) {
	case DUAL_NUM:
	case M3_NUM:
	case HC_NUM:
	case MP5_NUM:
	case SNIPER_NUM:
	case KNIFE_NUM:
	case M4_NUM:
		if (!((int)wp_flags->value & items[itemNum].flag)) {
			gi.cprintf(ent, PRINT_HIGH, "Weapon disabled on this server.\n");
			return;
		}
		ent->client->resp.weapon = GET_ITEM(itemNum);
		break;
	case LASER_NUM:
	case KEV_NUM:
	case SLIP_NUM:
	case SIL_NUM:
	case HELM_NUM:
	case BAND_NUM:
		if (!((int)itm_flags->value & items[itemNum].flag)) {
			gi.cprintf(ent, PRINT_HIGH, "Item disabled on this server.\n");
			return;
		}
		ent->client->resp.item = GET_ITEM(itemNum);
		break;
	default:
		gi.cprintf(ent, PRINT_HIGH, "Invalid weapon or item choice.\n");
		return;
	}

	gi.cprintf(ent, PRINT_HIGH, "Weapon selected: %s\nItem selected: %s\n",
		   (ent->client->resp.weapon)->pickup_name, (ent->client->resp.item)->pickup_name);
}

// AQ:TNG - JBravo adding tkok
void Cmd_TKOk(edict_t * ent)
{
	if (!ent->enemy || !ent->enemy->inuse || !ent->enemy->client || (ent == ent->enemy)) {
		gi.cprintf(ent, PRINT_HIGH, "Nothing to forgive\n");
	} else if (ent->client->resp.team == ent->enemy->client->resp.team) {
		if (ent->enemy->client->team_kills) {
			gi.cprintf(ent, PRINT_HIGH, "You forgave %s\n", ent->enemy->client->pers.netname);
			gi.cprintf(ent->enemy, PRINT_HIGH, "%s forgave you\n", ent->client->pers.netname);
			ent->enemy->client->team_kills--;
			if (ent->enemy->client->team_wounds)
				ent->enemy->client->team_wounds /= 2;
		}
	} else {
		gi.cprintf(ent, PRINT_HIGH, "That's very noble of you...\n");
		gi.bprintf(PRINT_HIGH, "%s turned the other cheek\n", ent->client->pers.netname);
	}
	ent->enemy = NULL;
	return;
}

void Cmd_Time(edict_t * ent)
{
	int mins, secs, remaining, rmins, rsecs;
	float gametime;

	if (!timelimit->value) {
		gi.cprintf(ent, PRINT_HIGH, "Timelimit disabled\n");
		return;
	}

	if (matchmode->value)
		gametime = matchtime;
	else
		gametime = level.time;

	mins = gametime / 60;
	secs = gametime - (mins * 60);
	remaining = (timelimit->value * 60) - gametime;
	rmins = remaining / 60;
	rsecs = remaining - (rmins * 60);

	if (rmins < 0)
		rmins = 0;
	if (rsecs < 0)
		rsecs = 0;

	gi.cprintf(ent, PRINT_HIGH, "Elapsed time: %d:%02d. Remaining time: %d:%02d\n", mins, secs, rmins, rsecs);

}

void Cmd_Roundtimeleft_f(edict_t * ent)
{
	int remaining;

	if(!teamplay->value) {
		gi.cprintf(ent, PRINT_HIGH, "This command need teamplay to be enabled\n");
		return;
	}

	if (ctf->value || teamdm->value || team_round_going != 1)
		return;

	if ((int)roundtimelimit->value <= 0)
		return;

	remaining = (roundtimelimit->value * 60) - (current_round_length/10);
	gi.cprintf(ent, PRINT_HIGH, "There is %d:%02i left in this round\n", remaining / 60, remaining % 60);
}

/*
	Freud's AutoRecord
	This will automatically record a new demo for every map if the client turns it on
*/
void RemoveSpaces(char *s)
{
	char *p;

	p = strchr(s, ' ');
	if(!p)
		return;

	for(s = p+1; *s; s++)
	{
		if(*s != ' ')
			*p++ = *s;
	}
	*p = 0;
}

void Cmd_AutoRecord_f(edict_t * ent)
{
	char rec_date[20], recstr[MAX_QPATH];
	time_t clock;

	time( &clock );
	strftime( rec_date, sizeof(rec_date)-1, "%Y_%b_%d_%H%M", localtime(&clock));

	if (matchmode->value)
	{
		if(use_3teams->value)
			Com_sprintf(recstr, sizeof(recstr), "%s-%s_vs_%s_vs_%s-%s", rec_date, teams[TEAM1].name, teams[TEAM2].name, teams[TEAM3].name, level.mapname);
		else
			Com_sprintf(recstr, sizeof(recstr), "%s-%s_vs_%s-%s", rec_date, teams[TEAM1].name, teams[TEAM2].name, level.mapname);

		RemoveSpaces(recstr); //Remove spaces -M
	} else {
		Com_sprintf(recstr, sizeof(recstr), "%s-%s", rec_date, level.mapname);
	}

	stuffcmd(ent, va("record \"%s\"\n", recstr));
}

/*
TNG:Freud
Cmd_Ghost_f

Person gets frags/kills/damage/weapon/item/team/stats back if he disconnected
*/
void Cmd_Ghost_f(edict_t * ent)
{
	int x, frames_since;
	qboolean found = false;

	if (!use_ghosts->value) {
		gi.cprintf(ent, PRINT_HIGH, "Ghosting is not enabled on this server\n");
		return;
	}

	if (num_ghost_players == 0) {
		gi.cprintf(ent, PRINT_HIGH, "No ghost match found\n");
		return;
	}

	for (x = 0; x < num_ghost_players; x++) {
		if (found == true) {
			ghost_players[x - 1] = ghost_players[x];
		} else if (strcmp(ghost_players[x].ipaddr, ent->client->ipaddr) == 0 &&
			   strcmp(ghost_players[x].netname, ent->client->pers.netname) == 0) {
			found = true;
			gi.cprintf(ent, PRINT_HIGH, "Welcome back %s\n", ent->client->pers.netname);
			frames_since = level.framenum - ghost_players[x].disconnect_frame;
			ent->client->resp.enterframe = ghost_players[x].enterframe + frames_since;
			ent->client->resp.score = ghost_players[x].score;
			ent->client->resp.kills = ghost_players[x].kills;
			ent->client->resp.damage_dealt = ghost_players[x].damage_dealt;
			if (teamplay->value) {
				if (ghost_players[x].team && ghost_players[x].team != NOTEAM)
					JoinTeam(ent, ghost_players[x].team, 1);
				ent->client->resp.weapon = ghost_players[x].weapon;
				ent->client->resp.item = ghost_players[x].item;
			}

			ent->client->resp.stats_shots_t = ghost_players[x].stats_shots_t;
			ent->client->resp.stats_shots_h = ghost_players[x].stats_shots_h;

			memcpy(ent->client->resp.stats_locations, ghost_players[x].stats_locations,
			       sizeof(ghost_players[x].stats_locations));
			memcpy(ent->client->resp.stats_shots, ghost_players[x].stats_shots,
			       sizeof(ghost_players[x].stats_shots));
			memcpy(ent->client->resp.stats_hits, ghost_players[x].stats_hits,
			       sizeof(ghost_players[x].stats_hits));
			memcpy(ent->client->resp.stats_headshot, ghost_players[x].stats_headshot,
			       sizeof(ghost_players[x].stats_headshot));
		}
	}
	if (found == true) {
		num_ghost_players--;
	} else {
		gi.cprintf(ent, PRINT_HIGH, "No ghost match found\n");
	}
}
