//-----------------------------------------------------------------------------
// Special item spawning/management code.  Mainly hacked from CTF, thanks
// Zoid.
// - zucc
//
// $Id: a_items.c,v 1.7 2003/02/10 02:12:25 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_items.c,v $
// Revision 1.7  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
// Revision 1.6  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.5  2001/07/18 15:19:11  slicerdw
// Time for weapons and items dissapearing is set to "6" to prevent lag on ctf
//
// Revision 1.4  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.3.2.1  2001/05/20 15:17:31  igor_rock
// removed the old ctf code completly
//
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

int tnums[ITEM_COUNT] = {
	SIL_NUM, SLIP_NUM, BAND_NUM, KEV_NUM, LASER_NUM,
	HELM_NUM
};

//int tnumspawned[ITEM_COUNT] = { 0 };

void SpecThink(edict_t * spec);

static edict_t *FindSpecSpawn(void)
{
	edict_t *spot = NULL;
	int i = rand() % 16;

	while (i--)
		spot = G_Find(spot, FOFS(classname), "info_player_deathmatch");
	if (!spot)
		spot = G_Find(spot, FOFS(classname), "info_player_deathmatch");

	return spot;
}

static void SpawnSpec(gitem_t * item, edict_t * spot)
{
	edict_t *ent;
	vec3_t forward, right, angles;

	ent = G_Spawn();
	ent->classname = item->classname;
	ent->typeNum = item->typeNum;
	ent->item = item;
	ent->spawnflags = DROPPED_ITEM;
	ent->s.effects = item->world_model_flags;
	ent->s.renderfx = RF_GLOW;
	VectorSet(ent->mins, -15, -15, -15);
	VectorSet(ent->maxs, 15, 15, 15);
	// zucc dumb hack to make laser look like it is on the ground
	if (item->typeNum == LASER_NUM) {
		VectorSet(ent->mins, -15, -15, -1);
		VectorSet(ent->maxs, 15, 15, 1);
	}
	gi.setmodel(ent, ent->item->world_model);
	ent->solid = SOLID_TRIGGER;
	ent->movetype = MOVETYPE_TOSS;
	ent->touch = Touch_Item;
	ent->owner = ent;

	angles[0] = 0;
	angles[1] = rand() % 360;
	angles[2] = 0;

	AngleVectors(angles, forward, right, NULL);
	VectorCopy(spot->s.origin, ent->s.origin);
	ent->s.origin[2] += 16;
	VectorScale(forward, 100, ent->velocity);
	ent->velocity[2] = 300;

	ent->nextthink = level.time + SPEC_RESPAWN_TIME;
	ent->think = SpecThink;

	gi.linkentity(ent);
}

void SpawnSpecs(edict_t * ent)
{
	gitem_t *spec;
	edict_t *spot;
	int i;

	if(item_respawnmode->value)
		return;

	for(i = 0; i<ITEM_COUNT; i++)
	{
		if ((spec = GET_ITEM(tnums[i])) != NULL && (spot = FindSpecSpawn()) != NULL) {
			//AQ2:TNG - Igor adding itm_flags
			if ((int)itm_flags->value & items[tnums[i]].flag)
			{
				//gi.dprintf("Spawning special item '%s'.\n", tnames[i]);
				SpawnSpec(spec, spot);
			}
			//AQ2:TNG End adding itm_flags
		}
	}
}

void SpecThink(edict_t * spec)
{
	edict_t *spot;

	if ((spot = FindSpecSpawn()) != NULL) {
		SpawnSpec(spec->item, spot);
		G_FreeEdict(spec);
	} else {
		spec->nextthink = level.time + SPEC_RESPAWN_TIME;
		spec->think = SpecThink;
	}
}

static void MakeTouchSpecThink(edict_t * ent)
{
	ent->touch = Touch_Item;

	if (deathmatch->value && (!teamplay->value || teamdm->value || ctf->value == 2) && !allitem->value) {
		if(item_respawnmode->value) {
			ent->nextthink = level.time + (item_respawn->value*0.5f);
			ent->think = G_FreeEdict;
		}
		else {
			ent->nextthink = level.time + item_respawn->value;
			ent->think = SpecThink;
		}
	} else if (teamplay->value && !allitem->value) {
		//AQ2:TNG - Slicer This works for Special Items 
		if (ctf->value) {
			ent->nextthink = level.time + 6;
			ent->think = G_FreeEdict;
		} else {
			ent->nextthink = level.time + 60;
			ent->think = G_FreeEdict;
		}
	} else			// allitem->value is set

	{
		ent->nextthink = level.time + 1;
		ent->think = G_FreeEdict;
	}
}

void Drop_Spec(edict_t * ent, gitem_t * item)
{
	edict_t *spec;

	spec = Drop_Item(ent, item);
	//gi.cprintf(ent, PRINT_HIGH, "Dropping special item.\n");
	spec->nextthink = level.time + 1;
	spec->think = MakeTouchSpecThink;
	//zucc this and the one below should probably be -- not = 0, if
	// a server turns on multiple item pickup.
	ent->client->pers.inventory[ITEM_INDEX(item)]--;
}

void DeadDropSpec(edict_t * ent)
{
	gitem_t *spec;
	edict_t *dropped;
	int i;

	for(i = 0; i<ITEM_COUNT; i++)
	{
		if (INV_AMMO(ent, tnums[i]) > 0) {
			spec = GET_ITEM(tnums[i]);
			dropped = Drop_Item(ent, spec);
			// hack the velocity to make it bounce random
			dropped->velocity[0] = (rand() % 600) - 300;
			dropped->velocity[1] = (rand() % 600) - 300;
			dropped->nextthink = level.time + 1;
			dropped->think = MakeTouchSpecThink;
			dropped->owner = NULL;
			dropped->spawnflags = DROPPED_PLAYER_ITEM;
			ent->client->pers.inventory[ITEM_INDEX(spec)] = 0;
		}
	}
}

// frees the passed edict!
void RespawnSpec(edict_t * ent)
{
	edict_t *spot;

	if ((spot = FindSpecSpawn()) != NULL)
		SpawnSpec(ent->item, spot);
	G_FreeEdict(ent);
}

void SetupSpecSpawn(void)
{
	edict_t *ent;

	if (level.specspawn)
		return;

	ent = G_Spawn();
	ent->nextthink = level.time + 4;
	ent->think = SpawnSpecs;
	level.specspawn = 1;
}
