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

void Blox_Create(edict_t * self)
{
	if(self->blox)
		Blox_Lock(self);

	self->blox = Blox_CreatePlatform(false);
	self->blox->owner = self;
	self->blox->think = Blox_Think;
	self->blox->nextthink = level.time + 0.01;
	VectorCopy(self->s.origin, self->blox->s.origin);
	gi.linkentity(self->blox);
}

void Blox_Cancel(edict_t * self)
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
		// for some wicked reason, you can't make a SOLID_NOT entity a SOLID_BBOX
		tmp = Blox_CreatePlatform(true);
		VectorCopy(self->blox->s.origin, tmp->s.origin);
		G_FreeEdict(self->blox);
		self->blox = tmp;
		gi.linkentity(self->blox);
	}
}

void Blox_Think(edict_t * self)
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

	if(cmd == NULL || strlen(cmd) == 0) {
		gi.cprintf(ent, PRINT_HIGH, "Blox command help:\n");
		gi.cprintf(ent, PRINT_HIGH, " blox create <name> - create an object\n");
		gi.cprintf(ent, PRINT_HIGH, " blox lock - make the current object solid\n");
		gi.cprintf(ent, PRINT_HIGH, " blox cancel - destroy current object\n");
	} else if(Q_stricmp(cmd, "create") == 0) {
		Blox_Create(ent);
	} else if(Q_stricmp(cmd, "lock") == 0) {
		Blox_Lock(ent);
	} else if(Q_stricmp(cmd, "cancel") == 0) {
		Blox_Cancel(ent);
	} else {
		gi.cprintf(ent, PRINT_HIGH, "Unknown blox command: %s\n", cmd);
	}
}
