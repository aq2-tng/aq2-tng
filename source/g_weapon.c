//-----------------------------------------------------------------------------
// g_weapon.c
//
// $Id: g_weapon.c,v 1.15 2004/04/08 23:19:51 slicerdw Exp $
//
//-----------------------------------------------------------------------------
// $Log: g_weapon.c,v $
// Revision 1.15  2004/04/08 23:19:51  slicerdw
// Optimized some code, added a couple of features and fixed minor bugs
//
// Revision 1.14  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.13  2002/02/19 10:28:43  freud
// Added to %D hit in the kevlar vest and kevlar helmet, also body for handcannon
// and shotgun.
//
// Revision 1.12  2002/02/18 18:25:51  ra
// Bumped version to 2.6, fixed ctf falling and kicking of players in ctf
// uvtime
//
// Revision 1.11  2002/02/18 13:55:35  freud
// Added last damaged players %P
//
// Revision 1.10  2002/02/01 17:49:56  freud
// Heavy changes in stats code. Removed lots of variables and replaced them
// with int arrays of MODs. This cleaned tng_stats.c up a whole lots and
// everything looks well, might need more testing.
//
// Revision 1.9  2002/01/24 02:24:56  deathwatch
// Major update to Stats code (thanks to Freud)
// new cvars:
// stats_afterround - will display the stats after a round ends or map ends
// stats_endmap - if on (1) will display the stats scoreboard when the map ends
//
// Revision 1.8  2001/12/23 16:30:50  ra
// 2.5 ready. New stats from Freud. HC and shotgun gibbing seperated.
//
// Revision 1.7  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.6  2001/08/18 01:28:06  deathwatch
// Fixed some stats stuff, added darkmatch + day_cycle, cleaned up several files, restructured ClientCommand
//
// Revision 1.5  2001/08/06 14:38:45  ra
// Adding UVtime for ctf
//
// Revision 1.4  2001/08/06 03:00:49  ra
// Added FF after rounds. Please someone look at the EVIL if statments for me :)
//
// Revision 1.3  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.2.2.2  2001/05/20 18:54:19  igor_rock
// added original ctf code snippets from zoid. lib compilesand runs but
// doesn't function the right way.
// Jsut committing these to have a base to return to if something wents
// awfully wrong.
//
// Revision 1.2.2.1  2001/05/20 15:17:31  igor_rock
// removed the old ctf code completly
//
// Revision 1.2  2001/05/11 16:07:26  mort
// Various CTF bits and pieces...
//
// Revision 1.1.1.1  2001/05/06 17:30:24  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "cgf_sfx_glass.h"
#include "a_game.h"		//zucc for KickDoor


void knife_touch (edict_t * ent, edict_t * other, cplane_t * plane,
		  csurface_t * surf);
void Zylon_Grenade (edict_t * ent);
void setFFState (edict_t * ent);

//FIREBLADE
qboolean ap_already_hit[1000];	// 1000 = a number much larger than the possible max # of clients
int *took_damage;		//FB 6/2/99, to keep track of shotgun damage
//FIREBLADE

/*
=================
check_dodge

This is a support routine used when a client is firing
a non-instant attack weapon.  It checks to see if a
monster's dodge function should be called.
=================
*/
static void
check_dodge (edict_t * self, vec3_t start, vec3_t dir, int speed)
{
  vec3_t end;
  vec3_t v;
  trace_t tr;
  float eta;

  // easy mode only ducks one quarter the time
  if (skill->value == 0)
    {
      if (random () > 0.25)
	return;
    }
  VectorMA (start, 8192, dir, end);
  PRETRACE ();
  tr = gi.trace (start, NULL, NULL, end, self, MASK_SHOT);
  POSTTRACE ();
  if ((tr.ent) && (tr.ent->svflags & SVF_MONSTER) && (tr.ent->health > 0)
      && (tr.ent->monsterinfo.dodge) && infront (tr.ent, self))
    {
      VectorSubtract (tr.endpos, start, v);
      eta = (VectorLength (v) - tr.ent->maxs[0]) / speed;
      tr.ent->monsterinfo.dodge (tr.ent, self, eta);
    }
}


/* zucc - bulletholes for testing spread patterns */

void
BulletHoleThink (edict_t * self)
{
  G_FreeEdict (self);
}


void
SpawnHole (trace_t * tr, vec3_t dir)
{

  vec3_t origin;

  edict_t *lss;
  lss = G_Spawn ();
  lss->movetype = MOVETYPE_NOCLIP;
  lss->solid = SOLID_TRIGGER;
  lss->classname = "bhole";
  //lss->s.modelindex = gi.modelindex ("models/weapons/g_m4/tris.md2" );
  //lss->s.modelindex = gi.modelindex ("models/objects/holes/hole1/tris.md2" );
// bhole model doesn't want to show up, so be it
  lss->s.modelindex = gi.modelindex ("sprites/lsight.sp2");
  lss->s.renderfx = RF_GLOW;
  lss->gravity = 0;
  lss->think = BulletHoleThink;
  lss->nextthink = level.time + 1000;
  vectoangles (tr->plane.normal, lss->s.angles);
  VectorNormalize (dir);
  VectorMA (tr->endpos, 0, dir, origin);
  VectorCopy (origin, lss->s.origin);
  gi.linkentity (lss);



}


/*
=================
fire_hit

Used for all impact (hit/punch/slash) attacks
=================
*/
qboolean
fire_hit (edict_t * self, vec3_t aim, int damage, int kick)
{
  trace_t tr;
  vec3_t forward, right, up;
  vec3_t v;
  vec3_t point;
  float range;
  vec3_t dir;

  //see if enemy is in range
  VectorSubtract (self->enemy->s.origin, self->s.origin, dir);
  range = VectorLength (dir);
  if (range > aim[0])
    return false;

  if (aim[1] > self->mins[0] && aim[1] < self->maxs[0])
    {
      // the hit is straight on so back the range up to the edge of their bbox
      range -= self->enemy->maxs[0];
    }
  else
    {
      // this is a side hit so adjust the "right" value out to the edge of their bbox
      if (aim[1] < 0)
	aim[1] = self->enemy->mins[0];
      else
	aim[1] = self->enemy->maxs[0];
    }

  VectorMA (self->s.origin, range, dir, point);

  PRETRACE ();
  tr = gi.trace (self->s.origin, NULL, NULL, point, self, MASK_SHOT);
  POSTTRACE ();
  if (tr.fraction < 1)
    {
      if (!tr.ent->takedamage)
	return false;
      // if it will hit any client/monster then hit the one we wanted to hit
      if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
	tr.ent = self->enemy;
    }

  AngleVectors (self->s.angles, forward, right, up);
  VectorMA (self->s.origin, range, forward, point);
  VectorMA (point, aim[1], right, point);
  VectorMA (point, aim[2], up, point);
  VectorSubtract (point, self->enemy->s.origin, dir);

  setFFState (self);
  // do the damage
  T_Damage (tr.ent, self, self, dir, point, vec3_origin, damage, kick / 2,
	    DAMAGE_NO_KNOCKBACK, MOD_HIT);

  if (!(tr.ent->svflags & SVF_MONSTER) && (!tr.ent->client))
    return false;

  // do our special form of knockback here
  VectorMA (self->enemy->absmin, 0.5, self->enemy->size, v);
  VectorSubtract (v, point, v);
  VectorNormalize (v);
  VectorMA (self->enemy->velocity, kick, v, self->enemy->velocity);
  if (self->enemy->velocity[2] > 0)
    self->enemy->groundentity = NULL;
  return true;
}


