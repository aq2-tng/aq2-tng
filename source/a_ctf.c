// CTF Flag functions

#include "g_local.h"
#include "a_ctf.h"

//AQ2:M CTF
vec3_t redFlagLocation;
vec3_t blueFlagLocation;

int redFlagTaken = 0;
int blueFlagTaken = 0;

edict_t *redFlag;
edict_t *blueFlag;

int flagFrames[] = 
{ 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186 };

int firstFlagFrame = 173;
int lastFlagFrame = 186;

void droptofloor (edict_t *ent);

void flagThink(edict_t *self)
{
	self->s.frame++;
	if(self->s.frame > lastFlagFrame)
		self->s.frame = firstFlagFrame;

	self->nextthink = level.time + FRAMETIME;
}

void returnFlag(edict_t *self)
{
	if(self->item->quantity == 1) // red flag
	{
		if(redFlagTaken)
		{
			G_FreeEdict(self);
			SpawnRedFlag();

			gi.sound(redFlag, CHAN_FLAG | CHAN_RELIABLE, gi.soundindex("ctf/flagret.wav"), 1, ATTN_NONE, 0);
			gi.bprintf(PRINT_HIGH, "The red flag returned!\n");

			redFlagTaken = 0;
		}
	}
	else  // blue flag
	{
		if(blueFlagTaken)
		{
			G_FreeEdict(self);
			SpawnBlueFlag();

			gi.sound(blueFlag, CHAN_FLAG | CHAN_RELIABLE, gi.soundindex("ctf/flagret.wav"), 1, ATTN_NONE, 0);
			gi.bprintf(PRINT_HIGH, "The blue flag returned!\n");

			blueFlagTaken = 0;
		}
	}
}

void dropFlag(edict_t *self)
{
	droptofloor(self);
	self->think = flagThink;
	self->nextthink = level.time + FRAMETIME;
	self->touch = flagTouch;
}

void flagTouch(edict_t *self, edict_t *other, cplane_t *plane, csurface_t *surf)
{
	if(!other->client)
	{
		return;
	}
	if(other->health < 1)
	{
		return;
	}

	if(self->item->quantity == other->client->resp.team) // Use quantity as the team..
	{
		if(self->item->quantity == 1) // red flag
		{
			if(redFlagTaken)
			{
				G_FreeEdict(self);
				SpawnRedFlag();

				gi.sound(redFlag, CHAN_FLAG | CHAN_RELIABLE, gi.soundindex("ctf/flagret.wav"), 1, ATTN_NONE, 0);
				gi.bprintf(PRINT_HIGH, "%s returned the red flag!\n", other->client->pers.netname);
			
				redFlagTaken = 0;
			}
		}
		else // blue flag
		{
			if(blueFlagTaken)
			{
				G_FreeEdict(self);
				SpawnBlueFlag();

				gi.sound(blueFlag, CHAN_FLAG | CHAN_RELIABLE, gi.soundindex("ctf/flagret.wav"), 1, ATTN_NONE, 0);
				gi.bprintf(PRINT_HIGH, "%s returned the blue flag!\n", other->client->pers.netname);

				blueFlagTaken = 0;
			}
		}

		return;
	}

	other->s.modelindex3 = self->s.modelindex;

	if(self->item->quantity == 1)
	{
		gi.bprintf(PRINT_HIGH, "%s took the red flag!\n", other->client->pers.netname);
		redFlagTaken = 1;
	}
	else
	{
		gi.bprintf(PRINT_HIGH, "%s took the blue flag!\n", other->client->pers.netname);
		blueFlagTaken = 1;
	}

	// Do the usual touch item code...
	Touch_Item(self, other, plane, surf);
}

void checkForCap(edict_t *ent)
{
	if(!(ent->client->pers.inventory[ITEM_INDEX(FindItem("Blue Flag"))] ||
		ent->client->pers.inventory[ITEM_INDEX(FindItem("Red Flag"))]))
		return;

	if(ent->client->resp.team == 1) // red
	{
		if(!redFlagTaken) // Can't capture when enemy has your flag
		{
			float dist;
			float t; // temp
			dist = 0;

			t = ent->s.origin[0] - redFlagLocation[0];
			t *= t;
			dist += t;

			t = ent->s.origin[1] - redFlagLocation[1];
			t *= t;
			dist += t;

			t = ent->s.origin[2] - redFlagLocation[2];
			t *= t;
			dist += t;

			if(dist < 2000)
			{
				gi.bprintf(PRINT_HIGH, "%s captured the blue flag!\n", ent->client->pers.netname);
						// effect
 				if(ctf_effects->value)
 				{
					ent->s.event = EV_PLAYER_TELEPORT;	
				}
				blueFlagTaken = 0;

				// Sound the flag capturing
				gi.sound(ent, CHAN_FLAG | CHAN_RELIABLE, gi.soundindex("ctf/flagcap.wav"), 1, ATTN_NONE, 0);

				ent->s.modelindex3 = 0;
				ent->client->pers.inventory[ITEM_INDEX(FindItem("Blue Flag"))] = 0;

				// Respawn flag
				SpawnBlueFlag();

				team1_score++;
				
				ent->client->resp.score += 5;
			}
		}
	}
	else if(ent->client->resp.team == 2) // blue
	{
		if(!blueFlagTaken) // Can't capture when enemy has your flag
		{
			float dist;
			float t; // temp
			dist = 0;

			t = ent->s.origin[0] - blueFlagLocation[0];
			t *= t;
			dist += t;

			t = ent->s.origin[1] - blueFlagLocation[1];
			t *= t;
			dist += t;

			t = ent->s.origin[2] - blueFlagLocation[2];
			t *= t;
			dist += t;

			if(dist < 2000)
			{
				gi.bprintf(PRINT_HIGH, "%s captured the red flag!\n", ent->client->pers.netname);
					// effect
 				if(ctf_effects->value)
 				{
					ent->s.event = EV_PLAYER_TELEPORT;	
				}
				redFlagTaken = 0;

				// Sound the flag capturing
				gi.sound(ent, CHAN_FLAG | CHAN_RELIABLE, gi.soundindex("ctf/flagcap.wav"), 1, ATTN_NONE, 0);

				ent->s.modelindex3 = 0;
				ent->client->pers.inventory[ITEM_INDEX(FindItem("Red Flag"))] = 0;

				SpawnRedFlag();

				team2_score++;

				ent->client->resp.score += 5;
			}
		}
	}

}

