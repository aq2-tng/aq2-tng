#include "g_local.h"

/*
===============
FL_make
make the flashlight
===============
*/

void FL_make (edict_t * self)
{
	vec3_t start, forward, right, end;

	if (!darkmatch->value)
		return;

	if ((self->deadflag == DEAD_DEAD) || (self->solid == SOLID_NOT))
	{
		if (self->flashlight)
		{
			G_FreeEdict (self->flashlight);
			self->flashlight = NULL;
		}
		return;
	}

	gi.sound (self, CHAN_VOICE, gi.soundindex ("misc/flashlight.wav"), 1,
	ATTN_NORM, 0);

	if (self->flashlight)
	{
		G_FreeEdict (self->flashlight);
		self->flashlight = NULL;
		return;
	}

	AngleVectors (self->client->v_angle, forward, right, NULL);

	VectorSet (end, 100, 0, 0);
	G_ProjectSource (self->s.origin, end, forward, right, start);

	self->flashlight = G_Spawn ();
	self->flashlight->owner = self;
	self->flashlight->movetype = MOVETYPE_NOCLIP;
	self->flashlight->solid = SOLID_NOT;
	self->flashlight->classname = "flashlight";
	self->flashlight->s.modelindex = gi.modelindex ("sprites/null.sp2");
	self->flashlight->s.skinnum = 0;
	self->flashlight->s.effects |= EF_HYPERBLASTER;	// Other effects can be used here, such as flag1, but these look corney and dull. Try stuff and tell me if you find anything cool (EF_HYPERBLASTER)
	self->flashlight->think = FL_think;
	self->flashlight->nextthink = level.time + 0.1;
}

/*
8===============>
FL_think
Moving the flashlight
<===============8
*/
void FL_think (edict_t * self)
{

	vec3_t start, end, endp, offset;
	vec3_t forward, right, up;
	vec3_t angles;
	trace_t tr;
	int height = 0;

  /*vec3_t start,end,endp,offset;
     vec3_t forward,right,up;
     trace_t tr; */

  //AngleVectors (self->owner->client->v_angle, forward, right, up);
  VectorAdd (self->owner->client->v_angle, self->owner->client->kick_angles,
	     angles);
  AngleVectors ( /*self->owner->client->v_angle */ angles, forward, right,
		up);

/*	VectorSet(offset,24 , 6, self->owner->viewheight-7);
	G_ProjectSource (self->owner->s.origin, offset, forward, right, start);
	VectorMA(start,8192,forward,end);

	tr = gi.trace (start,NULL,NULL, end,self->owner,CONTENTS_SOLID|CONTENTS_MONSTER|CONTENTS_DEADMONSTER);

	if (tr.fraction != 1)
	{
		VectorMA(tr.endpos,-4,forward,endp);
		VectorCopy(endp,tr.endpos);
	}

	if ((tr.ent->svflags & SVF_MONSTER) || (tr.ent->client))
	{
		if ((tr.ent->takedamage) && (tr.ent != self->owner))
		{
			self->s.skinnum = 1;
		}
	}
	else
		self->s.skinnum = 0;

	vectoangles(tr.plane.normal,self->s.angles);
	VectorCopy(tr.endpos,self->s.origin);

	gi.linkentity (self);
	self->nextthink = level.time + 0.1; */

	if (self->owner->client->pers.firing_style == ACTION_FIRING_CLASSIC)
		height = 8;

	VectorSet (offset, 24, 8, self->owner->viewheight - height);

	P_ProjectSource (self->owner->client, self->owner->s.origin, offset,
		forward, right, start);
	VectorMA (start, 8192, forward, end);

	PRETRACE ();
	tr = gi.trace (start, NULL, NULL, end, self->owner,
		CONTENTS_SOLID | CONTENTS_MONSTER | CONTENTS_DEADMONSTER);
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