/*
=================
fire_lead

This is an internal support routine used for bullet/pellet based weapons.
=================
*/
static void
fire_lead (edict_t * self, vec3_t start, vec3_t aimdir, int damage, int kick,
	   int te_impact, int hspread, int vspread, int mod)
{
  trace_t tr;
  vec3_t dir;
  vec3_t forward, right, up;
  vec3_t end;
  float r;
  float u;
  vec3_t water_start;
  qboolean water = false;
  int content_mask = MASK_SHOT | MASK_WATER;

  PRETRACE ();
  tr = gi.trace (self->s.origin, NULL, NULL, start, self, MASK_SHOT);
  POSTTRACE ();
  if (!(tr.fraction < 1.0))
    {
      vectoangles (aimdir, dir);
      AngleVectors (dir, forward, right, up);

      r = crandom () * hspread;
      u = crandom () * vspread;
      VectorMA (start, 8192, forward, end);
      VectorMA (end, r, right, end);
      VectorMA (end, u, up, end);

      if (gi.pointcontents (start) & MASK_WATER)
	{
	  water = true;
	  VectorCopy (start, water_start);
	  content_mask &= ~MASK_WATER;
	}

      PRETRACE ();
      tr = gi.trace (start, NULL, NULL, end, self, content_mask);
      POSTTRACE ();

// glass fx
      // catch case of firing thru one or breakable glasses
      while ((tr.fraction < 1.0)
	     && (tr.surface->flags & (SURF_TRANS33 | SURF_TRANS66))
	     && (tr.ent)
	     && (0 == Q_stricmp (tr.ent->classname, "func_explosive")))
	{
	  // break glass  
	  CGF_SFX_ShootBreakableGlass (tr.ent, self, &tr, mod);
	  // continue trace from current endpos to start
	  PRETRACE ();
	  tr = gi.trace (tr.endpos, NULL, NULL, end, tr.ent, content_mask);
	  POSTTRACE ();
	}
// ---

      // see if we hit water
      if (tr.contents & MASK_WATER)
	{
	  int color;

	  water = true;
	  VectorCopy (tr.endpos, water_start);

	  if (!VectorCompare (start, tr.endpos))
	    {
	      if (tr.contents & CONTENTS_WATER)
		{
		  if (strcmp (tr.surface->name, "*brwater") == 0)
		    color = SPLASH_BROWN_WATER;
		  else
		    color = SPLASH_BLUE_WATER;
		}
	      else if (tr.contents & CONTENTS_SLIME)
		color = SPLASH_SLIME;
	      else if (tr.contents & CONTENTS_LAVA)
		color = SPLASH_LAVA;
	      else
		color = SPLASH_UNKNOWN;

	      if (color != SPLASH_UNKNOWN)
		{
		  gi.WriteByte (svc_temp_entity);
		  gi.WriteByte (TE_SPLASH);
		  gi.WriteByte (8);
		  gi.WritePosition (tr.endpos);
		  gi.WriteDir (tr.plane.normal);
		  gi.WriteByte (color);
		  gi.multicast (tr.endpos, MULTICAST_PVS);
		}

	      // change bullet's course when it enters water
	      VectorSubtract (end, start, dir);
	      vectoangles (dir, dir);
	      AngleVectors (dir, forward, right, up);
	      r = crandom () * hspread * 2;
	      u = crandom () * vspread * 2;
	      VectorMA (water_start, 8192, forward, end);
	      VectorMA (end, r, right, end);
	      VectorMA (end, u, up, end);
	    }

	  // re-trace ignoring water this time
	  PRETRACE ();
	  tr = gi.trace (water_start, NULL, NULL, end, self, MASK_SHOT);
	  POSTTRACE ();
	}
    }

  // send gun puff / flash
  if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
    {
      if (tr.fraction < 1.0)
	{
	  if (tr.ent->takedamage)
	    {
	      T_Damage (tr.ent, self, self, aimdir, tr.endpos,
			tr.plane.normal, damage, kick, DAMAGE_BULLET, mod);
	    }
	  else
	    {
	      if (mod != MOD_M3 && mod != MOD_HC)
		{
		  AddDecal (self, &tr);
		}
	      //zucc remove spawnhole for real release
//                                SpawnHole( &tr, dir );
	      if (strncmp (tr.surface->name, "sky", 3) != 0)
		{
		  gi.WriteByte (svc_temp_entity);
		  gi.WriteByte (te_impact);
		  gi.WritePosition (tr.endpos);
		  gi.WriteDir (tr.plane.normal);
		  gi.multicast (tr.endpos, MULTICAST_PVS);

		  if (self->client)
		    PlayerNoise (self, tr.endpos, PNOISE_IMPACT);
		}
	    }
	}
    }

  // if went through water, determine where the end and make a bubble trail
  if (water)
    {
      vec3_t pos;

      VectorSubtract (tr.endpos, water_start, dir);
      VectorNormalize (dir);
      VectorMA (tr.endpos, -2, dir, pos);
      if (gi.pointcontents (pos) & MASK_WATER)
	VectorCopy (pos, tr.endpos);
      else
	{
	  PRETRACE ();
	  tr = gi.trace (pos, NULL, NULL, water_start, tr.ent, MASK_WATER);
	  POSTTRACE ();
	}

      VectorAdd (water_start, tr.endpos, pos);
      VectorScale (pos, 0.5, pos);

      gi.WriteByte (svc_temp_entity);
      gi.WriteByte (TE_BUBBLETRAIL);
      gi.WritePosition (water_start);
      gi.WritePosition (tr.endpos);
      gi.multicast (pos, MULTICAST_PVS);
    }
}


/*
=================
fire_bullet

Fires a single round.  Used for machinegun and chaingun.  Would be fine for
pistols, rifles, etc....
=================
*/
void
fire_bullet (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
	     int kick, int hspread, int vspread, int mod)
{
  setFFState (self);
  fire_lead (self, start, aimdir, damage, kick, TE_GUNSHOT, hspread, vspread,
	     mod);
}



