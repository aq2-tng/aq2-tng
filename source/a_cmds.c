//-----------------------------------------------------------------------------
// zucc
// File for our new commands.
//
// laser sight patch, by Geza Beladi
//
// $Id: a_cmds.c,v 1.6 2001/07/16 19:02:06 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_cmds.c,v $
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



extern void P_ProjectSource (gclient_t * client, vec3_t point, vec3_t distance, vec3_t forward, vec3_t right, vec3_t result);


/*----------------------------------------
SP_LaserSight

  Create/remove the laser sight entity
-----------------------------------------*/

#define lss self->lasersight

void
SP_LaserSight (edict_t * self, gitem_t * item)
{

  vec3_t start, forward, right, end;
  int laser_on = 0;
  gitem_t *temp_item;



//      gi.cprintf(self, PRINT_HIGH, "Calling lasersight function have_laser %d.\n", self->client->have_laser);

  temp_item = FindItem (LASER_NAME);
  if (!(self->client->pers.inventory[ITEM_INDEX (temp_item)]))
    {
      if (lss)			// laser is on

	{
	  G_FreeEdict (lss);
	  lss = NULL;
	}
//              gi.cprintf(self, PRINT_HIGH, "didn't have laser sight.\n");             
      return;
    }

  // zucc code to make it be used with the right weapons


//      gi.cprintf(self, PRINT_HIGH, "curr_weap = %d.\n", self->client->curr_weap);             

  switch (self->client->curr_weap)
    {
    case MK23_NUM:
    case MP5_NUM:
    case M4_NUM:
      {
	laser_on = 1;
	break;
      }
    default:
      {
	laser_on = 0;
	break;
      }
    }

//      gi.cprintf(self, PRINT_HIGH, "laser_on is %d.\n", laser_on);            

  // laser is on but we want it off
  if (lss && !laser_on)
    {
//              gi.cprintf(self, PRINT_HIGH, "trying to free the laser sight\n");               
      G_FreeEdict (lss);
      lss = NULL;
      //gi.bprintf (PRINT_HIGH, "lasersight off.");
      return;
    }

//      gi.cprintf(self, PRINT_HIGH, "laser wasn't lss is %p.\n", lss);         

  // off and we want it to stay that way
  if (!laser_on)
    return;

  //gi.bprintf (PRINT_HIGH, "lasersight on.");

  AngleVectors (self->client->v_angle, forward, right, NULL);

  VectorSet (end, 100, 0, 0);
  G_ProjectSource (self->s.origin, end, forward, right, start);

  lss = G_Spawn ();
  lss->owner = self;
  lss->movetype = MOVETYPE_NOCLIP;
  lss->solid = SOLID_NOT;
  lss->classname = "lasersight";
  lss->s.modelindex = gi.modelindex ("sprites/lsight.sp2");
  lss->s.renderfx = RF_TRANSLUCENT;
  lss->think = LaserSightThink;
  lss->nextthink = level.time + 0.01;
}


/*---------------------------------------------
LaserSightThink

  Updates the sights position, angle, and shape
  is the lasersight entity
---------------------------------------------*/

void
LaserSightThink (edict_t * self)
{
  vec3_t start, end, endp, offset;
  vec3_t forward, right, up;
  vec3_t angles;
  trace_t tr;
  int height = 0;

  // zucc compensate for weapon ride up
  VectorAdd (self->owner->client->v_angle, self->owner->client->kick_angles, angles);
  AngleVectors ( /*self->owner->client->v_angle */ angles, forward, right, up);


  if (self->owner->lasersight != self)
    {
      self->think = G_FreeEdict;
    }


  if (self->owner->client->pers.firing_style == ACTION_FIRING_CLASSIC)
    height = 8;


  VectorSet (offset, 24, 8, self->owner->viewheight - height);

  P_ProjectSource (self->owner->client, self->owner->s.origin, offset, forward, right, start);
  VectorMA (start, 8192, forward, end);

  PRETRACE ();
  tr = gi.trace (start, NULL, NULL, end, self->owner, CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER);
  POSTTRACE ();

  if (tr.fraction != 1)
    {
      VectorMA (tr.endpos, -4, forward, endp);
      VectorCopy (endp, tr.endpos);
    }

  vectoangles (tr.plane.normal, self->s.angles);
  VectorCopy (tr.endpos, self->s.origin);

  gi.linkentity (self);
  self->nextthink = level.time + 0.1;
}


