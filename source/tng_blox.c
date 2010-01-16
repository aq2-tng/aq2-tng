#include "g_local.h"

/* used the lasersight code for reference for the live mouse move */

void Blox_Create(edict_t * self)
{
	vec3_t start, forward, right, end;

	//AngleVectors(self->client->v_angle, forward, right, NULL);
	//VectorSet(end, 100, 0, 0);
	G_ProjectSource(self->s.origin, end, forward, right, start);
	self->blox = G_Spawn();
	self->blox->owner = self;
	self->blox->movetype = MOVETYPE_NOCLIP;
	self->blox->solid = SOLID_NOT;
	self->blox->classname = "blox_edit";
	self->blox->model = "models/objects/dmspot/tris.md2";
	self->blox->s.renderfx = RF_TRANSLUCENT;
	self->blox->think = Blox_Think;
	self->blox->nextthink = level.time + 0.01;

	VectorSet (self->blox->mins, -32, -32, -24);
	VectorSet (self->blox->maxs, 32, 32, -16);

	gi.setmodel(self->blox, self->blox->model);
	//pos[2] -= 24.0;
}

void Blox_Cancel(edict_t * self)
{
	if(self->blox)
		self->blox->think = G_FreeEdict;
}

void Blox_Lock(edict_t * self)
{
	if(self->blox) {
		self->blox->classname = "blox_solid";
		self->blox->solid = SOLID_BBOX;
		self->blox->s.renderfx = 0;
		self->blox->nextthink = 0;
		// is it ok to just leave the entity in the world?
		self->blox = NULL;
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
	} else if(Q_stricmp(cmd, "create") == 0) {
		Blox_Create(ent);
	} else if(Q_stricmp(cmd, "lock") == 0) {
		Blox_Lock(ent);
	} else {
		gi.cprintf(ent, PRINT_HIGH, "Unknown blox command: %s\n", cmd);
	}
}