// zucc fire_load_ap for rounds that pass through soft targets and keep going
static void
fire_lead_ap (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
	      int kick, int te_impact, int hspread, int vspread, int mod)
{
  trace_t tr;
  vec3_t dir;
  vec3_t forward, right, up;
  vec3_t end;
  float r;
  float u;
  vec3_t water_start;
  qboolean water = false;
  int content_mask = MASK_SHOT | MASK_WATER;
  vec3_t from;
  edict_t *ignore;

//FIREBLADE
  memset (ap_already_hit, 0, game.maxclients * sizeof (qboolean));
//FIREBLADE

  // setup
  stopAP = 0;
  vectoangles (aimdir, dir);
  AngleVectors (dir, forward, right, up);

  r = crandom () * hspread;
  u = crandom () * vspread;
  VectorMA (start, 8192, forward, end);
  VectorMA (end, r, right, end);
  VectorMA (end, u, up, end);
  VectorCopy (start, from);
  if (gi.pointcontents (start) & MASK_WATER)
    {
      water = true;
      VectorCopy (start, water_start);
      content_mask &= ~MASK_WATER;
    }

  ignore = self;

  //      PRETRACE();
  //      tr = gi.trace (self->s.origin, NULL, NULL, start, self, MASK_SHOT);
  //      POSTTRACE();
  while (ignore)
    //if (!(tr.fraction < 1.0))
    {

      PRETRACE ();
      //tr = gi.trace (from, NULL, NULL, end, ignore, mask);
      tr = gi.trace (from, NULL, NULL, end, ignore, content_mask);
      POSTTRACE ();

// glass fx
      // catch case of firing thru one or breakable glasses
      while ((tr.fraction < 1.0)
	     && (tr.surface->flags & (SURF_TRANS33 | SURF_TRANS66))
	     && (tr.ent)
	     && (0 == Q_stricmp (tr.ent->classname, "func_explosive")))
	{
	  // break glass  
	  CGF_SFX_ShootBreakableGlass (tr.ent, self, &tr, mod);
	  // continue trace from current endpos to start
	  PRETRACE ();
	  tr = gi.trace (tr.endpos, NULL, NULL, end, tr.ent, content_mask);
	  POSTTRACE ();
	}
// ---

      // see if we hit water
      if (tr.contents & MASK_WATER)
	{
	  int color;

	  water = true;
	  VectorCopy (tr.endpos, water_start);

	  if (!VectorCompare (from, tr.endpos))
	    {
	      if (tr.contents & CONTENTS_WATER)
		{
		  if (strcmp (tr.surface->name, "*brwater") == 0)
		    color = SPLASH_BROWN_WATER;
		  else
		    color = SPLASH_BLUE_WATER;
		}
	      else if (tr.contents & CONTENTS_SLIME)
		color = SPLASH_SLIME;
	      else if (tr.contents & CONTENTS_LAVA)
		color = SPLASH_LAVA;
	      else
		color = SPLASH_UNKNOWN;

	      if (color != SPLASH_UNKNOWN)
		{
		  gi.WriteByte (svc_temp_entity);
		  gi.WriteByte (TE_SPLASH);
		  gi.WriteByte (8);
		  gi.WritePosition (tr.endpos);
		  gi.WriteDir (tr.plane.normal);
		  gi.WriteByte (color);
		  gi.multicast (tr.endpos, MULTICAST_PVS);
		}

	      // change bullet's course when it enters water
	      VectorSubtract (end, from, dir);
	      vectoangles (dir, dir);
	      AngleVectors (dir, forward, right, up);
	      r = crandom () * hspread * 2;
	      u = crandom () * vspread * 2;
	      VectorMA (water_start, 8192, forward, end);
	      VectorMA (end, r, right, end);
	      VectorMA (end, u, up, end);
	    }

	  // re-trace ignoring water this time
	  PRETRACE ();
	  tr = gi.trace (water_start, NULL, NULL, end, ignore, MASK_SHOT);
	  POSTTRACE ();
	}

      // send gun puff / flash

      ignore = NULL;

      if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
	{
	  if (tr.fraction < 1.0)
	    {

	      if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
		{
		  ignore = tr.ent;
		  VectorCopy (tr.endpos, from);
//FIREBLADE
		  // Advance the "from" point a few units
		  // towards "end" here
		  if (tr.ent->client)
		    {
		      vec3_t tempv;
		      vec3_t out;
		      if (ap_already_hit[tr.ent - g_edicts - 1])
			{
			  VectorSubtract (end, from, tempv);
			  VectorNormalize (tempv);
			  VectorScale (tempv, 8, out);
			  VectorAdd (out, from, from);
			  continue;
			}
		      ap_already_hit[tr.ent - g_edicts - 1] = true;
		    }
//FIREBLADE
		}

	      if ((tr.ent != self) && (tr.ent->takedamage))
		{
		  T_Damage (tr.ent, self, self, aimdir, tr.endpos,
			    tr.plane.normal, damage, kick, 0, mod);
		  if (stopAP)	// the AP round hit something that would stop it (kevlar)
		    ignore = NULL;
		}
	      else if (tr.ent != self && !water)
		{
		  //zucc remove spawnhole for real release
		  //        SpawnHole( &tr, dir  );

		  if (strncmp (tr.surface->name, "sky", 3) != 0)
		    {
		      AddDecal (self, &tr);
		      gi.WriteByte (svc_temp_entity);
		      gi.WriteByte (te_impact);
		      gi.WritePosition (tr.endpos);
		      gi.WriteDir (tr.plane.normal);
		      gi.multicast (tr.endpos, MULTICAST_PVS);

		      if (self->client)
			PlayerNoise (self, tr.endpos, PNOISE_IMPACT);
		    }
		}
	    }
	}
    }
  // if went through water, determine where the end and make a bubble trail
  if (water)
    {
      vec3_t pos;

      VectorSubtract (tr.endpos, water_start, dir);
      VectorNormalize (dir);
      VectorMA (tr.endpos, -2, dir, pos);
      if (gi.pointcontents (pos) & MASK_WATER)
	VectorCopy (pos, tr.endpos);
      else
	{
	  PRETRACE ();
	  tr = gi.trace (pos, NULL, NULL, water_start, tr.ent, MASK_WATER);
	  POSTTRACE ();
	}

      VectorAdd (water_start, tr.endpos, pos);
      VectorScale (pos, 0.5, pos);

      gi.WriteByte (svc_temp_entity);
      gi.WriteByte (TE_BUBBLETRAIL);
      gi.WritePosition (water_start);
      gi.WritePosition (tr.endpos);
      gi.multicast (pos, MULTICAST_PVS);
    }

}



// zucc - for the M4
void
fire_bullet_sparks (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
		    int kick, int hspread, int vspread, int mod)
{
  setFFState (self);
  fire_lead_ap (self, start, aimdir, damage, kick, TE_BULLET_SPARKS, hspread,
		vspread, mod);
}

// zucc - for sniper
void
fire_bullet_sniper (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
		    int kick, int hspread, int vspread, int mod)
{
  setFFState (self);
  fire_lead_ap (self, start, aimdir, damage, kick, TE_GUNSHOT, hspread,
		vspread, mod);
}


/*
 * Init and ProduceShotgunDamageReport 
 */

void InitShotgunDamageReport ()
{
  memset (took_damage, 0, game.maxclients * sizeof (int));
}