edict_t* SpawnFlag()
{
	edict_t *ent = G_Spawn();
	
	ent->owner = NULL;
	ent->s.angles[PITCH] = 0;
	ent->s.angles[ROLL] = 0;
	ent->s.angles[YAW] = 0;
	ent->movetype = MOVETYPE_NONE;
	ent->clipmask = MASK_SHOT;
	ent->solid = SOLID_TRIGGER;
	ent->s.frame = firstFlagFrame;
	ent->think = dropFlag;
	ent->nextthink = level.time + FRAMETIME;
	ent->touch = flagTouch;

	return ent;
}

edict_t* SpawnRedFlag()
{
	edict_t *ent = SpawnFlag();

	ent->s.modelindex = gi.modelindex("models/flags/flag1.md2");
	_VectorCopy(redFlagLocation, ent->s.origin);
	ent->classname = "redFlag";
	ent->item = FindItem("Red Flag");

	if(ctf_effects->value)
		ent->s.effects = EF_FLAG1;

	gi.linkentity(ent);

	redFlag = ent;

	return ent;
}

edict_t* SpawnBlueFlag()
{
	edict_t *ent = SpawnFlag();

	ent->s.modelindex = gi.modelindex("models/flags/flag2.md2");
	_VectorCopy(blueFlagLocation, ent->s.origin);
	ent->classname = "blueFlag";
	ent->item = FindItem("Blue Flag");

	if(ctf_effects->value)
		ent->s.effects = EF_FLAG2;

	gi.linkentity(ent);

	blueFlag = ent;

	return ent;
}

void spawnFlags(char *mapname)
{
	char locfile[256];
	char line[256];
	FILE *f;
	char *param;

	if(ctf->value)
	{
		sprintf(locfile, "action/aqm/%s.flg", mapname); // Re-using locfile... save's a bit of memory :)

		f = fopen(locfile, "rt");
		if(!f)
		{
			gi.dprintf("No flag location file for %s\n",mapname);

			redFlagLocation[0] = 0;
			redFlagLocation[1] = 0;
			redFlagLocation[2] = 0;
			blueFlagLocation[0] = 0;
			blueFlagLocation[1] = 0;
			blueFlagLocation[2] = 0;

			// Spawn the two flags...
			SpawnRedFlag();
			SpawnBlueFlag();
		}
		else
		{
			fgets(line, 256, f);

			param = strtok(line, " ,:\n\0");
			redFlagLocation[0] = atoi(param);
	
			param = strtok(NULL, " ,:\n\0");
			redFlagLocation[1] = atoi(param);

			param = strtok(NULL, " ,:\n\0");
			redFlagLocation[2] = atoi(param);

			param = strtok(NULL, " ,:\n\0");
			blueFlagLocation[0] = atoi(param);

			param = strtok(NULL, " ,:\n\0");
			blueFlagLocation[1] = atoi(param);

			param = strtok(NULL, " ,:\n\0");
			blueFlagLocation[2] = atoi(param);

			// Spawn the two flags...
			SpawnRedFlag();
			SpawnBlueFlag();
		}
	}

}

edict_t *Drop_Item (edict_t *ent, gitem_t *item);

void DropFlag ( edict_t* ent )
{
	if(ent->client->pers.inventory[ITEM_INDEX(FindItem("Blue Flag"))])
	{
		ent->client->pers.inventory[ITEM_INDEX(FindItem("Blue Flag"))] = 0;
		blueFlag = Drop_Item (ent, FindItem("Blue Flag"));
		ent->s.modelindex3 = 0;
	}
	if(ent->client->pers.inventory[ITEM_INDEX(FindItem("Red Flag"))])
	{
		ent->client->pers.inventory[ITEM_INDEX(FindItem("Red Flag"))] = 0;
		redFlag = Drop_Item (ent, FindItem("Red Flag"));
		ent->s.modelindex3 = 0;
	}
}