void
Cmd_New_Reload_f (edict_t * ent)
{
//FB 6/1/99 - refuse to reload during LCA
  if ((int) teamplay->value && lights_camera_action)
    return;
//FB 6/1/99

  ent->client->reload_attempts++;
}


//+BD ENTIRE CODE BLOCK NEW
// Cmd_Reload_f()
// Handles weapon reload requests
void
Cmd_Reload_f (edict_t * ent)
{
//      int rds_left;           //+BD - Variable to handle rounds left


  //+BD - If the player is dead, don't bother
  if (ent->deadflag == DEAD_DEAD)
    {
      //gi.centerprintf(ent, "I know you're a hard ass,\nBUT YOU'RE FUCKING DEAD!!\n");
      return;
    }

  if (ent->client->weaponstate == WEAPON_BANDAGING
      || ent->client->bandaging == 1
      || ent->client->bandage_stopped == 1
      || ent->client->weaponstate == WEAPON_ACTIVATING
      || ent->client->weaponstate == WEAPON_DROPPING
      || ent->client->weaponstate == WEAPON_FIRING)
    {
      return;
    }

  if (!ent->client->fast_reload)
    ent->client->reload_attempts--;
  if (ent->client->reload_attempts < 0)
    ent->client->reload_attempts = 0;

  //First, grab the current magazine max count...
  if ((ent->client->curr_weap == MK23_NUM)
      || (ent->client->curr_weap == MP5_NUM)
      || (ent->client->curr_weap == M4_NUM)
      || (ent->client->curr_weap == M3_NUM)
      || (ent->client->curr_weap == HC_NUM)
      || (ent->client->curr_weap == SNIPER_NUM)
      || (ent->client->curr_weap == DUAL_NUM))
    {
    }
  else				//We should never get here, but...
    //BD 5/26 - Actually we get here quite often right now. Just exit for weaps that we
    //          don't want reloaded or that never reload (grenades)

    {
      //gi.centerprintf(ent,"Where'd you train?\nYou can't reload that!\n");
      return;
    }

  if (ent->client->pers.inventory[ent->client->ammo_index])
    {
      //Set the weaponstate...
      if (ent->client->curr_weap == M3_NUM)
	{
	  if (ent->client->shot_rds >= ent->client->shot_max)
	    {
	      return;
	    }
	  // already in the process of reloading!
	  if (ent->client->weaponstate == WEAPON_RELOADING && (ent->client->shot_rds < (ent->client->shot_max - 1)) && !(ent->client->fast_reload) && ((ent->client->pers.inventory[ent->client->ammo_index] - 1) > 0))
	    {
	      // don't let them start fast reloading until far enough into the firing sequence
	      // this gives them a chance to break off from reloading to fire the weapon - zucc
	      if (ent->client->ps.gunframe >= 48)
		{
		  ent->client->fast_reload = 1;
		  (ent->client->pers.inventory[ent->client->ammo_index])--;
		}
	      else
		{
		  ent->client->reload_attempts++;

		}
	    }
	}
      if (ent->client->curr_weap == HC_NUM)
	{
	  if (ent->client->cannon_rds >= ent->client->cannon_max)
	    {
	      return;
	    }

	  if (!(ent->client->pers.inventory[ent->client->ammo_index] >= 2))
	    return;
	}
      if (ent->client->curr_weap == SNIPER_NUM)
	{
	  if (ent->client->sniper_rds >= ent->client->sniper_max)
	    {
	      return;
	    }
	  // already in the process of reloading!
	  if (ent->client->weaponstate == WEAPON_RELOADING && (ent->client->sniper_rds < (ent->client->sniper_max - 1)) && !(ent->client->fast_reload) && ((ent->client->pers.inventory[ent->client->ammo_index] - 1) > 0))
	    {
	      // don't let them start fast reloading until far enough into the firing sequence
	      // this gives them a chance to break off from reloading to fire the weapon - zucc
	      if (ent->client->ps.gunframe >= 72)
		{
		  ent->client->fast_reload = 1;
		  (ent->client->pers.inventory[ent->client->ammo_index])--;
		}
	      else
		{
		  ent->client->reload_attempts++;
		}
	    }
	  ent->client->ps.fov = 90;
	  if (ent->client->pers.weapon)
	    ent->client->ps.gunindex = gi.modelindex (ent->client->pers.weapon->view_model);
	}
      if (ent->client->curr_weap == DUAL_NUM)
	{
	  //TempFile change to pistol, then reload
	  if (ent->client->pers.inventory[ent->client->ammo_index] == 1)
	    {
	      gitem_t *it;
	      it = FindItem (MK23_NAME);
	      it->use (ent, it);
	      ent->client->autoreloading = true;
	      return;
	    }
	  //TempFile
	  if (!(ent->client->pers.inventory[ent->client->ammo_index] >= 2))
	    return;
//FIREBLADE 7/11/1999 - stop reloading when weapon already full
	  if (ent->client->dual_rds == ent->client->dual_max)
	    return;
	}
      if (ent->client->curr_weap == MP5_NUM)
	{
	  if (ent->client->mp5_rds == ent->client->mp5_max)
	    return;
	}
      if (ent->client->curr_weap == M4_NUM)
	{
	  if (ent->client->m4_rds == ent->client->m4_max)
	    return;
	}
      if (ent->client->curr_weap == MK23_NUM)
	{
	  if (ent->client->mk23_rds == ent->client->mk23_max)
	    return;
	}
//FIREBLADE

      ent->client->weaponstate = WEAPON_RELOADING;
      //(ent->client->pers.inventory[ent->client->ammo_index])--;
    }
  else
    gi.cprintf (ent, PRINT_HIGH, "Out of ammo\n");
  //gi.centerprintf(ent,"Pull your head out-\nYou've got NO AMMO!\n");
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
void
_SetSniper (edict_t * ent, int zoom)
{
// AQ:TNG - JBravo fixing compilerwarnings.
  int desired_fov = SNIPER_FOV1;
  int sniper_mode = SNIPER_1X;
// JBravo.
  int oldmode;

  if ((zoom != 1) && (zoom != 2) && (zoom != 4) && (zoom != 6) && (ent->client->curr_weap != SNIPER_NUM))
    return;

  gi.sound (ent, CHAN_ITEM, gi.soundindex ("misc/lensflik.wav"), 1, ATTN_NORM, 0);

  oldmode = ent->client->resp.sniper_mode;

  switch (zoom)
    {
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

  if (sniper_mode == oldmode)
    return;

  ent->client->resp.sniper_mode = sniper_mode;
  ent->client->desired_fov = desired_fov;

  if (sniper_mode == SNIPER_1X && ent->client->pers.weapon)
    ent->client->ps.gunindex = gi.modelindex (ent->client->pers.weapon->view_model);
  //show the model if switching to 1x

  if (oldmode == SNIPER_1X)
    {
      //do idleness stuff when switching from 1x, see function below
      ent->client->weaponstate = WEAPON_BUSY;
      ent->client->idle_weapon = 6;
      ent->client->ps.gunframe = 22;
    }
}
//tempfile END

void
Cmd_New_Weapon_f (edict_t * ent)
{
  ent->client->weapon_attempts++;
  if (ent->client->weapon_attempts == 1)
    Cmd_Weapon_f (ent);
}

//TempFile BEGIN

void
_ZoomIn (edict_t * ent, qboolean overflow)
{
  switch (ent->client->resp.sniper_mode)
    {
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
      else
	return;			// no more zooming can be done

      break;
    default:
      return;
    }

  ent->client->weapon_attempts++;
  if (ent->client->weapon_attempts == 1)
    Cmd_Weapon_f (ent);
}

void
_ZoomOut (edict_t * ent, qboolean overflow)
{
  switch (ent->client->resp.sniper_mode)
    {
    case SNIPER_1X:
      if (overflow)
	ent->client->desired_zoom = 6;
      else
	return;
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
    default:
      return;
    }

  ent->client->weapon_attempts++;
  if (ent->client->weapon_attempts == 1)
    Cmd_Weapon_f (ent);
}

void
Cmd_Lens_f (edict_t * ent)
{
  int nArg = atoi (gi.args ());
  char args[16];
  strcpy (args, gi.args ());

  if (ent->client->curr_weap != SNIPER_NUM)
    return;

  if (nArg == 0)
    {
      //perhaps in or out? let's see.
      if (Q_stricmp (args, "in") == 0)
	_ZoomIn (ent, false);
      else if (Q_stricmp (args, "out") == 0)
	_ZoomOut (ent, false);
      else
	_ZoomIn (ent, true);

      return;			// _ZoomX will call Cmd_Weapon_f

    }

  if (nArg == 0)
    return;
  else if ((nArg == 1) || (!(nArg % 2) && (nArg <= 6)))
    ent->client->desired_zoom = nArg;

  ent->client->weapon_attempts++;
  if (ent->client->weapon_attempts == 1)
    Cmd_Weapon_f (ent);
}

//TempFile END

// function to change the firing mode of weapons (when appropriate) 

void
Cmd_Weapon_f (edict_t * ent)
{
  int dead;

  dead = (ent->solid == SOLID_NOT || ent->deadflag == DEAD_DEAD);

  ent->client->weapon_attempts--;
  if (ent->client->weapon_attempts < 0)
    ent->client->weapon_attempts = 0;

  if (ent->client->bandaging || ent->client->bandage_stopped)
    {
      if (!(ent->client->resp.weapon_after_bandage_warned))
	{
	  ent->client->resp.weapon_after_bandage_warned = true;
	  gi.cprintf (ent, PRINT_HIGH, "You'll get to your weapon when you're done bandaging!\n");
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

  if (ent->client->curr_weap == MK23_NUM)
    {
      if (!dead)
	gi.sound (ent, CHAN_ITEM, gi.soundindex ("misc/click.wav"), 1, ATTN_NORM, 0);
      ent->client->resp.mk23_mode = !(ent->client->resp.mk23_mode);
      if (ent->client->resp.mk23_mode)
	gi.cprintf (ent, PRINT_HIGH, "MK23 Pistol set for semi-automatic action\n");
      else
	gi.cprintf (ent, PRINT_HIGH, "MK23 Pistol set for automatic action\n");
    }
  if (ent->client->curr_weap == MP5_NUM)
    {
      if (!dead)
	gi.sound (ent, CHAN_ITEM, gi.soundindex ("misc/click.wav"), 1, ATTN_NORM, 0);
      ent->client->resp.mp5_mode = !(ent->client->resp.mp5_mode);
      if (ent->client->resp.mp5_mode)
	gi.cprintf (ent, PRINT_HIGH, "MP5 set to 3 Round Burst mode\n");
      else
	gi.cprintf (ent, PRINT_HIGH, "MP5 set to Full Automatic mode\n");
    }
  if (ent->client->curr_weap == M4_NUM)
    {
      if (!dead)
	gi.sound (ent, CHAN_ITEM, gi.soundindex ("misc/click.wav"), 1, ATTN_NORM, 0);
      ent->client->resp.m4_mode = !(ent->client->resp.m4_mode);
      if (ent->client->resp.m4_mode)
	gi.cprintf (ent, PRINT_HIGH, "M4 set to 3 Round Burst mode\n");
      else
	gi.cprintf (ent, PRINT_HIGH, "M4 set to Full Automatic mode\n");
    }

  if (ent->client->curr_weap == SNIPER_NUM)
    {
      if (dead)
	return;
// TempFile - BEGIN
      if (ent->client->desired_zoom)	// we know what we want to do

	{
	  _SetSniper (ent, ent->client->desired_zoom);
	  ent->client->desired_zoom = 0;
	}
      else
	{
	  _ZoomIn (ent, true);	// standard behaviour

	  return;		//we need to return because _ZoomIn will call this again

	}
// TempFile - END

    }

// AQ2:TNG Deathwatch - Single Barreled HC
	if (ent->client->curr_weap == HC_NUM && hc_single->value)
    {
		if (!dead)
			gi.sound(ent, CHAN_ITEM, gi.soundindex("misc/click.wav"), 1, ATTN_NORM, 0);

		ent->client->resp.hc_mode = !(ent->client->resp.hc_mode);
		if ( ent->client->resp.hc_mode )
			gi.cprintf (ent, PRINT_HIGH, "Single Barreled Handcannon\n");
		else
			gi.cprintf (ent, PRINT_HIGH, "Double Barreled Handcannon\n");
	}
// AQ2:TNG End

  if (ent->client->curr_weap == KNIFE_NUM)
    {
      if (dead)
	return;
      if (ent->client->weaponstate == WEAPON_READY)
	{
	  ent->client->resp.knife_mode = !(ent->client->resp.knife_mode);
	  ent->client->weaponstate = WEAPON_ACTIVATING;
	  if (ent->client->resp.knife_mode)
	    {
	      gi.cprintf (ent, PRINT_HIGH, "Switching to throwing\n");
	      ent->client->ps.gunframe = 0;
	    }
	  else
	    {
	      gi.cprintf (ent, PRINT_HIGH, "Switching to slashing\n");
	      ent->client->ps.gunframe = 106;
	    }

	}
    }
  if (ent->client->curr_weap == GRENADE_NUM)
    {
      if (ent->client->resp.grenade_mode == 0)
	{
	  gi.cprintf (ent, PRINT_HIGH, "Prepared to make a medium range throw\n");
	  ent->client->resp.grenade_mode = 1;
	}
      else if (ent->client->resp.grenade_mode == 1)
	{
	  gi.cprintf (ent, PRINT_HIGH, "Prepared to make a long range throw\n");
	  ent->client->resp.grenade_mode = 2;
	}
      else
	{
	  gi.cprintf (ent, PRINT_HIGH, "Prepared to make a short range throw\n");
	  ent->client->resp.grenade_mode = 0;
	}

    }

}


// sets variable to toggle nearby door status
void
Cmd_OpenDoor_f (edict_t * ent)
{

  ent->client->doortoggle = 1;

  return;
}

void
Cmd_Bandage_f (edict_t * ent)
{
  gitem_t *item;


  if ((ent->client->bleeding != 0 || ent->client->leg_damage != 0) && ent->client->bandaging != 1)
    ent->client->reload_attempts = 0;	// prevent any further reloading



  if ((ent->client->weaponstate == WEAPON_READY || ent->client->weaponstate == WEAPON_END_MAG)
      && (ent->client->bleeding != 0 || ent->client->leg_damage != 0)
      && ent->client->bandaging != 1)
    {

      // zucc - check if they have a primed grenade

      if (ent->client->curr_weap == GRENADE_NUM
	  && ((ent->client->ps.gunframe >= GRENADE_IDLE_FIRST
	       && ent->client->ps.gunframe <= GRENADE_IDLE_LAST)
	      || (ent->client->ps.gunframe >= GRENADE_THROW_FIRST
		  && ent->client->ps.gunframe <= GRENADE_THROW_LAST)))
	{
	  ent->client->ps.gunframe = 0;
	  fire_grenade2 (ent, ent->s.origin, tv (0, 0, 0), GRENADE_DAMRAD, 0, 2, GRENADE_DAMRAD * 2, false);
	  item = FindItem (GRENADE_NAME);
	  ent->client->pers.inventory[ITEM_INDEX (item)]--;
	  if (ent->client->pers.inventory[ITEM_INDEX (item)] <= 0)
	    {
	      ent->client->newweapon = FindItem (MK23_NAME);

	    }
	}

      ent->client->bandaging = 1;
      ent->client->resp.sniper_mode = SNIPER_1X;
      ent->client->ps.fov = 90;
      ent->client->desired_fov = 90;
      if (ent->client->pers.weapon)
	ent->client->ps.gunindex = gi.modelindex (ent->client->pers.weapon->view_model);
      gi.cprintf (ent, PRINT_HIGH, "You've started bandaging\n");

    }
  else if (ent->client->bandaging == 1)
    gi.cprintf (ent, PRINT_HIGH, "Already bandaging\n");
  //FIREBLADE 12/26/98 - fix inappropriate message
  else if (ent->client->bleeding == 0 && ent->client->leg_damage == 0)
    gi.cprintf (ent, PRINT_HIGH, "No need to bandage\n");
  else
    gi.cprintf (ent, PRINT_HIGH, "Can't bandage now\n");
  //FIREBLADE
}

// function called in generic_weapon function that does the bandaging

void
Bandage (edict_t * ent)
{
  ent->client->leg_noise = 0;
  ent->client->leg_damage = 0;
  ent->client->leghits = 0;
  ent->client->bleeding = 0;
  ent->client->bleed_remain = 0;
  //        ent->client->bleedcount = 0;
  //        ent->client->bleeddelay = 0;    
  ent->client->bandaging = 0;
  ent->client->leg_dam_count = 0;
  ent->client->attacker = NULL;
  ent->client->bandage_stopped = 1;
  ent->client->idle_weapon = BANDAGE_TIME;
}



void
Cmd_ID_f (edict_t * ent)
{

  if (!ent->client->resp.id)
    {
      gi.cprintf (ent, PRINT_HIGH, "Disabling player identification display.\n");
      ent->client->resp.id = 1;
    }
  else
    {
      gi.cprintf (ent, PRINT_HIGH, "Activating player identification display.\n");
      ent->client->resp.id = 0;
    }
  return;
}


static void
loc_buildboxpoints (vec3_t p[8], vec3_t org, vec3_t mins, vec3_t maxs)
{
  VectorAdd (org, mins, p[0]);
  VectorCopy (p[0], p[1]);
  p[1][0] -= mins[0];
  VectorCopy (p[0], p[2]);
  p[2][1] -= mins[1];
  VectorCopy (p[0], p[3]);
  p[3][0] -= mins[0];
  p[3][1] -= mins[1];
  VectorAdd (org, maxs, p[4]);
  VectorCopy (p[4], p[5]);
  p[5][0] -= maxs[0];
  VectorCopy (p[0], p[6]);
  p[6][1] -= maxs[1];
  VectorCopy (p[0], p[7]);
  p[7][0] -= maxs[0];
  p[7][1] -= maxs[1];
}

qboolean
loc_CanSee (edict_t * targ, edict_t * inflictor)
{
  trace_t trace;
  vec3_t targpoints[8];
  int i;
  vec3_t viewpoint;

// bmodels need special checking because their origin is 0,0,0
  if (targ->movetype == MOVETYPE_PUSH)
    return false;		// bmodels not supported

  loc_buildboxpoints (targpoints, targ->s.origin, targ->mins, targ->maxs);

  VectorCopy (inflictor->s.origin, viewpoint);
  viewpoint[2] += inflictor->viewheight;

  for (i = 0; i < 8; i++)
    {
      PRETRACE ();
      trace = gi.trace (viewpoint, vec3_origin, vec3_origin, targpoints[i], inflictor, MASK_SOLID);
      POSTTRACE ();
      if (trace.fraction == 1.0)
	return true;
    }

  return false;
}


// originally from Zoid's CTF
void
SetIDView (edict_t * ent)
{
  vec3_t forward, dir;
  trace_t tr;
  edict_t *who, *best;
//FIREBLADE, suggested by hal[9k]  3/11/1999
  float bd = 0.9;
//FIREBLADE
  float d;
  int i;

  ent->client->ps.stats[STAT_ID_VIEW] = 0;

//FIREBLADE
  if (ent->solid != SOLID_NOT && !teamplay->value)
    {
      if (!((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
	return;			// won't ever work in non-teams so don't run the code...

    }

  if (ent->client->chase_mode)
    {
      if (ent->client->chase_target &&
	  ent->client->chase_target->inuse)
	{
	  ent->client->ps.stats[STAT_ID_VIEW] =
	    CS_PLAYERSKINS +
	    (ent->client->chase_target - g_edicts - 1);
	}
      return;
    }
//FIREBLADE

  if (ent->client->resp.id == 1)
    return;

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  VectorScale (forward, 8192, forward);
  VectorAdd (ent->s.origin, forward, forward);
  PRETRACE ();
  tr = gi.trace (ent->s.origin, NULL, NULL, forward, ent, MASK_SOLID);
  POSTTRACE ();
  if (tr.fraction < 1 && tr.ent && tr.ent->client)
    {
      ent->client->ps.stats[STAT_ID_VIEW] =
	CS_PLAYERSKINS + (ent - g_edicts - 1);
      return;
    }

  AngleVectors (ent->client->v_angle, forward, NULL, NULL);
  best = NULL;
  for (i = 1; i <= maxclients->value; i++)
    {
      who = g_edicts + i;
      if (!who->inuse)
	continue;
      VectorSubtract (who->s.origin, ent->s.origin, dir);
      VectorNormalize (dir);
      d = DotProduct (forward, dir);
      if (d > bd && loc_CanSee (ent, who) &&
//FIREBLADE
	  (who->solid != SOLID_NOT || who->deadflag == DEAD_DEAD) &&
	  (ent->solid == SOLID_NOT || OnSameTeam (ent, who)))
	{
//FIREBLADE
	  bd = d;
	  best = who;
	}
    }
  if (best != NULL && bd > 0.90)
    {
      ent->client->ps.stats[STAT_ID_VIEW] =
	CS_PLAYERSKINS + (best - g_edicts - 1);
    }
}


void
Cmd_IR_f (edict_t * ent)
{
  int band = 0;
  if (ir->value)
    {
      if (ent->client->pers.inventory[ITEM_INDEX (FindItem (BAND_NAME))])
	{
	  band = 1;
	}
      if (ent->client->resp.ir == 0)
	{
	  ent->client->resp.ir = 1;
	  if (band)
	    gi.cprintf (ent, PRINT_HIGH, "IR vision disabled.\n");
	  else
	    gi.cprintf (ent, PRINT_HIGH, "IR vision will be disabled when you get a bandolier.\n");
	  //      if ( ent->client->ps.rdflags & RDF_IRGOGGLES )
	  //      ent->client->ps.rdflags &= ~RDF_IRGOGGLES;
	}
      else
	{
	  ent->client->resp.ir = 0;
	  if (band)
	    gi.cprintf (ent, PRINT_HIGH, "IR vision enabled.\n");
	  else
	    gi.cprintf (ent, PRINT_HIGH, "IR vision will be enabled when you get a bandolier.\n");
	  //      if ( !(ent->client->ps.rdflags & RDF_IRGOGGLES) )
	  //      ent->client->ps.rdflags |= RDF_IRGOGGLES;
	}

    }
  else
    {
      gi.cprintf (ent, PRINT_HIGH, "IR vision not enabled on this server.\n");
    }
}


// zucc choose command, avoids using the menus in teamplay

void
Cmd_Choose_f (edict_t * ent)
{

  char *s;

  s = gi.args ();

  // only works in teamplay
  if (!teamplay->value)
    return;

  // convert names a player might try
  if (!stricmp (s, "A 2nd pistol") || !stricmp (s, "railgun"))
    s = DUAL_NAME;
  if (!stricmp (s, "shotgun"))
    s = M3_NAME;
  if (!stricmp (s, "machinegun"))
    s = HC_NAME;
  if (!stricmp (s, "super shotgun"))
    s = MP5_NAME;
  if (!stricmp (s, "chaingun"))
    s = SNIPER_NAME;
  if (!stricmp (s, "bfg10k"))
    s = KNIFE_NAME;
  if (!stricmp (s, "grenade launcher"))
    s = M4_NAME;

  if (stricmp (s, MP5_NAME) == 0)
    ent->client->resp.weapon = FindItem (MP5_NAME);
  else if (stricmp (s, M3_NAME) == 0)
    ent->client->resp.weapon = FindItem (M3_NAME);
  else if (stricmp (s, M4_NAME) == 0)
    ent->client->resp.weapon = FindItem (M4_NAME);
  else if (stricmp (s, HC_NAME) == 0)
    ent->client->resp.weapon = FindItem (HC_NAME);
  else if (stricmp (s, SNIPER_NAME) == 0)
    ent->client->resp.weapon = FindItem (SNIPER_NAME);
  else if (stricmp (s, KNIFE_NAME) == 0)
    ent->client->resp.weapon = FindItem (KNIFE_NAME);
  else if (stricmp (s, DUAL_NAME) == 0)
    ent->client->resp.weapon = FindItem (DUAL_NAME);
  else if (stricmp (s, KEV_NAME) == 0)
    ent->client->resp.item = FindItem (KEV_NAME);
  else if (stricmp (s, HELM_NAME) == 0)
    ent->client->resp.item = FindItem (HELM_NAME);
  else if (stricmp (s, LASER_NAME) == 0)
    ent->client->resp.item = FindItem (LASER_NAME);
  else if (stricmp (s, SLIP_NAME) == 0)
    ent->client->resp.item = FindItem (SLIP_NAME);
  else if (stricmp (s, SIL_NAME) == 0)
    ent->client->resp.item = FindItem (SIL_NAME);
  else if (stricmp (s, BAND_NAME) == 0)
    ent->client->resp.item = FindItem (BAND_NAME);
  else
    {
      gi.cprintf (ent, PRINT_HIGH, "Invalid weapon or item choice.\n");
      return;
    }
  gi.cprintf (ent, PRINT_HIGH, "Weapon selected: %s\nItem selected: %s\n", (ent->client->resp.weapon)->pickup_name, (ent->client->resp.item)->pickup_name);

}

// AQ:TNG - JBravo adding tkok
void Cmd_TKOk(edict_t *ent)
{
	if (!ent->enemy || !ent->enemy->inuse || !ent->enemy->client || (ent == ent->enemy)) {
		gi.cprintf(ent, PRINT_HIGH, "Nothing to forgive\n");
	}
	else if (ent->client->resp.team == ent->enemy->client->resp.team) {
		if (ent->enemy->client->team_kills) {
			gi.cprintf(ent, PRINT_HIGH, "You forgave %s\n",
				ent->enemy->client->pers.netname);
			gi.cprintf(ent->enemy, PRINT_HIGH, "%s forgave you\n",
				ent->client->pers.netname);
			ent->enemy->client->team_kills--;
			if (ent->enemy->client->team_wounds)
				ent->enemy->client->team_wounds /= 2;
			ent->enemy = NULL;
		}
	} else {
		gi.cprintf(ent, PRINT_HIGH, "That's very noble of you...\n");
		gi.bprintf(PRINT_HIGH, "%s turned the other cheek\n", ent->client->pers.netname);
	}
	ent->enemy = NULL;
	return;
}