void ProduceShotgunDamageReport (edict_t * self)
{
	int l;
	int total = 0, total_to_print, printed = 0;
	char *textbuf;

	//FB 6/2/99 - shotgun/handcannon damage notification
	for (l = 1; l <= game.maxclients; l++)
	{
		if (took_damage[l - 1])
			total++;
	}
	if (!total)
		return;

	if (total > 10)
		total_to_print = 10;
	else
		total_to_print = total;

	//256 is enough, its limited to 10 nicks
	textbuf = self->client->resp.last_damaged_players;
	*textbuf = '\0';

	for (l = 1; l <= game.maxclients; l++)
	{
		if (took_damage[l - 1])
		{
			if (printed == total_to_print - 1)
			{
				if (total_to_print == 2)
					strcat(textbuf, " and ");
				else if (total_to_print != 1)
					strcat(textbuf, ", and ");
			}
			else if (printed) {
				strcat (textbuf, ", ");
			}
			strcat (textbuf, g_edicts[l].client->pers.netname);
			printed++;
		}
		if (printed == total_to_print)
			break;
	}
	gi.cprintf (self, PRINT_HIGH, "You hit %s\n", textbuf);

	self->client->resp.last_damaged_part = LOC_NO;

	// TNG Stats
	if (!teamplay->value || team_round_going || stats_afterround->value) {

		self->client->resp.stats_shots_h += total;

		if (self->client->curr_weap == M3_NUM)
			self->client->resp.stats_hits[MOD_M3] += total;
		else if (self->client->curr_weap == HC_NUM)
			self->client->resp.stats_hits[MOD_HC] += total;

		/*

		if (self->client->curr_weap == M3_NUM)
		self->client->resp.stats_shotgun_shots_h += total_to_print;

		if (self->client->curr_weap == HC_NUM)
		self->client->resp.stats_hc_shots_h += total_to_print;
		*/

	}

/*
  self->client->resp.stats_shots_h += total_to_print;
  if (self->client->curr_weap == M3_NUM)
    self->client->resp.stats_shotgun_shots_h += total_to_print;
  if (self->client->curr_weap == HC_NUM)
    self->client->resp.stats_hc_shots_h += total_to_print;
  if (mod == MOD_M3)
    self->client->resp.stats_shotgun_shots_h += total_to_print;
  if (mod == MOD_HC)
    self->client->resp.stats_hc_shots_h += total_to_print; */
//FB 6/2/99
}


/*
=================
fire_shotgun

Shoots shotgun pellets.  Used by shotgun and super shotgun.
=================
*/
void
fire_shotgun (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
	      int kick, int hspread, int vspread, int count, int mod)
{
  int i;

  for (i = 0; i < count; i++)
    fire_lead (self, start, aimdir, damage, kick, TE_SHOTGUN, hspread,
	       vspread, mod);
}


/*
=================
fire_blaster

Fires a single blaster bolt.  Used by the blaster and hyperb blaster.
=================
*/
void
blaster_touch (edict_t * self, edict_t * other, cplane_t * plane,
	       csurface_t * surf)
{
  int mod;

  if (other == self->owner)
    return;

  if (surf && (surf->flags & SURF_SKY))
    {
      G_FreeEdict (self);
      return;
    }

  if (self->owner->client)
    PlayerNoise (self->owner, self->s.origin, PNOISE_IMPACT);

  if (other->takedamage)
    {
      if (self->spawnflags & 1)
	mod = MOD_HYPERBLASTER;
      else
	mod = MOD_BLASTER;
      T_Damage (other, self, self->owner, self->velocity, self->s.origin,
		plane->normal, self->dmg, 1, DAMAGE_ENERGY, mod);
    }
  else
    {
      gi.WriteByte (svc_temp_entity);
      gi.WriteByte (TE_BLASTER);
      gi.WritePosition (self->s.origin);
      if (!plane)
	gi.WriteDir (vec3_origin);
      else
	gi.WriteDir (plane->normal);
      gi.multicast (self->s.origin, MULTICAST_PVS);
    }

  G_FreeEdict (self);
}
//SLIC2 changed argument name hyper to hyperb
void
fire_blaster (edict_t * self, vec3_t start, vec3_t dir, int damage, int speed,
	      int effect, qboolean hyperb)
{
  edict_t *bolt;
  trace_t tr;

  VectorNormalize (dir);

  bolt = G_Spawn ();
// 3.20 ANTI-LAG FIX, just in case we end up using this code for something... -FB
  bolt->svflags = SVF_DEADMONSTER;
// ^^^
  VectorCopy (start, bolt->s.origin);
  VectorCopy (start, bolt->s.old_origin);
  vectoangles (dir, bolt->s.angles);
  VectorScale (dir, speed, bolt->velocity);
  bolt->movetype = MOVETYPE_FLYMISSILE;
  bolt->clipmask = MASK_SHOT;
  bolt->solid = SOLID_BBOX;
  bolt->s.effects |= effect;
  VectorClear (bolt->mins);
  VectorClear (bolt->maxs);
  bolt->s.modelindex = gi.modelindex ("models/objects/laser/tris.md2");
  bolt->s.sound = gi.soundindex ("misc/lasfly.wav");
  bolt->owner = self;
  bolt->touch = blaster_touch;
  bolt->nextthink = level.time + 2;
  bolt->think = G_FreeEdict;
  bolt->dmg = damage;
  bolt->classname = "bolt";
  if (hyperb)
    bolt->spawnflags = 1;
  gi.linkentity (bolt);

  if (self->client)
    check_dodge (self, bolt->s.origin, dir, speed);

  PRETRACE ();
  tr = gi.trace (self->s.origin, NULL, NULL, bolt->s.origin, bolt, MASK_SHOT);
  POSTTRACE ();

  if (tr.fraction < 1.0)
    {
      VectorMA (bolt->s.origin, -10, dir, bolt->s.origin);
      bolt->touch (bolt, tr.ent, NULL, NULL);
    }
}

qboolean
G_EntExists (edict_t * ent)
{
  return ((ent) && (ent->client));	//(ent->inuse)); 
}



/*
=================
fire_grenade
=================
*/
static void
Grenade_Explode (edict_t * ent)
{
  vec3_t origin;
  int mod;


  if (ent->owner->client)
    PlayerNoise (ent->owner, ent->s.origin, PNOISE_IMPACT);

  //FIXME: if we are onground then raise our Z just a bit since we are a point?
  if (ent->enemy)
    {
      float points;
      vec3_t v;
      vec3_t dir;

      VectorAdd (ent->enemy->mins, ent->enemy->maxs, v);
      VectorMA (ent->enemy->s.origin, 0.5, v, v);
      VectorSubtract (ent->s.origin, v, v);
      points = ent->dmg - 0.5 * VectorLength (v);
      VectorSubtract (ent->enemy->s.origin, ent->s.origin, dir);
      if (ent->spawnflags & 1)
	mod = MOD_HANDGRENADE;
      else
	mod = MOD_GRENADE;
      T_Damage (ent->enemy, ent, ent->owner, dir, ent->s.origin, vec3_origin,
		(int) points, (int) points, DAMAGE_RADIUS, mod);
    }

  if (ent->spawnflags & 2)
    mod = MOD_HELD_GRENADE;
  else if (ent->spawnflags & 1)
    mod = MOD_HG_SPLASH;
  else
    mod = MOD_G_SPLASH;
  T_RadiusDamage (ent, ent->owner, ent->dmg, ent->enemy, ent->dmg_radius,
		  mod);

  VectorMA (ent->s.origin, -0.02, ent->velocity, origin);
  gi.WriteByte (svc_temp_entity);
  if (ent->waterlevel)
    {
      if (ent->groundentity)
	gi.WriteByte (TE_GRENADE_EXPLOSION_WATER);
      else
	gi.WriteByte (TE_ROCKET_EXPLOSION_WATER);
    }
  else
    {
      if (ent->groundentity)
	gi.WriteByte (TE_GRENADE_EXPLOSION);
      else
	gi.WriteByte (TE_ROCKET_EXPLOSION);
    }
  gi.WritePosition (origin);
  gi.multicast (ent->s.origin, MULTICAST_PHS);

  G_FreeEdict (ent);
}

