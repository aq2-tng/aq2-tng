//-----------------------------------------------------------------------------
// a_ctf.c
// CTF Flag functions
//-----------------------------------------------------------------------------
//
// $Id: a_ctf.c,v 1.7 2001/05/14 16:36:00 mort Exp $
//
//-----------------------------------------------------------------------------
// $Log: a_ctf.c,v $
// Revision 1.7  2001/05/14 16:36:00  mort
// Frags for returning flag
//
// Revision 1.6  2001/05/13 11:24:31  igor_rock
// Added the CVS Headers
//
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "a_ctf.h"

// Locations
vec3_t redFlagLocation;
vec3_t blueFlagLocation;

// Taken flags
int redFlagTaken = 0;
int blueFlagTaken = 0;

// Flag entitys
edict_t *redFlag;
edict_t *blueFlag;
// Spawn entitys
edict_t *closestRedSpawns[3]; // AQ2:M - CTF
edict_t *closestBlueSpawns[3]; // AQ2:M - CTF

// Started flag
int started = 0;

// Have we loaded the flags?
int loadedFlags = 0;

// Hacked spawn flag
int hackedSpawns = 0;

// Flag animation
int flagFrames[] = 
{ 173, 174, 175, 176, 177, 178, 179, 180, 181, 182, 183, 184, 185, 186 };
int firstFlagFrame = 173;
int lastFlagFrame = 186;

// droptofloor delcaration
void droptofloor (edict_t *ent);

// Flag think function
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

				other->client->resp.score += 2;
		
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

				other->client->resp.score += 2;


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
	cvar_t  *game;

	if(ctf->value)
	{
		redFlagTaken = 0;
		blueFlagTaken = 0;

		game = gi.cvar("game", "", 0);

		if(!*game->string)
			sprintf(locfile, "%s/tng/%s.flg", GAMEVERSION, mapname);
		else
			sprintf(locfile, "%s/tng/%s.flg", game->string, mapname);

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

		loadedFlags = 1;
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

void ForceSpawn(edict_t *ent) // AQ2:M - CTF
{
	// AQ2:M - CTF
	// Spawn straight away - make sure they ent alive already too :P
	if(ctf->value)
	{
		if(started)
		{
			if(ent->hasSpawned == 0)
			{
				ent->client->resp.last_killed_target = NULL;
				ent->client->resp.last_damaged_part = 0;
				ent->hasSpawned = 1;

				PutClientInServer(ent);
				AddToTransparentList(ent);

				ent->solid = SOLID_TRIGGER; // For trigger stuff..
				ent->client->respawn_time = level.time + 2; // 2 seconds "not-solid"
			}
		}
	}
//	if((ctf->value && started && (ent->deadflag || ent->client->pers.spectator)) || (!ent->hasSpawned && started))
//	{
//	}
}


void SVCmd_MoveFlag()
{
	// Move flag command...
	char *var;
	float pos;
    char name[MAX_OSPATH];
	FILE *f;
	cvar_t *game;

	if(gi.argc() != 6)
	{
		gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"k_moveflag\"\n");
		return;
    }

	var = gi.argv(2);

	if(Q_stricmp (var, "red") == 0)
	{
		// Red flag...
		var = gi.argv(3);
		pos = atof(var);
		redFlagLocation[0] = pos;
		var = gi.argv(4);
		pos = atof(var);
		redFlagLocation[1] = pos;
		var = gi.argv(5);
		pos = atof(var);
		redFlagLocation[2] = pos;

		if(redFlag)
			if(redFlag->inuse)
				G_FreeEdict(redFlag);

		SpawnRedFlag();
	}
	else if(Q_stricmp (var, "blue") == 0)
	{
		var = gi.argv(3);
		pos = atof(var);
		blueFlagLocation[0] = pos;
		var = gi.argv(4);
		pos = atof(var);
		blueFlagLocation[1] = pos;
		var = gi.argv(5);
		pos = atof(var);
		blueFlagLocation[2] = pos;

		if(blueFlag)
			if(blueFlag->inuse)
				G_FreeEdict(blueFlag);

		SpawnBlueFlag();
	}
	else
	{
		gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"k_moveflag\"\n");
		return;
	}

	// Write the file.
    game = gi.cvar("game", "", 0);

    if (!*game->string)
		sprintf (name, "%s/tng/%s.flg", GAMEVERSION, level.mapname);
    else
		sprintf (name, "%s/tng/%s.flg", game->string, level.mapname);

    gi.cprintf (NULL, PRINT_HIGH, "Writing %s.\n", name);

    f = fopen (name, "w");
    if (!f)
    {
		gi.cprintf (NULL, PRINT_HIGH, "Couldn't open %s\n", name);
        return;
    }

	fprintf (f, "%d %d %d %d %d %d\n", 
		(int)redFlagLocation[0],
		(int)redFlagLocation[1],
		(int)redFlagLocation[2],
		(int)blueFlagLocation[0],
		(int)blueFlagLocation[1],
		(int)blueFlagLocation[2]);

	if(hackedSpawns)
	{
		int n = 0;
		while(n < 3)
		{
			if(closestRedSpawns[n])
			{
				if(closestRedSpawns[n]->inuse)
				{
					fprintf (f, "%d %d %d\n",
						closestRedSpawns[n]->s.origin[0],
						closestRedSpawns[n]->s.origin[1],
						closestRedSpawns[n]->s.origin[2]);

					n++;
					continue;
				}
			}

			fprintf (f, "#\n");
			n++;
		}


		n = 0;
		while(n < 3)
		{
			if(closestBlueSpawns[n])
			{
				if(closestBlueSpawns[n]->inuse)
				{
					fprintf (f, "%d %d %d\n",
						closestBlueSpawns[n]->s.origin[0],
						closestBlueSpawns[n]->s.origin[1],
						closestBlueSpawns[n]->s.origin[2]);
					
					n++;
					continue;
				}
			}

			fprintf (f, "#\n");
			n++;
		}
	}

	fclose (f);
}

