#include "g_local.h"

edict_t *ball = NULL;
float activate = 0.0;

void B_make (edict_t * self)
{
	if(!ball) {
		ball = G_Spawn ();
		ball->movetype = MOVETYPE_BOUNCE;
		ball->classname = "ball";
		ball->think = B_think;
		ball->nextthink = level.time + 0.1;

		VectorSet (ball->avelocity, 0, 300, 0);
		ball->model = "models/objects/gibs/skull/tris.md2";

		VectorSet (ball->mins, -8, -8, 0);
		VectorSet (ball->maxs, 8, 8, 32);

		gi.setmodel(ball, ball->model);
	}

	ball->solid = SOLID_TRIGGER;
	activate = level.time + 2;

	VectorCopy(self->s.origin, ball->s.origin);

	gi.cprintf(self, PRINT_HIGH, "Poof! A magic ball!\n");

	gi.linkentity(ball);
}

void B_think (edict_t * self)
{
	self->nextthink = level.time + 0.1;
	self->s.effects |= EF_GIB;
	if(activate > 0 && self->solid == SOLID_TRIGGER && level.time > activate)
		ball->solid = SOLID_BBOX;
	gi.linkentity(self);
}

void B_kick (edict_t * ent)
{
	vec3_t start;
	vec3_t forward, right;
	vec3_t offset;
	float kick = 1000;
	trace_t tr;
	vec3_t end;

	AngleVectors (ent->client->v_angle, forward, right, NULL);

	VectorScale (forward, 0, ent->client->kick_origin);

	VectorSet (offset, 0, 0, ent->viewheight - 100);
	//VectorSet (offset, 0, 0, ent->viewheight - 20);
	P_ProjectSource (ent->client, ent->s.origin, offset, forward, right, start);

	VectorMA (start, 100, forward, end);

	PRETRACE ();
	tr = gi.trace (ent->s.origin, NULL, NULL, end, ent, MASK_SHOT);
	POSTTRACE ();

	if(strcmp(tr.ent->classname, "ball"))
		return;

	if (tr.fraction >= 1.0)
		return;

	VectorNormalize (forward);

	vec3_t kvel, flydir;

	VectorCopy (forward, flydir);
	flydir[2] += 0.5f;

	VectorScale (flydir, kick, kvel);
	VectorAdd (ball->velocity, kvel, ball->velocity);

	gi.sound (ent, CHAN_WEAPON, gi.soundindex ("weapons/kick.wav"), 1, ATTN_NORM, 0);
	PlayerNoise (ent, ent->s.origin, PNOISE_SELF);
	ent->client->jumping = 0;	// only 1 jumpkick per jump
}