static void
Grenade_Touch (edict_t * ent, edict_t * other, cplane_t * plane,
	       csurface_t * surf)
{
  if (other == ent->owner)
    return;

  if (surf && (surf->flags & SURF_SKY))
    {
      G_FreeEdict (ent);
      return;
    }

  if (!other->takedamage)
    {
/*FIREBLADE
                if (ent->spawnflags & 1)
                {
                        if (random() > 0.5)
                                gi.sound (ent, CHAN_VOICE, gi.soundindex ("weapons/hgrenb1a.wav"), 1, ATTN_NORM, 0);
                        else
                                gi.sound (ent, CHAN_VOICE, gi.soundindex ("weapons/hgrenb2a.wav"), 1, ATTN_NORM, 0);
                }
                else
FIREBLADE*/
      {
	gi.sound (ent, CHAN_VOICE, gi.soundindex ("weapons/grenlb1b.wav"), 1,
		  ATTN_NORM, 0);
      }
      return;
    }

  // zucc not needed since grenades don't blow up on contact
  //ent->enemy = other;
  //Grenade_Explode (ent);
}

void
fire_grenade (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
	      int speed, float timer, float damage_radius)
{
  edict_t *grenade;
  vec3_t dir;
  vec3_t forward, right, up;

  vectoangles (aimdir, dir);
  AngleVectors (dir, forward, right, up);

  grenade = G_Spawn ();
  VectorCopy (start, grenade->s.origin);
  VectorScale (aimdir, speed, grenade->velocity);
  VectorMA (grenade->velocity, 200 + crandom () * 10.0, up,
	    grenade->velocity);
  VectorMA (grenade->velocity, crandom () * 10.0, right, grenade->velocity);
  VectorSet (grenade->avelocity, 300, 300, 300);
  grenade->movetype = MOVETYPE_BOUNCE;
  grenade->clipmask = MASK_SHOT;
  grenade->solid = SOLID_BBOX;
  grenade->s.effects |= EF_GRENADE;
  VectorClear (grenade->mins);
  VectorClear (grenade->maxs);
  grenade->s.modelindex = gi.modelindex ("models/objects/grenade/tris.md2");
  grenade->owner = self;
  grenade->touch = Grenade_Touch;
  grenade->nextthink = level.time + timer;
  grenade->think = Grenade_Explode;
  grenade->dmg = damage;
  grenade->dmg_radius = damage_radius;
  grenade->classname = "grenade";

  gi.linkentity (grenade);
}

void
fire_grenade2 (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
	       int speed, float timer, float damage_radius, qboolean held)
{
  edict_t *grenade;
  vec3_t dir;
  vec3_t forward, right, up;

  vectoangles (aimdir, dir);
  AngleVectors (dir, forward, right, up);

  grenade = G_Spawn ();
  VectorCopy (start, grenade->s.origin);
  VectorScale (aimdir, speed, grenade->velocity);
  VectorMA (grenade->velocity, 200 + crandom () * 10.0, up,
	    grenade->velocity);
  VectorMA (grenade->velocity, crandom () * 10.0, right, grenade->velocity);
  VectorSet (grenade->avelocity, 300, 300, 300);
  grenade->movetype = MOVETYPE_BOUNCE;
  grenade->clipmask = MASK_SHOT;
  grenade->solid = SOLID_BBOX;
  //grenade->s.effects |= EF_GRENADE;
  VectorClear (grenade->mins);
  VectorClear (grenade->maxs);
  grenade->s.modelindex = gi.modelindex ("models/objects/grenade2/tris.md2");
  grenade->owner = self;
  grenade->touch = Grenade_Touch;
  grenade->nextthink = level.time + timer;
  grenade->think = Grenade_Explode;
  grenade->dmg = damage;
  grenade->dmg_radius = damage_radius;
  grenade->classname = "hgrenade";
  if (held)
    grenade->spawnflags = 3;
  else
    grenade->spawnflags = 1;
  //grenade->s.sound = gi.soundindex("weapons/hgrenc1b.wav");

  if (timer <= 0.0)
    Grenade_Explode (grenade);
  else
    {
      gi.sound (self, CHAN_WEAPON, gi.soundindex ("weapons/hgrent1a.wav"), 1,
		ATTN_NORM, 0);
      gi.linkentity (grenade);
    }
}


/*
=================
fire_rocket
=================
*/
void
rocket_touch (edict_t * ent, edict_t * other, cplane_t * plane,
	      csurface_t * surf)
{
  vec3_t origin;
  int n;

  if (other == ent->owner)
    return;

  if (surf && (surf->flags & SURF_SKY))
    {
      G_FreeEdict (ent);
      return;
    }

  if (ent->owner->client)
    PlayerNoise (ent->owner, ent->s.origin, PNOISE_IMPACT);

  // calculate position for the explosion entity
  VectorMA (ent->s.origin, -0.02, ent->velocity, origin);

  if (other->takedamage)
    {
      T_Damage (other, ent, ent->owner, ent->velocity, ent->s.origin,
		plane->normal, ent->dmg, 0, 0, MOD_ROCKET);
    }
  else
    {
      // don't throw any debris in net games
      if (!deathmatch->value && !coop->value)
	{
	  if ((surf)
	      && !(surf->
		   flags & (SURF_WARP | SURF_TRANS33 | SURF_TRANS66 |
			    SURF_FLOWING)))
	    {
	      n = rand () % 5;
	      while (n--)
		ThrowDebris (ent, "models/objects/debris2/tris.md2", 2,
			     ent->s.origin);
	    }
	}
    }

  T_RadiusDamage (ent, ent->owner, ent->radius_dmg, other, ent->dmg_radius,
		  MOD_R_SPLASH);

  gi.WriteByte (svc_temp_entity);
  if (ent->waterlevel)
    gi.WriteByte (TE_ROCKET_EXPLOSION_WATER);
  else
    gi.WriteByte (TE_ROCKET_EXPLOSION);
  gi.WritePosition (origin);
  gi.multicast (ent->s.origin, MULTICAST_PHS);

  G_FreeEdict (ent);
}

void
fire_rocket (edict_t * self, vec3_t start, vec3_t dir, int damage, int speed,
	     float damage_radius, int radius_damage)
{
  edict_t *rocket;

  rocket = G_Spawn ();
  VectorCopy (start, rocket->s.origin);
  VectorCopy (dir, rocket->movedir);
  vectoangles (dir, rocket->s.angles);
  VectorScale (dir, speed, rocket->velocity);
  rocket->movetype = MOVETYPE_FLYMISSILE;
  rocket->clipmask = MASK_SHOT;
  rocket->solid = SOLID_BBOX;
  rocket->s.effects |= EF_ROCKET;
  VectorClear (rocket->mins);
  VectorClear (rocket->maxs);
  rocket->s.modelindex = gi.modelindex ("models/objects/rocket/tris.md2");
  rocket->owner = self;
  rocket->touch = rocket_touch;
  rocket->nextthink = level.time + 8000 / speed;
  rocket->think = G_FreeEdict;
  rocket->dmg = damage;
  rocket->radius_dmg = radius_damage;
  rocket->dmg_radius = damage_radius;
  rocket->s.sound = gi.soundindex ("weapons/rockfly.wav");
  rocket->classname = "rocket";

  if (self->client)
    check_dodge (self, rocket->s.origin, dir, speed);

  gi.linkentity (rocket);
}