void SVCmd_MoveSpawn()
{
	// Move flag command...
	char *var;
    char name[MAX_OSPATH];
	FILE *f;
	int spawn;
	cvar_t *game;

	if(gi.argc() != 7)
	{
		gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"k_movespawn\"\n");
		return;
    }

	var = gi.argv(2);

	if(Q_stricmp (var, "red") == 0)
	{
		// Red spawn...
		edict_t *ent;

		var = gi.argv(3);
		spawn = atoi(var);

		if(spawn > 2 || spawn < 0)
		{
			gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"k_movespawn\"\n");
			return;
 		}

		ent = G_Spawn();
		ent->s.skinnum = 0;
		ent->solid = SOLID_NOT;
		VectorSet (ent->mins, 0, 0, 0);
		VectorSet (ent->maxs, 0, 0, 0);

		var = gi.argv(4);
		ent->s.origin[0] = atoi(var);
		var = gi.argv(5);
		ent->s.origin[1] = atoi(var);
		var = gi.argv(6);
		ent->s.origin[2] = atoi(var);

		closestRedSpawns[spawn] = ent;

		gi.linkentity(ent);
	}
	else if(Q_stricmp (var, "blue") == 0)
	{
		// Blue spawn...
		edict_t *ent;

		var = gi.argv(3);
		spawn = atoi(var);

		if(spawn > 2 || spawn < 0)
		{
			gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"k_movespawn\"\n");
			return;
 		}

		ent = G_Spawn();
		ent->s.skinnum = 0;
		ent->solid = SOLID_NOT;
		VectorSet (ent->mins, 0, 0, 0);
		VectorSet (ent->maxs, 0, 0, 0);

		var = gi.argv(4);
		ent->s.origin[0] = atoi(var);
		var = gi.argv(5);
		ent->s.origin[1] = atoi(var);
		var = gi.argv(6);
		ent->s.origin[2] = atoi(var);

		closestBlueSpawns[spawn] = ent;

		gi.linkentity(ent);
	}
	else
	{
		gi.cprintf (NULL, PRINT_HIGH, "Unknown server command \"k_movespawn\"\n");
		return;
	}

	// Write the file.
    game = gi.cvar("game", "", 0);

    if (!*game->string)
		sprintf (name, "%s/tng/%s.flg", GAMEVERSION, level.mapname);
    else
		sprintf (name, "%s/tng/%s.flg", game->string, level.mapname);

    gi.cprintf (NULL, PRINT_HIGH, "Writing %s.\n", name);

    f = fopen (name, "w");
    if (!f)
    {
		gi.cprintf (NULL, PRINT_HIGH, "Couldn't open %s\n", name);
        return;
    }

	fprintf (f, "%d %d %d %d %d %d\n", 
		(int)redFlagLocation[0],
		(int)redFlagLocation[1],
		(int)redFlagLocation[2],
		(int)blueFlagLocation[0],
		(int)blueFlagLocation[1],
		(int)blueFlagLocation[2]);

	hackedSpawns = 1;

	if(hackedSpawns)
	{
		int n = 0;
		while(n < 3)
		{
			if(closestRedSpawns[n])
			{
				if(closestRedSpawns[n]->inuse)
				{
					fprintf (f, "%d %d %d\n",
						(int)closestRedSpawns[n]->s.origin[0],
						(int)closestRedSpawns[n]->s.origin[1],
						(int)closestRedSpawns[n]->s.origin[2]);

					n++;
					continue;
				}
			}

			fprintf (f, "#\n");
			n++;
		}


		n = 0;
		while(n < 3)
		{
			if(closestBlueSpawns[n])
			{
				if(closestBlueSpawns[n]->inuse)
				{
					fprintf (f, "%d %d %d\n",
						(int)closestBlueSpawns[n]->s.origin[0],
						(int)closestBlueSpawns[n]->s.origin[1],
						(int)closestBlueSpawns[n]->s.origin[2]);
					
					n++;
					continue;
				}
			}

			fprintf (f, "#\n");
			n++;
		}
	}

	fclose (f);
}

void SVCmd_RemoveSpawns()
{
    char name[MAX_OSPATH];
	FILE *f;
	cvar_t *game;

	// Write the file.
    game = gi.cvar("game", "", 0);

    if (!*game->string)
		sprintf (name, "%s/tng/%s.flg", GAMEVERSION, level.mapname);
    else
		sprintf (name, "%s/tng/%s.flg", game->string, level.mapname);

    gi.cprintf (NULL, PRINT_HIGH, "Writing %s.\n", name);

    f = fopen (name, "w");
    if (!f)
    {
		gi.cprintf (NULL, PRINT_HIGH, "Couldn't open %s\n", name);
        return;
    }

	fprintf (f, "%d %d %d %d %d %d\n", 
		(int)redFlagLocation[0],
		(int)redFlagLocation[1],
		(int)redFlagLocation[2],
		(int)blueFlagLocation[0],
		(int)blueFlagLocation[1],
		(int)blueFlagLocation[2]);

	hackedSpawns = 0;

	fclose (f);

	SetupTeamSpawnPoints();
}
