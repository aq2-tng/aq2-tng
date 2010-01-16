#include "g_local.h"

/* used the lasersight code for reference for the live mouse move */

edict_t* Blox_CreatePlatform(qboolean solid)
{
	edict_t *ent;
	ent = G_Spawn();

	if(solid) {
		ent->movetype = MOVETYPE_NONE;
		ent->solid = SOLID_BBOX;
	} else {
		ent->movetype = MOVETYPE_NOCLIP;
		ent->solid = SOLID_NOT;
		ent->s.renderfx = RF_TRANSLUCENT;
	}

	ent->classname = "blox_platform";
	ent->model = "models/objects/dmspot/tris.md2";

	VectorSet (ent->mins, -32, -32, -24);
	VectorSet (ent->maxs, 32, 32, -16);

	gi.setmodel(ent, ent->model);

	return ent;
}

// change block origin relatively
void Blox_MoveActive(edict_t *self, float x, float y, float z)
{
	if(self->blox) {
		self->blox->s.origin[0] += x;
		self->blox->s.origin[1] += z;
		self->blox->s.origin[2] += y;
	}
}

void Blox_MakeActive(edict_t * ent)
{
	vec3_t start, end, endp, offset;
	vec3_t forward, right, up, angles;
	trace_t tr;
	float bd = 0, d;
	int i;


	/* if we are dragging a block, don't mind */
	if(ent->blox && ent->blox->think == Blox_MoveThink) {
		Blox_Lock(ent);
		return;
	}

	VectorAdd(ent->client->v_angle, ent->client->kick_angles, angles);
	AngleVectors(angles, forward, right, up);

	VectorSet(offset, 24, 8, ent->viewheight);
	P_ProjectSource(ent->client, ent->s.origin, offset, forward, right, start);
	VectorMA(start, 8192, forward, end);

	PRETRACE ();
	tr = gi.trace(start, NULL, NULL, end, ent, CONTENTS_MONSTER);
	POSTTRACE ();

	if (tr.fraction < 1 && tr.ent && tr.ent->client)
	{
		return;
	}

	if(tr.ent->solid == SOLID_BBOX && strcmp(tr.ent->classname , "blox_platform") == 0) {
		if(ent->blox && ent->blox != tr.ent) {
			ent->blox->think = Blox_SolidThink;
			ent->blox->nextthink = level.time + 0.01;
		}
		tr.ent->s.effects |= EF_COLOR_SHELL;
		tr.ent->s.renderfx |= RF_SHELL_GREEN;
		ent->blox = tr.ent;
	} else if(ent->blox) {
		ent->blox->think = Blox_SolidThink;
		ent->blox->nextthink = level.time + 0.01;
		ent->blox = NULL;
	}
}

void Blox_Create(edict_t * self)
{
	if(self->blox)
		Blox_Lock(self);

	self->blox = Blox_CreatePlatform(false);
	self->blox->owner = self;
	self->blox->think = Blox_MoveThink;
	self->blox->nextthink = level.time + 0.01;
	VectorCopy(self->s.origin, self->blox->s.origin);
	gi.linkentity(self->blox);
}

void Blox_Destroy(edict_t * self)
{
	if(self->blox) {
		G_FreeEdict(self->blox);
		self->blox = NULL;
	}
}

void Blox_Lock(edict_t * self)
{
	edict_t *tmp;
	if(self->blox) {
		if(self->blox->think == Blox_MoveThink) {
			// for some wicked reason, you can't make a SOLID_NOT entity a SOLID_BBOX
			tmp = Blox_CreatePlatform(true);
			VectorCopy(self->blox->s.origin, tmp->s.origin);
			G_FreeEdict(self->blox);
			gi.linkentity(tmp);
			self->blox = NULL;
		} else {
			self->blox->think = Blox_SolidThink;
			self->blox->nextthink = level.time + 0.01;
		}
	}
}

/* solid blocks only think of removing their effect */
void Blox_SolidThink(edict_t * self)
{
	self->s.effects = 0;
	self->s.renderfx = 0;
	self->nextthink = 0;
	self->think = G_FreeEdict;
	gi.linkentity(self);
}

/* moving blocks follow the players aim */
void Blox_MoveThink(edict_t * self)
{
	vec3_t start, end, endp, offset;
	vec3_t forward, right, up, angles;
	trace_t tr;

	// zucc compensate for weapon ride up
	VectorAdd(self->owner->client->v_angle, self->owner->client->kick_angles, angles);
	AngleVectors(angles, forward, right, up);

	VectorSet(offset, 24, 8, self->owner->viewheight);
	P_ProjectSource(self->owner->client, self->owner->s.origin, offset, forward, right, start);
	VectorMA(start, 8192, forward, end);

	PRETRACE();
	tr = gi.trace(start, NULL, NULL, end, self->owner, CONTENTS_SOLID);
	POSTTRACE();

	if (tr.fraction != 1) {
		VectorMA(tr.endpos, -4, forward, endp);
		VectorCopy(endp, tr.endpos);
	}

	VectorCopy(tr.endpos, self->s.origin);
	self->s.origin[2] += 24;
	self->nextthink = level.time + 0.1;

	gi.linkentity(self);
}

void Cmd_Blox_f(edict_t * ent)
{
	char *cmd = gi.args();
	float ftmp;

	if(cmd == NULL || strlen(cmd) == 0) {
		gi.cprintf(ent, PRINT_HIGH, "Blox command help:\n");
		gi.cprintf(ent, PRINT_HIGH, " blox create <name> - create an object\n");
		gi.cprintf(ent, PRINT_HIGH, " blox activate - activate pointer object\n");
		gi.cprintf(ent, PRINT_HIGH, " blox lock - make the current object solid\n");
		gi.cprintf(ent, PRINT_HIGH, " blox destroy - destroy current object\n");
		gi.cprintf(ent, PRINT_HIGH, " blox movex <i> - move active object\n");
		gi.cprintf(ent, PRINT_HIGH, " blox movey <i> - move active object\n");
		gi.cprintf(ent, PRINT_HIGH, " blox movez <i> - move active object\n");
	} else if(Q_stricmp(cmd, "create") == 0) {
		Blox_Create(ent);
	} else if(Q_stricmp(cmd, "activate") == 0) {
		Blox_MakeActive(ent);
	} else if(Q_stricmp(cmd, "lock") == 0) {
		Blox_Lock(ent);
	} else if(Q_stricmp(cmd, "destroy") == 0) {
		Blox_Destroy(ent);
	} else if(sscanf(cmd, "movex %f", &ftmp) == 1) {
		Blox_MoveActive(ent, ftmp, 0, 0);
	} else if(sscanf(cmd, "movey %f", &ftmp) == 1) {
		Blox_MoveActive(ent, 0, ftmp, 0);
	} else if(sscanf(cmd, "movez %f", &ftmp) == 1) {
		Blox_MoveActive(ent, 0, 0, ftmp);
	} else {
		gi.cprintf(ent, PRINT_HIGH, "Unknown blox command: %s\n", cmd);
	}
}