/*
=================
fire_rail
=================
*/
void
fire_rail (edict_t * self, vec3_t start, vec3_t aimdir, int damage, int kick)
{
  vec3_t from;
  vec3_t end;
  trace_t tr;
  edict_t *ignore;
  int mask;
  qboolean water;

  VectorMA (start, 8192, aimdir, end);
  VectorCopy (start, from);
  ignore = self;
  water = false;
  mask = MASK_SHOT | CONTENTS_SLIME | CONTENTS_LAVA;
  while (ignore)
    {
      PRETRACE ();
      tr = gi.trace (from, NULL, NULL, end, ignore, mask);
      POSTTRACE ();

      if (tr.contents & (CONTENTS_SLIME | CONTENTS_LAVA))
	{
	  mask &= ~(CONTENTS_SLIME | CONTENTS_LAVA);
	  water = true;
	}
      else
	{
// FROM 3.20, just in case we ever end up using this code for something...      -FB
	  //ZOID--added so rail goes through SOLID_BBOX entities (gibs, etc)
	  if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client) ||
	      (tr.ent->solid == SOLID_BBOX))
// ^^^
	    ignore = tr.ent;
	  else
	    ignore = NULL;

	  if ((tr.ent != self) && (tr.ent->takedamage))
	    T_Damage (tr.ent, self, self, aimdir, tr.endpos, tr.plane.normal,
		      damage, kick, 0, MOD_RAILGUN);
	}

      VectorCopy (tr.endpos, from);
    }

  // send gun puff / flash
  gi.WriteByte (svc_temp_entity);
  gi.WriteByte (TE_RAILTRAIL);
  gi.WritePosition (start);
  gi.WritePosition (tr.endpos);
  gi.multicast (self->s.origin, MULTICAST_PHS);
//      gi.multicast (start, MULTICAST_PHS);
  if (water)
    {
      gi.WriteByte (svc_temp_entity);
      gi.WriteByte (TE_RAILTRAIL);
      gi.WritePosition (start);
      gi.WritePosition (tr.endpos);
      gi.multicast (tr.endpos, MULTICAST_PHS);
    }

  if (self->client)
    PlayerNoise (self, tr.endpos, PNOISE_IMPACT);
}


/*
=================
fire_bfg
=================
*/
void
bfg_explode (edict_t * self)
{
  edict_t *ent;
  float points;
  vec3_t v;
  float dist;

  if (self->s.frame == 0)
    {
      // the BFG effect
      ent = NULL;
      while ((ent = findradius (ent, self->s.origin, self->dmg_radius)) !=
	     NULL)
	{
	  if (!ent->takedamage)
	    continue;
	  if (ent == self->owner)
	    continue;
	  if (!CanDamage (ent, self))
	    continue;
	  if (!CanDamage (ent, self->owner))
	    continue;

	  VectorAdd (ent->mins, ent->maxs, v);
	  VectorMA (ent->s.origin, 0.5, v, v);
	  VectorSubtract (self->s.origin, v, v);
	  dist = VectorLength (v);
	  points = self->radius_dmg * (1.0 - sqrt (dist / self->dmg_radius));
	  if (ent == self->owner)
	    points = points * 0.5;

	  gi.WriteByte (svc_temp_entity);
	  gi.WriteByte (TE_BFG_EXPLOSION);
	  gi.WritePosition (ent->s.origin);
	  gi.multicast (ent->s.origin, MULTICAST_PHS);
	  T_Damage (ent, self, self->owner, self->velocity, ent->s.origin,
		    vec3_origin, (int) points, 0, DAMAGE_ENERGY,
		    MOD_BFG_EFFECT);
	}
    }

  self->nextthink = level.time + FRAMETIME;
  self->s.frame++;
  if (self->s.frame == 5)
    self->think = G_FreeEdict;
}

void
bfg_touch (edict_t * self, edict_t * other, cplane_t * plane,
	   csurface_t * surf)
{
  if (other == self->owner)
    return;

  if (surf && (surf->flags & SURF_SKY))
    {
      G_FreeEdict (self);
      return;
    }

  if (self->owner->client)
    PlayerNoise (self->owner, self->s.origin, PNOISE_IMPACT);

  // core explosion - prevents firing it into the wall/floor
  if (other->takedamage)
    T_Damage (other, self, self->owner, self->velocity, self->s.origin,
	      plane->normal, 200, 0, 0, MOD_BFG_BLAST);
  T_RadiusDamage (self, self->owner, 200, other, 100, MOD_BFG_BLAST);

  gi.sound (self, CHAN_VOICE, gi.soundindex ("weapons/bfg__x1b.wav"), 1,
	    ATTN_NORM, 0);
  self->solid = SOLID_NOT;
  self->touch = NULL;
  VectorMA (self->s.origin, -1 * FRAMETIME, self->velocity, self->s.origin);
  VectorClear (self->velocity);
  self->s.modelindex = gi.modelindex ("sprites/s_bfg3.sp2");
  self->s.frame = 0;
  self->s.sound = 0;
  self->s.effects &= ~EF_ANIM_ALLFAST;
  self->think = bfg_explode;
  self->nextthink = level.time + FRAMETIME;
  self->enemy = other;

  gi.WriteByte (svc_temp_entity);
  gi.WriteByte (TE_BFG_BIGEXPLOSION);
  gi.WritePosition (self->s.origin);
  gi.multicast (self->s.origin, MULTICAST_PVS);
}


void
bfg_think (edict_t * self)
{
  edict_t *ent;
  edict_t *ignore;
  vec3_t point;
  vec3_t dir;
  vec3_t start;
  vec3_t end;
  int dmg;
  trace_t tr;

  if (deathmatch->value)
    dmg = 5;
  else
    dmg = 10;

  ent = NULL;
  while ((ent = findradius (ent, self->s.origin, 256)) != NULL)
    {
      if (ent == self)
	continue;

      if (ent == self->owner)
	continue;

      if (!ent->takedamage)
	continue;

      if (!(ent->svflags & SVF_MONSTER) && (!ent->client)
	  && (strcmp (ent->classname, "misc_explobox") != 0))
	continue;
      //AQ2:TNG Igor not quite sure about this (FIXME)
      //don't target players in CTF
      if (ctf->value && ent->client &&
	  self->owner->client &&
	  ent->client->resp.team == self->owner->client->resp.team)
	continue;
      //AQ2:TNG End
      VectorMA (ent->absmin, 0.5, ent->size, point);

      VectorSubtract (point, self->s.origin, dir);
      VectorNormalize (dir);

      ignore = self;
      VectorCopy (self->s.origin, start);
      VectorMA (start, 2048, dir, end);
      while (1)
	{
	  PRETRACE ();
	  tr =
	    gi.trace (start, NULL, NULL, end, ignore,
		      CONTENTS_SOLID | CONTENTS_MONSTER |
		      CONTENTS_DEADMONSTER);
	  POSTTRACE ();

	  if (!tr.ent)
	    break;

	  // hurt it if we can
	  if ((tr.ent->takedamage) && !(tr.ent->flags & FL_IMMUNE_LASER)
	      && (tr.ent != self->owner))
	    T_Damage (tr.ent, self, self->owner, dir, tr.endpos, vec3_origin,
		      dmg, 1, DAMAGE_ENERGY, MOD_BFG_LASER);

	  // if we hit something that's not a monster or player we're done
	  if (!(tr.ent->svflags & SVF_MONSTER) && (!tr.ent->client))
	    {
	      gi.WriteByte (svc_temp_entity);
	      gi.WriteByte (TE_LASER_SPARKS);
	      gi.WriteByte (4);
	      gi.WritePosition (tr.endpos);
	      gi.WriteDir (tr.plane.normal);
	      gi.WriteByte (self->s.skinnum);
	      gi.multicast (tr.endpos, MULTICAST_PVS);
	      break;
	    }

	  ignore = tr.ent;
	  VectorCopy (tr.endpos, start);
	}

      gi.WriteByte (svc_temp_entity);
      gi.WriteByte (TE_BFG_LASER);
      gi.WritePosition (self->s.origin);
      gi.WritePosition (tr.endpos);
      gi.multicast (self->s.origin, MULTICAST_PHS);
    }

  self->nextthink = level.time + FRAMETIME;
}


