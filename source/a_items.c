//-----------------------------------------------------------------------------
// Special item spawning/management code.  Mainly hacked from CTF, thanks
// Zoid.
// - zucc
//
// $Id: a_items.c,v 1.3 2001/05/15 15:49:14 igor_rock Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_items.c,v $
// Revision 1.3  2001/05/15 15:49:14  igor_rock
// added itm_flags for deathmatch
//
// Revision 1.2  2001/05/11 16:07:25  mort
// Various CTF bits and pieces...
//
// Revision 1.1.1.1  2001/05/06 17:24:25  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"



// time too wait between failures to respawn?
#define SPEC_RESPAWN_TIME       60
// time before they will get respawned
#define SPEC_TECH_TIMEOUT       60

char *tnames[] =
{
  "item_quiet", "item_slippers", "item_vest", "item_band", "item_lasersight", "item_helmet",
  NULL
};


void SpecThink (edict_t * spec);


static edict_t *
FindSpecSpawn (void)
{
  edict_t *spot = NULL;
  int i = rand () % 16;

  while (i--)
    spot = G_Find (spot, FOFS (classname), "info_player_deathmatch");
  if (!spot)
    spot = G_Find (spot, FOFS (classname), "info_player_deathmatch");

  if (spot == NULL)
    {
/* FB  6/24/99
   gi.dprintf("Warning: failed to find special item spawn point!\n");
 */
    }

  return spot;
}



static void
SpawnSpec (gitem_t * item, edict_t * spot)
{
  edict_t *ent;
  vec3_t forward, right;
  vec3_t angles;

  ent = G_Spawn ();

  ent->classname = item->classname;
  ent->item = item;
  ent->spawnflags = DROPPED_ITEM;
  ent->s.effects = item->world_model_flags;
  ent->s.renderfx = RF_GLOW;
  VectorSet (ent->mins, -15, -15, -15);
  VectorSet (ent->maxs, 15, 15, 15);
  // zucc dumb hack to make laser look like it is on the ground
  if (stricmp (item->pickup_name, LASER_NAME) == 0)
    {
      VectorSet (ent->mins, -15, -15, -1);
      VectorSet (ent->maxs, 15, 15, 1);
    }
  gi.setmodel (ent, ent->item->world_model);
  ent->solid = SOLID_TRIGGER;
  ent->movetype = MOVETYPE_TOSS;
  ent->touch = Touch_Item;
  ent->owner = ent;

  angles[0] = 0;
  angles[1] = rand () % 360;
  angles[2] = 0;

  AngleVectors (angles, forward, right, NULL);
  VectorCopy (spot->s.origin, ent->s.origin);
  ent->s.origin[2] += 16;
  VectorScale (forward, 100, ent->velocity);
  ent->velocity[2] = 300;

  ent->nextthink = level.time + SPEC_RESPAWN_TIME;
  ent->think = SpecThink;

  gi.linkentity (ent);
}

void
SpawnSpecs (edict_t * ent)
{
  gitem_t *spec;
  edict_t *spot;
  int i;
  i = 0;
  while (tnames[i])
    {
      if ((spec = FindItemByClassname (tnames[i])) != NULL &&
	  (spot = FindSpecSpawn ()) != NULL)
	{
	  //AQ2:TNG - Igor adding itm_flags
	  if (
	      (((int)itm_flags->value & ITF_SIL) && (strcmp(tnames[i], "item_quiet") == 0)) || 
	      (((int)itm_flags->value & ITF_SLIP) && (strcmp(tnames[i], "item_slippers") == 0)) ||
	      (((int)itm_flags->value & ITF_KEV) && (strcmp(tnames[i], "item_vest") == 0)) ||
	      (((int)itm_flags->value & ITF_BAND) && (strcmp(tnames[i], "item_band") == 0)) ||
	      (((int)itm_flags->value & ITF_LASER) && (strcmp(tnames[i], "item_lasersight") == 0)) ||
	      (((int)itm_flags->value & ITF_HELM) && (strcmp(tnames[i], "item_helmet") == 0))
	      )
	    {
	      //gi.dprintf("Spawning special item '%s'.\n", tnames[i]);
	      SpawnSpec (spec, spot);
	    }
	  //AQ2:TNG End adding itm_flags
	}
      i++;
    }
}


void
SpecThink (edict_t * spec)
{
  edict_t *spot;

  if ((spot = FindSpecSpawn ()) != NULL)
    {
      SpawnSpec (spec->item, spot);
      G_FreeEdict (spec);
    }
  else
    {
      spec->nextthink = level.time + SPEC_RESPAWN_TIME;
      spec->think = SpecThink;
    }
}


static void
MakeTouchSpecThink (edict_t * ent)
{
  ent->touch = Touch_Item;

  if (deathmatch->value && !teamplay->value && !allitem->value)
    {
      ent->nextthink = level.time + SPEC_RESPAWN_TIME - 1;
      ent->think = SpecThink;
    }
  else if (teamplay->value && !allitem->value)
    {
	  // AQ2:M - CTF
	  if(ctf->value) // AQ2:M - CTF
		ent->nextthink = level.time + ctf_item_remove_time->value; // AQ2:M - CTF
	  else
		ent->nextthink = level.time + 60;
      ent->think = G_FreeEdict;
    }
  else				// allitem->value is set

    {
      ent->nextthink = level.time + 1;
      ent->think = G_FreeEdict;
    }
}




void
Drop_Spec (edict_t * ent, gitem_t * item)
{
  edict_t *spec;

  spec = Drop_Item (ent, item);
  //gi.cprintf(ent, PRINT_HIGH, "Dropping special item.\n");
  spec->nextthink = level.time + 1;
  spec->think = MakeTouchSpecThink;
  //zucc this and the one below should probably be -- not = 0, if
  // a server turns on multiple item pickup.
  ent->client->pers.inventory[ITEM_INDEX (item)]--;
}

void
DeadDropSpec (edict_t * ent)
{
  gitem_t *spec;
  edict_t *dropped;
  int i;

  i = 0;
  while (tnames[i])
    {
      if ((spec = FindItemByClassname (tnames[i])) != NULL &&
	  ent->client->pers.inventory[ITEM_INDEX (spec)])
	{
	  dropped = Drop_Item (ent, spec);
	  // hack the velocity to make it bounce random
	  dropped->velocity[0] = (rand () % 600) - 300;
	  dropped->velocity[1] = (rand () % 600) - 300;
	  dropped->nextthink = level.time + 1;
	  dropped->think = MakeTouchSpecThink;
	  dropped->owner = NULL;
	  ent->client->pers.inventory[ITEM_INDEX (spec)] = 0;
	}
      i++;
    }
}


// frees the passed edict!
void
RespawnSpec (edict_t * ent)
{
  edict_t *spot;

  if ((spot = FindSpecSpawn ()) != NULL)
    SpawnSpec (ent->item, spot);
  G_FreeEdict (ent);
}

void
SetupSpecSpawn (void)
{
  edict_t *ent;

  if (level.specspawn)
    return;

  //gi.bprintf (PRINT_HIGH, "got into the setup\n");

  ent = G_Spawn ();
  ent->nextthink = level.time + 4;
  ent->think = SpawnSpecs;
  level.specspawn = 1;
}