void
fire_bfg (edict_t * self, vec3_t start, vec3_t dir, int damage, int speed,
	  float damage_radius)
{
  edict_t *bfg;

  bfg = G_Spawn ();
  VectorCopy (start, bfg->s.origin);
  VectorCopy (dir, bfg->movedir);
  vectoangles (dir, bfg->s.angles);
  VectorScale (dir, speed, bfg->velocity);
  bfg->movetype = MOVETYPE_FLYMISSILE;
  bfg->clipmask = MASK_SHOT;
  bfg->solid = SOLID_BBOX;
  bfg->s.effects |= EF_BFG | EF_ANIM_ALLFAST;
  VectorClear (bfg->mins);
  VectorClear (bfg->maxs);
  bfg->s.modelindex = gi.modelindex ("sprites/s_bfg1.sp2");
  bfg->owner = self;
  bfg->touch = bfg_touch;
  bfg->nextthink = level.time + 8000 / speed;
  bfg->think = G_FreeEdict;
  bfg->radius_dmg = damage;
  bfg->dmg_radius = damage_radius;
  bfg->classname = "bfg blast";
  bfg->s.sound = gi.soundindex ("weapons/bfg__l1a.wav");

  bfg->think = bfg_think;
  bfg->nextthink = level.time + FRAMETIME;
  bfg->teammaster = bfg;
  bfg->teamchain = NULL;

  if (self->client)
    check_dodge (self, bfg->s.origin, dir, speed);

  gi.linkentity (bfg);
}


void P_ProjectSource (gclient_t * client, vec3_t point, vec3_t distance,
		      vec3_t forward, vec3_t right, vec3_t result);

qboolean IsFemale (edict_t * ent);

void kick_attack (edict_t * ent)
{
	vec3_t start;
	vec3_t forward, right;
	vec3_t offset;
	int damage = 20;
	int kick = 400;
	trace_t tr;
	vec3_t end;


	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, 0, ent->client->kick_origin);

	VectorSet (offset, 0, 0, ent->viewheight - 20);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	VectorMA (start, 25, forward, end);

	PRETRACE ();
	tr = gi.trace (ent->s.origin, NULL, NULL, end, ent, MASK_SHOT);
	POSTTRACE ();

	// don't need to check for water
	if ((tr.surface) && (tr.surface->flags & SURF_SKY))
		return;

	if (tr.fraction >= 1.0)
		return;

	if (tr.ent->takedamage || KickDoor (&tr, ent, forward))
	{
		if (tr.ent->health <= 0)
			return;

		if (teamplay->value)
		{
			// AQ2:TNG - JBravo adding UVtime
			if (tr.ent->client && tr.ent->client->ctf_uvtime)
				return;

			//                                        if (tr.ent != ent && tr.ent->client && ent->client &&
			//                                                tr.ent->client->resp.team == ent->client->resp.team)
			// AQ:TNG - JBravo adding FF after rounds
			if ((tr.ent != ent) && (tr.ent->client && ent->client) &&
				(tr.ent->client->resp.team == ent->client->resp.team) &&
				team_round_going)
				return;
			if (!ff_afterround->value)
				return;
			// AQ:TNG
		}
		else if (((tr.ent != ent) && ((deathmatch->value
			&& ((int) (dmflags->value) & (DF_MODELTEAMS | DF_SKINTEAMS)))
			|| coop->value)	&& OnSameTeam (tr.ent, ent)))
			return;
		// zucc stop powerful upwards kicking
		//forward[2] = 0;
		// glass fx
		if (Q_stricmp (tr.ent->classname, "func_explosive") == 0)
			CGF_SFX_ShootBreakableGlass (tr.ent, ent, &tr, MOD_KICK);
		else
			T_Damage (tr.ent, ent, ent, forward, tr.endpos,
				tr.plane.normal, damage, kick, 0, MOD_KICK);

		gi.sound (ent, CHAN_WEAPON, gi.soundindex ("weapons/kick.wav"),
		1, ATTN_NORM, 0);
		PlayerNoise (ent, ent->s.origin, PNOISE_SELF);
		ent->client->jumping = 0;	// only 1 jumpkick per jump
		if (tr.ent->client && (tr.ent->client->curr_weap == M4_NUM
			|| tr.ent->client->curr_weap == MP5_NUM
			|| tr.ent->client->curr_weap == M3_NUM
			|| tr.ent->client->curr_weap ==	SNIPER_NUM
			|| tr.ent->client->curr_weap == HC_NUM))		// crandom() > .8 ) 
		{
			// zucc fix this so reloading won't happen on the new gun!
			tr.ent->client->reload_attempts = 0;
			DropSpecialWeapon (tr.ent);

			gi.cprintf (ent, PRINT_HIGH, "You kick %s's %s from %s hands!\n",
				tr.ent->client->pers.netname,(tr.ent->client->pers.weapon)->pickup_name, IsFemale(tr.ent) ? "her" : "his");

			gi.cprintf (tr.ent, PRINT_HIGH,	"%s kicked your weapon from your hands!\n",
				ent->client->pers.netname);
		} else if(tr.ent->client && tr.ent->client->ctf_grapple && tr.ent->client->ctf_grapplestate == CTF_GRAPPLE_STATE_FLY) {
			// hifi: if the player is shooting a grapple, lose it's focus
			CTFPlayerResetGrapple(tr.ent);
		}

	}
}

// zucc
// return values
// 0 - missed
// 1 - hit player
// 2 - hit wall


int
knife_attack (edict_t * self, vec3_t start, vec3_t aimdir, int damage,
	      int kick)
{
  trace_t tr;
  vec3_t end;


  VectorMA (start, 45, aimdir, end);

  PRETRACE ();
  tr = gi.trace (self->s.origin, NULL, NULL, end, self, MASK_SHOT);
  POSTTRACE ();

  // don't need to check for water
  if (!((tr.surface) && (tr.surface->flags & SURF_SKY)))
    {
      if (tr.fraction < 1.0)
	{
//glass fx
	  if (0 == Q_stricmp (tr.ent->classname, "func_explosive"))
	    {
	      CGF_SFX_ShootBreakableGlass (tr.ent, self, &tr, MOD_KNIFE);
	    }
	  else
// ---
	  if (tr.ent->takedamage)
	    {
	      setFFState (self);
	      T_Damage (tr.ent, self, self, aimdir, tr.endpos,
			tr.plane.normal, damage, kick, 0, MOD_KNIFE);
	      return -2;
	    }
	  else
	    {
	      gi.WriteByte (svc_temp_entity);
	      gi.WriteByte (TE_SPARKS);
	      gi.WritePosition (tr.endpos);
	      gi.WriteDir (tr.plane.normal);
	      gi.multicast (tr.endpos, MULTICAST_PVS);
	      return -1;
	    }

	}
      else
	return 0;
    }
  return 0;			// we hit the sky, call it a miss
}

static int knives = 0;

void
knife_touch (edict_t * ent, edict_t * other, cplane_t * plane,
	     csurface_t * surf)
{
  vec3_t origin;
  //      int                     n;

  edict_t *dropped;
  edict_t *knife;
  //      vec3_t          forward, right, up;
  vec3_t move_angles;
  gitem_t *item;


  if (other == ent->owner)
    return;

  if (surf && (surf->flags & SURF_SKY))
    {
      G_FreeEdict (ent);
      return;
    }

  if (ent->owner->client)
    {
      gi.sound (ent, CHAN_WEAPON, gi.soundindex ("weapons/clank.wav"), 1,
		ATTN_NORM, 0);
      PlayerNoise (ent->owner, ent->s.origin, PNOISE_IMPACT);
    }

  // calculate position for the explosion entity
  VectorMA (ent->s.origin, -0.02, ent->velocity, origin);
//glass fx
  if (0 == Q_stricmp (other->classname, "func_explosive"))
    {
      // ignore it, so it can bounce
      return;
    }
  else
// ---        
  if (other->takedamage)
    {
      T_Damage (other, ent, ent->owner, ent->velocity, ent->s.origin,
		plane->normal, ent->dmg, 0, 0, MOD_KNIFE_THROWN);
    }
  else
    {

      // code to manage excess knives in the game, guarantees that
      // no more than knifelimit knives will be stuck in walls.  
      // if knifelimit == 0 then it won't be in effect and it can
      // start removing knives even when less than the limit are
      // out there.
      if (knifelimit->value != 0)
	{
	  knives++;

	  if (knives > knifelimit->value)
	    knives = 1;

	  knife = FindEdictByClassnum ("weapon_Knife", knives);

	  if (knife)
	    {
	      knife->nextthink = level.time + .1;
	    }

	}

      dropped = G_Spawn ();
      item = GET_ITEM(KNIFE_NUM);

      dropped->classname = item->classname;
	  dropped->typeNum = item->typeNum;
      dropped->item = item;
      dropped->spawnflags = DROPPED_ITEM;
      dropped->s.effects = item->world_model_flags;
      dropped->s.renderfx = RF_GLOW;
      VectorSet (dropped->mins, -15, -15, -15);
      VectorSet (dropped->maxs, 15, 15, 15);
      gi.setmodel (dropped, dropped->item->world_model);
      dropped->solid = SOLID_TRIGGER;
      dropped->movetype = MOVETYPE_TOSS;
      dropped->touch = Touch_Item;
      dropped->owner = ent;
      dropped->gravity = 0;
      dropped->classnum = knives;

      vectoangles (ent->velocity, move_angles);
      //AngleVectors (ent->s.angles, forward, right, up);
      VectorCopy (ent->s.origin, dropped->s.origin);
      VectorCopy (move_angles, dropped->s.angles);
      //VectorScale (forward, 100, dropped->velocity);
      //dropped->velocity[2] = 300;

      //dropped->think = drop_make_touchable;
      //dropped->nextthink = level.time + 1;

      dropped->nextthink = level.time + 120;
      dropped->think = G_FreeEdict;

      gi.linkentity (dropped);


      if (!(ent->waterlevel))
	{
	  /*gi.WriteByte (svc_temp_entity);
	     gi.WriteByte (TE_WELDING_SPARKS);
	     gi.WritePosition (origin);
	     gi.multicast (ent->s.origin, MULTICAST_PHS);
	   */
	  gi.WriteByte (svc_temp_entity);
	  gi.WriteByte (TE_SPARKS);
	  gi.WritePosition (origin);
	  gi.WriteDir (plane->normal);
	  gi.multicast (ent->s.origin, MULTICAST_PVS);
	}

    }
  G_FreeEdict (ent);
}



void
knife_throw (edict_t * self, vec3_t start, vec3_t dir, int damage, int speed)
{
  edict_t *knife;
//        vec3_t          forward, right, up;
  trace_t tr;

  knife = G_Spawn ();

  VectorNormalize (dir);
  VectorCopy (start, knife->s.origin);
  VectorCopy (start, knife->s.old_origin);
  vectoangles (dir, knife->s.angles);
  VectorScale (dir, speed, knife->velocity);
  knife->movetype = MOVETYPE_TOSS;

/*                
	VectorCopy (start, knife->s.origin);
        VectorCopy (dir, knife->movedir);
        vectoangles (dir, knife->s.angles);
        VectorScale (dir, speed, knife->velocity);
*/

  //      gi.cprintf(self, PRINT_HIGH, "speed %d\n", speed);
  // below add upwards trajectory, not in action though
  //     AngleVectors (knife->s.angles, forward, right, up);     
  //     VectorMA (knife->velocity, 210, up, knife->velocity);
  VectorSet (knife->avelocity, 1200, 0, 0);

  knife->movetype = MOVETYPE_TOSS;
  knife->clipmask = MASK_SHOT;
  knife->solid = SOLID_BBOX;
  knife->s.effects = 0;		//EF_ROTATE?
  VectorClear (knife->mins);
  VectorClear (knife->maxs);
  knife->s.modelindex = gi.modelindex ("models/objects/knife/tris.md2");
  knife->owner = self;
  knife->touch = knife_touch;
  knife->nextthink = level.time + 8000 / speed;
  knife->think = G_FreeEdict;
  knife->dmg = damage;
  knife->s.sound = gi.soundindex ("misc/flyloop.wav");
  knife->classname = "thrown_knife";

// used by dodging monsters, skip
//      if (self->client)
//              check_dodge (self, rocket->s.origin, dir, speed);

  PRETRACE ();
  tr =
    gi.trace (self->s.origin, NULL, NULL, knife->s.origin, knife, MASK_SHOT);
  POSTTRACE ();
  if (tr.fraction < 1.0)
    {
      VectorMA (knife->s.origin, -10, dir, knife->s.origin);
      knife->touch (knife, tr.ent, NULL, NULL);
    }


  gi.linkentity (knife);
}


/*
=====================================================================
setFFState: Save team wound count & warning state before an attack

The purpose of this is so that we can increment team_wounds by 1 for
each real attack instead of just counting each bullet/pellet/shrapnel
as a wound. The ff_warning flag is so that we don't overflow the
clients from repeated FF warnings. Hopefully the overhead on this 
will be low enough to not affect things.
=====================================================================
*/
void setFFState (edict_t * ent)
{
	if (ent && ent->client)
	{
		ent->client->team_wounds_before = ent->client->team_wounds;
		ent->client->ff_warning = 0;
	}
}
