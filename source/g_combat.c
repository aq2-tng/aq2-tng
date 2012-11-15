//-----------------------------------------------------------------------------
// g_combat.c
//
// $Id: g_combat.c,v 1.27 2002/09/04 11:23:09 ra Exp $
//
//-----------------------------------------------------------------------------
// $Log: g_combat.c,v $
// Revision 1.27  2002/09/04 11:23:09  ra
// Added zcam to TNG and bumped version to 3.0
//
// Revision 1.26  2002/04/01 15:47:51  freud
// Typo fixed for statistics
//
// Revision 1.25  2002/04/01 15:16:06  freud
// Stats code redone, tng_stats now much more smarter. Removed a few global
// variables regarding stats code and added kevlar hits to stats.
//
// Revision 1.24  2002/02/19 10:28:43  freud
// Added to %D hit in the kevlar vest and kevlar helmet, also body for handcannon
// and shotgun.
//
// Revision 1.23  2002/02/18 17:17:20  freud
// Fixed the CTF leaving team bug. Also made the shield more efficient,
// No falling damage.
//
// Revision 1.22  2002/02/18 13:55:35  freud
// Added last damaged players %P
//
// Revision 1.21  2002/02/01 17:49:56  freud
// Heavy changes in stats code. Removed lots of variables and replaced them
// with int arrays of MODs. This cleaned tng_stats.c up a whole lots and
// everything looks well, might need more testing.
//
// Revision 1.20  2002/01/24 02:24:56  deathwatch
// Major update to Stats code (thanks to Freud)
// new cvars:
// stats_afterround - will display the stats after a round ends or map ends
// stats_endmap - if on (1) will display the stats scoreboard when the map ends
//
// Revision 1.19  2002/01/23 14:51:35  ra
// Damn if statements from HELL (ff_afterrounds fix)
//
// Revision 1.18  2002/01/23 14:18:02  ra
// Fixed another ff_afterrounds bobo
//
// Revision 1.17  2001/12/23 21:19:41  deathwatch
// Updated stats with location and average
// cleaned it up a bit as well
//
// Revision 1.16  2001/12/23 16:30:50  ra
// 2.5 ready. New stats from Freud. HC and shotgun gibbing seperated.
//
// Revision 1.15  2001/09/28 13:48:34  ra
// I ran indent over the sources. All .c and .h files reindented.
//
// Revision 1.14  2001/08/18 01:28:06  deathwatch
// Fixed some stats stuff, added darkmatch + day_cycle, cleaned up several files, restructured ClientCommand
//
// Revision 1.13  2001/08/17 21:31:37  deathwatch
// Added support for stats
//
// Revision 1.12  2001/08/06 14:38:45  ra
// Adding UVtime for ctf
//
// Revision 1.11  2001/08/06 03:00:48  ra
// Added FF after rounds. Please someone look at the EVIL if statments for me :)
//
// Revision 1.10  2001/07/16 19:02:06  ra
// Fixed compilerwarnings (-g -Wall).  Only one remains.
//
// Revision 1.9  2001/06/22 18:54:38  igor_rock
// fixed the "accuracy" for killing teammates with headshots
//
// Revision 1.8  2001/06/20 07:21:21  igor_rock
// added use_warnings to enable/disable time/frags left msgs
// added use_rewards to enable/disable eimpressive, excellent and accuracy msgs
// change the configfile prefix for modes to "mode_" instead "../mode-" because
// they don't have to be in the q2 dir for doewnload protection (action dir is sufficient)
// and the "-" is bad in filenames because of linux command line parameters start with "-"
//
// Revision 1.7  2001/05/31 16:58:14  igor_rock
// conflicts resolved
//
// Revision 1.6.2.3  2001/05/25 18:59:52  igor_rock
// Added CTF Mode completly :)
// Support for .flg files is still missing, but with "real" CTF maps like
// tq2gtd1 the ctf works fine.
// (I hope that all other modes still work, just tested DM and teamplay)
//
// Revision 1.6.2.2  2001/05/20 18:54:19  igor_rock
// added original ctf code snippets from zoid. lib compilesand runs but
// doesn't function the right way.
// Jsut committing these to have a base to return to if something wents
// awfully wrong.
//
// Revision 1.6.2.1  2001/05/20 15:17:31  igor_rock
// removed the old ctf code completly
//
// Revision 1.6  2001/05/20 12:54:18  igor_rock
// Removed newlines from Centered Messages like "Impressive"
//
// Revision 1.5  2001/05/12 14:05:29  mort
// Hurting someone whilst they are "god" is fixed now
//
// Revision 1.4  2001/05/11 12:21:19  slicerdw
// Commented old Location support ( ADF ) With the ML/ETE Compatible one
//
// Revision 1.2  2001/05/07 20:06:45  igor_rock
// changed sound dir from sound/rock to sound/tng
//
// Revision 1.1.1.1  2001/05/06 17:30:50  igor_rock
// This is the PG Bund Edition V1.25 with all stuff laying around here...
//
//-----------------------------------------------------------------------------

#include "g_local.h"
#include "cgf_sfx_glass.h"

void Add_TeamWound (edict_t * attacker, edict_t * victim, int mod);

/*
  ============
  CanDamage
  
  Returns true if the inflictor can directly damage the target.  Used for
  explosions and melee attacks.
  ============
*/
qboolean CanDamage (edict_t * targ, edict_t * inflictor)
{
	vec3_t dest;
	trace_t trace;

	// bmodels need special checking because their origin is 0,0,0
	//GLASS FX
	if ((targ->movetype == MOVETYPE_PUSH) ||
	((targ->movetype == MOVETYPE_FLYMISSILE)
	&& (0 == Q_stricmp ("func_explosive", targ->classname))))
	//GLASS FX
	{
		VectorAdd (targ->absmin, targ->absmax, dest);
		VectorScale (dest, 0.5, dest);
		PRETRACE ();
		trace = gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest,
			inflictor, MASK_SOLID);
		POSTTRACE ();
		if (trace.fraction == 1.0)
			return true;
		if (trace.ent == targ)
			return true;
		return false;
	}

	PRETRACE ();
	trace = gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, targ->s.origin,
		inflictor, MASK_SOLID);
	POSTTRACE ();
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] += 15.0;
	dest[1] += 15.0;
	PRETRACE ();
	trace = gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor,
		MASK_SOLID);
	POSTTRACE ();
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] += 15.0;
	dest[1] -= 15.0;
	PRETRACE ();
	trace =	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor,
		MASK_SOLID);
	POSTTRACE ();
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] -= 15.0;
	dest[1] += 15.0;
	PRETRACE ();
	trace = gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor,
		MASK_SOLID);
	POSTTRACE ();
	if (trace.fraction == 1.0)
		return true;

	VectorCopy (targ->s.origin, dest);
	dest[0] -= 15.0;
	dest[1] -= 15.0;
	PRETRACE ();
	trace =	gi.trace (inflictor->s.origin, vec3_origin, vec3_origin, dest, inflictor,
		MASK_SOLID);
	POSTTRACE ();
	if (trace.fraction == 1.0)
		return true;

	return false;
}


/*
  ============
  Killed
  ============
*/
void Killed (edict_t * targ, edict_t * inflictor, edict_t * attacker, int damage,
	vec3_t point)
{
	if (targ->health < -999)
		targ->health = -999;

	if (targ->client)
	{
		targ->client->bleeding = 0;
		//targ->client->bleedcount = 0;
		targ->client->bleed_remain = 0;
	}

	targ->enemy = attacker;

	if ((targ->svflags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD))
	{
		//              targ->svflags |= SVF_DEADMONSTER;       // now treat as a different content type
		if (!(targ->monsterinfo.aiflags & AI_GOOD_GUY))
		{
			level.killed_monsters++;
			if (coop->value && attacker->client)
				attacker->client->resp.score++;
			// medics won't heal monsters that they kill themselves
			if (strcmp (attacker->classname, "monster_medic") == 0)
				targ->owner = attacker;
		}
	}

	if (targ->movetype == MOVETYPE_PUSH || targ->movetype == MOVETYPE_STOP
	|| targ->movetype == MOVETYPE_NONE)
	{				// doors, triggers, etc
		targ->die (targ, inflictor, attacker, damage, point);
		return;
	}

	if ((targ->svflags & SVF_MONSTER) && (targ->deadflag != DEAD_DEAD))
	{
		targ->touch = NULL;
		monster_death_use (targ);
	}

	targ->die (targ, inflictor, attacker, damage, point);
}


/*
  ================
  SpawnDamage
  ================
*/
void SpawnDamage (int type, vec3_t origin, vec3_t normal, int damage)
{
	if (damage > 255)
		damage = 255;
	gi.WriteByte (svc_temp_entity);
	gi.WriteByte (type);
	//      gi.WriteByte (damage);
	gi.WritePosition (origin);
	gi.WriteDir (normal);
	gi.multicast (origin, MULTICAST_PVS);
}


/*
  ============
  T_Damage
  
  targ            entity that is being damaged
  inflictor       entity that is causing the damage
  attacker        entity that caused the inflictor to damage targ
  example: targ=monster, inflictor=rocket, attacker=player
  
  dir                     direction of the attack
  point           point at which the damage is being inflicted
  normal          normal vector from that point
  damage          amount of damage being inflicted
  knockback       force to be applied against targ as a result of the damage
  
  dflags          these flags are used to control how T_Damage works
  DAMAGE_RADIUS                   damage was indirect (from a nearby explosion)
  DAMAGE_NO_ARMOR                 armor does not protect from this damage
  DAMAGE_ENERGY                   damage is from an energy based weapon
  DAMAGE_NO_KNOCKBACK             do not affect velocity, just view angles
  DAMAGE_BULLET                   damage is from a bullet (used for ricochets)
  DAMAGE_NO_PROTECTION    kills godmode, armor, everything
  ============
*/
static int
CheckPowerArmor (edict_t * ent, vec3_t point, vec3_t normal, int damage,
		 int dflags)
{
	gclient_t *client;
	int save;
	int power_armor_type;
	// AQ:TNG - JBravo fixing compilerwarnings.
	// Bah.  JB will crash da servah if dis is wrong.
	int index = 0;
	// JBravo.
	int damagePerCell;
	int pa_te_type;
	int power = 0;
	int power_used;

	if (!damage)
		return 0;

	client = ent->client;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	if (client)
	{
		power_armor_type = PowerArmorType (ent);
		if (power_armor_type != POWER_ARMOR_NONE)
		{
			index = ITEM_INDEX (FindItem ("Cells"));
			power = client->pers.inventory[index];
		}
	}
	else if (ent->svflags & SVF_MONSTER)
	{
		power_armor_type = ent->monsterinfo.power_armor_type;
		power = ent->monsterinfo.power_armor_power;
	}
	else
		return 0;

	if (power_armor_type == POWER_ARMOR_NONE)
		return 0;
	if (!power)
		return 0;

	if (power_armor_type == POWER_ARMOR_SCREEN)
	{
		vec3_t vec;
		float dot;
		vec3_t forward;

		// only works if damage point is in front
		AngleVectors (ent->s.angles, forward, NULL, NULL);
		VectorSubtract (point, ent->s.origin, vec);
		VectorNormalize (vec);
		dot = DotProduct (vec, forward);
		if (dot <= 0.3)
			return 0;

		damagePerCell = 1;
		pa_te_type = TE_SCREEN_SPARKS;
		damage = damage / 3;
	}
	else
	{
		damagePerCell = 2;
		pa_te_type = TE_SHIELD_SPARKS;
		damage = (2 * damage) / 3;
	}

	save = power * damagePerCell;
	if (!save)
		return 0;
	if (save > damage)
		save = damage;

	SpawnDamage (pa_te_type, point, normal, save);
	ent->powerarmor_time = level.time + 0.2;

	power_used = save / damagePerCell;

	if (client)
		client->pers.inventory[index] -= power_used;
	else
		ent->monsterinfo.power_armor_power -= power_used;
	return save;
}

static int
CheckArmor (edict_t * ent, vec3_t point, vec3_t normal, int damage,
	    int te_sparks, int dflags)
{
	gclient_t *client;
	int save;
	int index;
	gitem_t *armor;

	if (!damage)
		return 0;

	client = ent->client;

	if (!client)
		return 0;

	if (dflags & DAMAGE_NO_ARMOR)
		return 0;

	index = ArmorIndex (ent);
	if (!index)
		return 0;

	armor = GetItemByIndex (index);

	if (dflags & DAMAGE_ENERGY)
		save = ceil (((gitem_armor_t *) armor->info)->energy_protection * damage);
	else
		save = ceil (((gitem_armor_t *) armor->info)->normal_protection * damage);
	if (save >= client->pers.inventory[index])
		save = client->pers.inventory[index];

	if (!save)
		return 0;

	client->pers.inventory[index] -= save;
	SpawnDamage (te_sparks, point, normal, save);

	return save;
}

void
M_ReactToDamage (edict_t * targ, edict_t * attacker)
{
	if (!(attacker->client) && !(attacker->svflags & SVF_MONSTER))
		return;

	if (attacker == targ || attacker == targ->enemy)
		return;

	// if we are a good guy monster and our attacker is a player
	// or another good guy, do not get mad at them
	if (targ->monsterinfo.aiflags & AI_GOOD_GUY)
	{
		if (attacker->client || (attacker->monsterinfo.aiflags & AI_GOOD_GUY))
			return;
	}

	// we now know that we are not both good guys

	// if attacker is a client, get mad at them because he's good and we're not
	if (attacker->client)
	{
		// this can only happen in coop (both new and old enemies are clients)
		// only switch if can't see the current enemy
		if (targ->enemy && targ->enemy->client)
		{
			if (visible (targ, targ->enemy))
			{
				targ->oldenemy = attacker;
				return;
			}
			targ->oldenemy = targ->enemy;
		}
		targ->enemy = attacker;
		if (!(targ->monsterinfo.aiflags & AI_DUCKED))
			FoundTarget (targ);
		return;
	}

	// it's the same base (walk/swim/fly) type and a different classname and it's not a tank
	// (they spray too much), get mad at them
	if (((targ->flags & (FL_FLY | FL_SWIM)) ==
	 (attacker->flags & (FL_FLY | FL_SWIM)))
	&& (strcmp (targ->classname, attacker->classname) != 0)
	&& (strcmp (attacker->classname, "monster_tank") != 0)
	&& (strcmp (attacker->classname, "monster_supertank") != 0)
	&& (strcmp (attacker->classname, "monster_makron") != 0)
	&& (strcmp (attacker->classname, "monster_jorg") != 0))
	{
		if (targ->enemy && targ->enemy->client)
			targ->oldenemy = targ->enemy;
		targ->enemy = attacker;
		if (!(targ->monsterinfo.aiflags & AI_DUCKED))
			FoundTarget (targ);
	}
	else
	// otherwise get mad at whoever they are mad at (help our buddy)
	{
		if (targ->enemy && targ->enemy->client)
			targ->oldenemy = targ->enemy;
		targ->enemy = attacker->enemy;
		if (!(targ->monsterinfo.aiflags & AI_DUCKED))
			FoundTarget (targ);
	}
}

qboolean CheckTeamDamage (edict_t * targ, edict_t * attacker)
{
	//AQ2:TNG Igor isn't quite sure if the next statement really belongs here... (FIXME) commented it out (friendly fire already does it, hm?)
	//  if (ctf->value && targ->client && attacker->client)
	//  if (targ->client->resp.team == attacker->client->resp.team &&
	//    targ != attacker)
	//      return true;
	//AQ2:TNG End
	//FIXME make the next line real and uncomment this block
	// if ((ability to damage a teammate == OFF) && (targ's team == attacker's team))
	return false;
}



void BloodSprayThink (edict_t * self)
{


  /*  if ( self->dmg > 0 )
     {
     self->dmg -= 10;
     //              SpawnDamage (TE_BLOOD, self->s.origin, self->movedir, self->dmg);
     gi.WriteByte (svc_temp_entity);
     gi.WriteByte (TE_SPLASH);
     gi.WriteByte (6);
     gi.WritePosition (self->s.origin);
     gi.WriteDir (self->movedir);
     gi.WriteByte (6);       //blood
     gi.multicast (self->s.origin, MULTICAST_PVS);

     }
     else
     {
     self->think = G_FreeEdict;
     }

     self->nextthink = level.time + 0.1;
     gi.linkentity (self);
   */

  G_FreeEdict (self);
}

void blood_spray_touch (edict_t * ent, edict_t * other, cplane_t * plane,
		   csurface_t * surf)
{
	if (other == ent->owner)
		return;
	ent->think = G_FreeEdict;
	ent->nextthink = level.time + 0.1;
}



void spray_blood (edict_t * self, vec3_t start, vec3_t dir, int damage, int mod)
{
	edict_t *blood;
	int speed;

	switch (mod)
	{
	case MOD_MK23:
		speed = 1800;
		break;
	case MOD_MP5:
		speed = 1500;
		break;
	case MOD_M4:
		speed = 2400;
		break;
	case MOD_KNIFE:
		speed = 0;
		break;
	case MOD_KNIFE_THROWN:
		speed = 0;
		break;
	case MOD_DUAL:
		speed = 1800;
		break;
	case MOD_SNIPER:
		speed = 4000;
		break;
	default:
		speed = 1800;
	}




	blood = G_Spawn ();
	VectorNormalize (dir);
	VectorCopy (start, blood->s.origin);
	VectorCopy (dir, blood->movedir);
	vectoangles (dir, blood->s.angles);
	VectorScale (dir, speed, blood->velocity);
	blood->movetype = MOVETYPE_BLOOD;
	blood->clipmask = MASK_SHOT;
	blood->solid = SOLID_BBOX;
	blood->s.effects |= EF_GIB;
	VectorClear (blood->mins);
	VectorClear (blood->maxs);
	blood->s.modelindex = gi.modelindex ("sprites/null.sp2");
	blood->owner = self;
	blood->nextthink = level.time + speed / 1000;	//3.2;
	blood->touch = blood_spray_touch;
	blood->think = BloodSprayThink;
	blood->dmg = damage;
	blood->classname = "blood_spray";

	gi.linkentity (blood);
}


// zucc based on some code in Action Quake
void spray_sniper_blood (edict_t * self, vec3_t start, vec3_t dir)
{
	vec3_t forward;
	int mod = MOD_SNIPER;

	VectorCopy (dir, forward);

	forward[2] += .03f;

	spray_blood (self, start, forward, 0, mod);


	VectorCopy (dir, forward);
	forward[2] -= .03f;
	spray_blood (self, start, forward, 0, mod);


	VectorCopy (dir, forward);
	if ((forward[0] > 0) && (forward[1] > 0))
	{
		forward[0] -= .03f;
		forward[1] += .03f;
	}
	if ((forward[0] > 0) && (forward[1] < 0))
	{
		forward[0] += .03f;
		forward[1] += .03f;
	}
	if ((forward[0] < 0) && (forward[1] > 0))
	{
		forward[0] -= .03f;
		forward[1] -= .03f;
	}
	if ((forward[0] < 0) && (forward[1] < 0))
	{
		forward[0] += .03f;
		forward[1] -= .03f;
	}
	spray_blood (self, start, forward, 0, mod);


	VectorCopy (dir, forward);
	if ((forward[0] > 0) && (forward[1] > 0))
	{
		forward[0] += .03f;
		forward[1] -= .03f;
	}
	if ((forward[0] > 0) && (forward[1] < 0))
	{
		forward[0] -= .03f;
		forward[1] -= .03f;
	}
	if ((forward[0] < 0) && (forward[1] > 0))
	{
		forward[0] += .03f;
		forward[1] += .03f;
	}
	if ((forward[0] < 0) && (forward[1] < 0))
	{
		forward[0] -= .03f;
		forward[1] += .03f;
	}
	spray_blood (self, start, forward, 0, mod);

	VectorCopy (dir, forward);
	spray_blood (self, start, forward, 0, mod);

}


void VerifyHeadShot (vec3_t point, vec3_t dir, float height, vec3_t newpoint)
{
	vec3_t normdir;
	vec3_t normdir2;


	VectorNormalize2 (dir, normdir);
	VectorScale (normdir, height, normdir2);
	VectorAdd (point, normdir2, newpoint);
}



// zucc adding location hit code
// location hit code based off ideas by pyromage and shockman

#define LEG_DAMAGE (height/2.2) - abs(targ->mins[2]) - 3
#define STOMACH_DAMAGE (height/1.8) - abs(targ->mins[2])
#define CHEST_DAMAGE (height/1.4) - abs(targ->mins[2])

#define HEAD_HEIGHT 12.0
qboolean IsFemale (edict_t * ent);


void
T_Damage (edict_t * targ, edict_t * inflictor, edict_t * attacker, vec3_t dir,
	  vec3_t point, vec3_t normal, int damage, int knockback, int dflags,
	  int mod)
{
	gclient_t *client;
	char buf[256];
	int take, save;
	int asave, psave;
	int te_sparks, do_sparks = 0;
	int damage_type = 0;		// used for MOD later
	int bleeding = 0;		// damage causes bleeding
	int head_success = 0;
	int instant_dam = 1;
	float z_rel;
	int height;
	float from_top;
	vec_t dist;
	float targ_maxs2;		//FB 6/1/99

	// do this before teamplay check
	if (!targ->takedamage)
		return;

	//FIREBLADE
	if (teamplay->value && mod != MOD_TELEFRAG)
	{
		if (lights_camera_action)
			return;

		// AQ2:TNG - JBravo adding UVtime
		if (ctf->value && targ->client)
		{
			if(targ->client->ctf_uvtime > 0)
				return;
			if (attacker->client && attacker->client->ctf_uvtime > 0)
				return;
		}

		// AQ2:TNG - JBravo adding FF after rounds
		if (targ != attacker && targ->client && attacker->client &&
			targ->client->resp.team == attacker->client->resp.team &&
			((int)(dmflags->value) & (DF_NO_FRIENDLY_FIRE))) {
				if (team_round_going)
					return;
				else if (!ff_afterround->value)
					return;
		}
		// AQ:TNG
	}
	//FIREBLADE

	if (dm_shield->value && targ->client)
	{
		if (targ->client->ctf_uvtime > 0)
			return;
		if (attacker->client && attacker->client->ctf_uvtime > 0)
			return;
	}

	// damage reduction for shotgun
	// if far away, reduce it to original action levels
	if (mod == MOD_M3)
	{
		dist = Distance(targ->s.origin, inflictor->s.origin);
		if (dist > 450.0)
			damage = damage - 2;
	}

	targ_maxs2 = targ->maxs[2];
	if (targ_maxs2 == 4)
		targ_maxs2 = CROUCHING_MAXS2;	//FB 6/1/99

	height = abs (targ->mins[2]) + targ_maxs2;

	// locational damage code
	// base damage is head shot damage, so all the scaling is downwards
	if (targ->client)
	{
		if (!((targ != attacker) &&
		((deathmatch->value && ((int)dmflags->value
		& (DF_MODELTEAMS | DF_SKINTEAMS)))
		|| coop->value) && (attacker && attacker->client
		&& OnSameTeam (targ, attacker) &&	 
		((int)dmflags->value & DF_NO_FRIENDLY_FIRE)
		&& (team_round_going && ff_afterround->value))))

		{

			// TNG Stats - Add +1 to hit, make sure that hc and m3 are handles differently

			if ((attacker->client) && (mod != MOD_M3) && (mod != MOD_HC)) {
				strcpy(attacker->client->resp.last_damaged_players, targ->client->pers.netname);

				if (!teamplay->value || team_round_going || stats_afterround->value) {
					attacker->client->resp.stats_hits[mod]++;
					attacker->client->resp.stats_shots_h++;
				}
			}
 
			// TNG Stats END


			if (mod == MOD_MK23 || mod == MOD_MP5 || mod == MOD_M4 ||
				mod == MOD_SNIPER || mod == MOD_DUAL || mod == MOD_KNIFE ||
				mod == MOD_KNIFE_THROWN)
			{
				z_rel = point[2] - targ->s.origin[2];
				from_top = targ_maxs2 - z_rel;
				if (from_top < 0.0)	//FB 6/1/99
				from_top = 0.0;	//Slightly negative values were being handled wrong
				bleeding = 1;
				instant_dam = 0;

				// damage reduction for longer range pistol shots
				if (mod == MOD_MK23 || mod == MOD_DUAL)
				{
					dist = Distance(targ->s.origin, inflictor->s.origin);
					if (dist > 600.0 && dist < 1400.0)
						damage = (int) (damage * 2 / 3);
					else if (dist > 1400.0)
						damage = (int) (damage * 1 / 2);
				}


	      //gi.cprintf(targ, PRINT_HIGH, "z_rel is %f\n leg: %f stomach: %f chest: %f\n", z_rel, LEG_DAMAGE, STOMACH_DAMAGE, CHEST_DAMAGE );
	      //gi.cprintf(targ, PRINT_HIGH, "point[2]: %f targ->s.origin[2]: %f height: %d\n", point[2], targ->s.origin[2], height );
	      //gi.cprintf(targ, PRINT_HIGH, "abs(trag->min[2]): %d targ_max[2] %d\n", (int)abs(targ->mins[2]), (int)targ_maxs2);
	      //gi.cprintf(attacker, PRINT_HIGH, "abs(trag->min[2]): %d targ_max[2] %d\n", (int)abs(targ->mins[2]), (int)targ_maxs2); 
	      //gi.cprintf(attacker, PRINT_HIGH, "abs(trag->min[0]): %d targ_max[0] %d\n", (int)abs(targ->mins[0]), (int)targ->maxs[0]); 
	      //gi.cprintf(attacker, PRINT_HIGH, "abs(trag->min[1]): %d targ_max[1] %d\n", (int)abs(targ->mins[1]), (int)targ->maxs[1]); 


				if (from_top < 2 * HEAD_HEIGHT)
				{
					vec3_t new_point;
					VerifyHeadShot (point, dir, HEAD_HEIGHT, new_point);
					VectorSubtract (new_point, targ->s.origin, new_point);
					//gi.cprintf(attacker, PRINT_HIGH, "z: %d y: %d x: %d\n", (int)(targ_maxs2 - new_point[2]),(int)(new_point[1]) , (int)(new_point[0]) );

					if ((targ_maxs2 - new_point[2]) < HEAD_HEIGHT
						&& (abs (new_point[1])) < HEAD_HEIGHT * .8
						&& (abs (new_point[0])) < HEAD_HEIGHT * .8)
					{
						head_success = 1;
					}
				}

				if (head_success)
				{
					if (attacker->client)
					{
						if (!teamplay->value || team_round_going || stats_afterround->value) {
							attacker->client->resp.stats_headshot[mod]++;
						}
						//AQ2:TNG Slicer Last Damage Location
						if (INV_AMMO(targ, HELM_NUM)) {
							attacker->client->resp.last_damaged_part = LOC_KVLR_HELMET;
							if ((!teamplay->value || team_round_going || stats_afterround->value))
								attacker->client->resp.stats_locations[LOC_KVLR_HELMET]++;
						} else {
							attacker->client->resp.last_damaged_part = LOC_HDAM;
							if ((!teamplay->value || team_round_going || stats_afterround->value))
								attacker->client->resp.stats_locations[LOC_HDAM]++;
						}

						//AQ2:TNG END
						if (!OnSameTeam (targ, attacker))
							attacker->client->resp.hs_streak++;

						// AQ:TNG Igor[Rock] changing sound dir
						if (attacker->client->resp.hs_streak == 3)
						{
							if (use_rewards->value)
							{
								sprintf (buf, "ACCURACY %s!", attacker->client->pers.netname);
								CenterPrintAll (buf);
								gi.sound (&g_edicts[0], CHAN_VOICE | CHAN_NO_PHS_ADD,
									gi.soundindex ("tng/accuracy.wav"), 1.0, ATTN_NONE, 0.0);
							}
							attacker->client->resp.hs_streak = 0;
						}
						// end of changing sound dir
					}

					if (INV_AMMO(targ, HELM_NUM) && mod != MOD_KNIFE
						&& mod != MOD_KNIFE_THROWN && mod != MOD_SNIPER)
					{
						if (attacker->client)
						{
							gi.cprintf (attacker, PRINT_HIGH, "%s has a Kevlar Helmet - AIM FOR THE BODY!\n",
								targ->client->pers.netname);
							gi.cprintf (targ, PRINT_HIGH, "Kevlar Helmet absorbed a part of %s's shot\n",
								attacker->client->pers.netname);
						}
						gi.sound (targ, CHAN_ITEM, gi.soundindex("misc/vest.wav"), 1,
							ATTN_NORM, 0);
						damage = (int) (damage / 2);
						damage_type = LOC_HDAM;
						bleeding = 0;
						instant_dam = 1;
						stopAP = 1;
						do_sparks = 1;
					}
					else if (INV_AMMO(targ, HELM_NUM) && mod == MOD_SNIPER)
					{
						if (attacker->client)
						{
							gi.cprintf (attacker, PRINT_HIGH,
							"%s has a Kevlar Helmet, too bad you have AP rounds...\n",
							targ->client->pers.netname);
							gi.cprintf (targ, PRINT_HIGH,
							"Kevlar Helmet absorbed some of %s's AP sniper round\n",
							attacker->client->pers.netname);
						}
						damage = (int) (damage * 0.325);
						gi.sound (targ, CHAN_VOICE, gi.soundindex("misc/headshot.wav"), 1,
							ATTN_NORM, 0);
						damage_type = LOC_HDAM;
					}
					else
					{
						damage = damage * 1.8 + 1;
						gi.cprintf (targ, PRINT_HIGH, "Head damage\n");
						if (attacker->client)
							gi.cprintf (attacker, PRINT_HIGH, "You hit %s in the head\n",
								targ->client->pers.netname);
						damage_type = LOC_HDAM;
						if (mod != MOD_KNIFE && mod != MOD_KNIFE_THROWN)
							gi.sound (targ, CHAN_VOICE, gi.soundindex ("misc/headshot.wav"), 1,
								ATTN_NORM, 0);
					//else
					//      gi.sound(targ, CHAN_VOICE, gi.soundindex("misc/glurp.wav"), 1, ATTN_NORM, 0);                
					}
				}
				else if (z_rel < LEG_DAMAGE)
				{
					damage = damage * .25;
					gi.cprintf (targ, PRINT_HIGH, "Leg damage\n");
					if (attacker->client)
					{
						attacker->client->resp.hs_streak = 0;
						gi.cprintf (attacker, PRINT_HIGH, "You hit %s in the legs\n",
							targ->client->pers.netname);
					}
					damage_type = LOC_LDAM;
					targ->client->leg_damage = 1;
					targ->client->leghits++;
					//AQ2:TNG Slicer Last Damage Location
					attacker->client->resp.last_damaged_part = LOC_LDAM;
					//AQ2:TNG END
					if (!teamplay->value || team_round_going || stats_afterround->value)
						attacker->client->resp.stats_locations[LOC_LDAM]++; // TNG Stats
				}
				else if (z_rel < STOMACH_DAMAGE)
				{
					damage = damage * .4;
					gi.cprintf (targ, PRINT_HIGH, "Stomach damage\n");
					if (attacker->client)
					{
						attacker->client->resp.hs_streak = 0;
						gi.cprintf (attacker, PRINT_HIGH, "You hit %s in the stomach\n",
							targ->client->pers.netname);
					}
					damage_type = LOC_SDAM;
					//TempFile bloody gibbing
					if (mod == MOD_SNIPER && sv_gib->value)
						ThrowGib (targ, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
					//AQ2:TNG Slicer Last Damage Location
					attacker->client->resp.last_damaged_part = LOC_SDAM;
					//AQ2:TNG END
					if (!teamplay->value || team_round_going || stats_afterround->value)
						attacker->client->resp.stats_locations[LOC_SDAM]++; // TNG Stats
				}
				else		//(z_rel < CHEST_DAMAGE)
				{
					if (attacker->client)
					{
						attacker->client->resp.hs_streak = 0;
					}

					if (INV_AMMO(targ, KEV_NUM) && mod != MOD_KNIFE
						&& mod != MOD_KNIFE_THROWN && mod != MOD_SNIPER)
					{
						if (attacker->client)
						{
							gi.cprintf (attacker, PRINT_HIGH, "%s has a Kevlar Vest - AIM FOR THE HEAD!\n",
								targ->client->pers.netname);
							gi.cprintf (targ, PRINT_HIGH, "Kevlar Vest absorbed most of %s's shot\n",
								attacker->client->pers.netname);
							/*
							if (IsFemale(targ))
							gi.cprintf(attacker, PRINT_HIGH, "You bruised %s through her Kevlar Vest\n", targ->client->pers.netname);
							else
							gi.cprintf(attacker, PRINT_HIGH, "You bruised %s through his Kevlar Vest\n", targ->client->pers.netname);
							*/
						}
						gi.sound (targ, CHAN_ITEM, gi.soundindex ("misc/vest.wav"), 1,
							ATTN_NORM, 0);
						damage = (int) (damage / 10);
						damage_type = LOC_CDAM;
						bleeding = 0;
						instant_dam = 1;
						stopAP = 1;
						do_sparks = 1;
					}
					else if (INV_AMMO(targ, KEV_NUM) && mod == MOD_SNIPER)
					{
						if (attacker->client)
						{
							gi.cprintf (attacker, PRINT_HIGH, "%s has a Kevlar Vest, too bad you have AP rounds...\n",
								targ->client->pers.netname);
							gi.cprintf (targ, PRINT_HIGH, "Kevlar Vest absorbed some of %s's AP sniper round\n",
								attacker->client->pers.netname);
						}
						damage = damage * .325;
						damage_type = LOC_CDAM;
					}
					else
					{
						damage = damage * .65;
						gi.cprintf (targ, PRINT_HIGH, "Chest damage\n");
						if (attacker->client)
							gi.cprintf (attacker, PRINT_HIGH, "You hit %s in the chest\n",
								targ->client->pers.netname);
						damage_type = LOC_CDAM;
						//TempFile bloody gibbing
						if (mod == MOD_SNIPER && sv_gib->value)
							ThrowGib (targ, "models/objects/gibs/sm_meat/tris.md2", damage, GIB_ORGANIC);
					}
					//AQ2:TNG Slicer Last Damage Location
					if (INV_AMMO(targ, KEV_NUM) && mod != MOD_KNIFE && mod != MOD_KNIFE_THROWN) {
						attacker->client->resp.last_damaged_part = LOC_KVLR_VEST;
						if (!teamplay->value || team_round_going || stats_afterround->value)
							attacker->client->resp.stats_locations[LOC_KVLR_VEST]++; // TNG Stats
					} else {
						attacker->client->resp.last_damaged_part = LOC_CDAM;
						if (!teamplay->value || team_round_going || stats_afterround->value)
							attacker->client->resp.stats_locations[LOC_CDAM]++; // TNG Stats
					}
					//AQ2:TNG END

				}
	      /*else
	         {   

	         // no mod to damage 
	         gi.cprintf(targ, PRINT_HIGH, "Head damage\n"); 
	         if (attacker->client) 
	         gi.cprintf(attacker, PRINT_HIGH, "You hit %s in the head\n", targ->client->pers.netname); 
	         damage_type = LOC_HDAM;
	         gi.sound(targ, CHAN_VOICE, gi.soundindex("misc/headshot.wav"), 1, ATTN_NORM, 0);
	         } */
			}
			if (team_round_going && attacker->client && targ != attacker
				&& OnSameTeam (targ, attacker))
			{
				Add_TeamWound (attacker, targ, mod);
			}
		}
    }


	if (damage_type && !instant_dam)	// bullets but not vest hits
	{
		vec3_t temp;
		vec3_t temporig;
		//vec3_t forward;
		VectorMA (targ->s.origin, 50, dir, temp);
		//AngleVectors (attacker->client->v_angle, forward, NULL, NULL);
		VectorScale (dir, 20, temp);
		VectorAdd (point, temp, temporig);
		if (mod != MOD_SNIPER)
			spray_blood (targ, temporig, dir, damage, mod);
		else
			spray_sniper_blood (targ, temporig, dir);
	}

	if (mod == MOD_FALLING && !(targ->flags & FL_GODMODE) )
	{
		if (targ->client && targ->health > 0)
		{
			gi.cprintf (targ, PRINT_HIGH, "Leg damage\n");
			targ->client->leg_damage = 1;
			targ->client->leghits++;
		//      bleeding = 1; for testing
		}
	}


	// friendly fire avoidance
	// if enabled you can't hurt teammates (but you can hurt yourself)
	// knockback still occurs
	if (targ != attacker &&
		((deathmatch->value && ((int)dmflags->value & (DF_MODELTEAMS | DF_SKINTEAMS)))
		|| coop->value))
	{
		if (OnSameTeam (targ, attacker))
		{
			if ((int)dmflags->value & DF_NO_FRIENDLY_FIRE &&
				(team_round_going || !ff_afterround->value))
				damage = 0;
			else
				mod |= MOD_FRIENDLY_FIRE;
		}
	}

	meansOfDeath = mod;
	locOfDeath = damage_type;	// location

	client = targ->client;

	if (dflags & DAMAGE_BULLET)
		te_sparks = TE_BULLET_SPARKS;
	else
		te_sparks = TE_SPARKS;

	VectorNormalize (dir);

  // bonus damage for suprising a monster
  //      if (!(dflags & DAMAGE_RADIUS) && (targ->svflags & SVF_MONSTER) && (attacker->client) && (!targ->enemy) && (targ->health > 0))
  //              damage *= 2;

	if (targ->flags & FL_NO_KNOCKBACK)
		knockback = 0;

	// figure momentum add
	if (!(dflags & DAMAGE_NO_KNOCKBACK))
	{
		if ((knockback) && (targ->movetype != MOVETYPE_NONE)
			&& (targ->movetype != MOVETYPE_BOUNCE)
			&& (targ->movetype != MOVETYPE_PUSH)
			&& (targ->movetype != MOVETYPE_STOP))
		{
			vec3_t kvel, flydir;
			float mass;

			if (mod != MOD_FALLING)
			{
				VectorCopy (dir, flydir);
				flydir[2] += 0.4f;
			}

			if (targ->mass < 50)
				mass = 50;
			else
				mass = targ->mass;

			if (targ->client && attacker == targ)
				VectorScale (flydir, 1600.0 * (float) knockback / mass, kvel);	// the rocket jump hack...
			else
				VectorScale (flydir, 500.0 * (float) knockback / mass, kvel);

			// FB
			//if (mod == MOD_KICK )
			//{
			//        kvel[2] = 0;
			//}

			VectorAdd (targ->velocity, kvel, targ->velocity);
		}
	}

	take = damage;
	save = 0;

	// check for godmode
	if ((targ->flags & FL_GODMODE) && !(dflags & DAMAGE_NO_PROTECTION))
	{
		take = 0;
		save = damage;
		SpawnDamage (te_sparks, point, normal, save);
	}

	// zucc don't need this stuff, but to remove it need to change how damagefeedback works with colors

	// check for invincibility
	if ((client && client->invincible_framenum > level.framenum)
		&& !(dflags & DAMAGE_NO_PROTECTION))
	{
		if (targ->pain_debounce_time < level.time)
		{
			gi.sound (targ, CHAN_ITEM, gi.soundindex ("items/protect4.wav"), 1,
				ATTN_NORM, 0);
			targ->pain_debounce_time = level.time + 2;
		}
		take = 0;
		save = damage;
	}

	psave = CheckPowerArmor (targ, point, normal, take, dflags);
	take -= psave;

	asave = CheckArmor (targ, point, normal, take, te_sparks, dflags);
	take -= asave;

	//treat cheat/powerup savings the same as armor
	asave += save;

	// team damage avoidance
	if (!(dflags & DAMAGE_NO_PROTECTION) && CheckTeamDamage (targ, attacker))
		return;
	if ((mod == MOD_M3) || (mod == MOD_HC)
	|| (mod == MOD_HELD_GRENADE) || (mod == MOD_HG_SPLASH)
	|| (mod == MOD_G_SPLASH) || (mod == MOD_BREAKINGGLASS))
	{
		//FB 6/3/99 - shotgun damage report stuff
		int playernum = targ - g_edicts;
		playernum--;
		if (playernum >= 0 && playernum <= game.maxclients - 1)
			*(took_damage + playernum) = 1;
		//FB 6/3/99

		bleeding = 1;
		instant_dam = 0;
	}

  /*        if ( (mod == MOD_M3) || (mod == MOD_HC) )
     {
     instant_dam = 1;            
     remain = take % 2;
     take = (int)(take/2); // balances out difference in how action and axshun handle damage/bleeding

     }
   */

	if (ctf->value)
		CTFCheckHurtCarrier (targ, attacker);

	// do the damage
	if (take)
	{
		// zucc added check for stopAP, if it hit a vest we want sparks
		if (((targ->svflags & SVF_MONSTER) || (client)) && !do_sparks)
			SpawnDamage (TE_BLOOD, point, normal, take);
		else
			SpawnDamage (te_sparks, point, normal, take);

		// all things that have at least some instantaneous damage, i.e. bruising/falling
		if (instant_dam)
			targ->health = targ->health - take;

		if (targ->health <= 0)
		{
			if (client && attacker->client)
			{
				//Added these here also, if this is the last shot and before shots is from
				//different attacker, msg's would go to wrong client -M
				if (!OnSameTeam (attacker, targ))
					attacker->client->resp.damage_dealt += damage;
			
				client->attacker = attacker;
				client->attacker_mod = mod;
				client->attacker_loc = damage_type;
			}

			if ((targ->svflags & SVF_MONSTER) || (client))
				targ->flags |= FL_NO_KNOCKBACK;
			Killed (targ, inflictor, attacker, take, point);
			return;
		}
	}

	if (targ->svflags & SVF_MONSTER)
	{
		M_ReactToDamage (targ, attacker);
		if (!(targ->monsterinfo.aiflags & AI_DUCKED) && (take))
		{
			targ->pain (targ, attacker, knockback, take);
			// nightmare mode monsters don't go into pain frames often
			if (skill->value == 3)
				targ->pain_debounce_time = level.time + 5;
		}
	}
	else if (client)
	{
		if (!(targ->flags & FL_GODMODE) && (take))
			targ->pain (targ, attacker, knockback, take);
	}
	else if (take)
	{
		if (targ->pain)
			targ->pain (targ, attacker, knockback, take);
	}

	// add to the damage inflicted on a player this frame
	// the total will be turned into screen blends and view angle kicks
	// at the end of the frame
	if (client)
	{
		client->damage_parmor += psave;
		client->damage_armor += asave;
		client->damage_blood += take;
		client->damage_knockback += knockback;
		//zucc handle adding bleeding here
		if (damage_type && bleeding)	// one of the hit location weapons
		{
			/* zucc add in partial bleeding, changed
			if ( client->bleeding < 4*damage*BLEED_TIME )
			{
			client->bleeding = 4*damage*BLEED_TIME + client->bleeding/2;

			}
			else
			{
			client->bleeding += damage*BLEED_TIME*2;

			} */
			client->bleeding += damage * BLEED_TIME;
			VectorSubtract (point, targ->absmax, targ->client->bleedloc_offset);
			//VectorSubtract(point, targ->s.origin,  client->bleedloc_offset);

		}
		else if (bleeding)
		{
			/*
			if ( client->bleeding < damage*BLEED_TIME )
			{
			client->bleeding = damage*BLEED_TIME;
			//client->bleedcount = 0;
			} */
			client->bleeding += damage * BLEED_TIME;
			VectorSubtract (point, targ->absmax, targ->client->bleedloc_offset);
			//VectorSubtract(point, targ->s.origin,  client->bleedloc_offset);

		}
		if (attacker->client)
		{
			if (!OnSameTeam (attacker, targ))
				attacker->client->resp.damage_dealt += damage;

			client->attacker = attacker;
			client->attacker_mod = mod;
			client->attacker_loc = damage_type;
			client->push_timeout = 50;
			//VectorCopy(dir, client->bleeddir );
			//VectorCopy(point, client->bleedpoint );
			//VectorCopy(normal, client->bleednormal);

		}
		VectorCopy (point, client->damage_from);
	}
}


/*
  ============
  T_RadiusDamage
  ============
*/
void
T_RadiusDamage (edict_t * inflictor, edict_t * attacker, float damage,
		edict_t * ignore, float radius, int mod)
{
	float points;
	edict_t *ent = NULL;
	vec3_t v;
	vec3_t dir;

	while ((ent = findradius (ent, inflictor->s.origin, radius)) != NULL)
	{
		if (ent == ignore)
			continue;
		if (!ent->takedamage)
			continue;

		VectorAdd (ent->mins, ent->maxs, v);
		VectorMA (ent->s.origin, 0.5, v, v);
		VectorSubtract (inflictor->s.origin, v, v);
		points = damage - 0.5 * VectorLength (v);
		//zucc reduce damage for crouching, max is 32 when standing
		if (ent->maxs[2] < 20)
		{
			points = points * 0.5;	// hefty reduction in damage
		}
		//if (ent == attacker)
		//points = points * 0.5; 
		if (points > 0)
		{
#ifdef _DEBUG
			if (0 == Q_stricmp (ent->classname, "func_explosive"))
			{
				CGF_SFX_ShootBreakableGlass (ent, inflictor, 0, mod);
			}
			else
#endif
			if (CanDamage (ent, inflictor))
			{
				VectorSubtract (ent->s.origin, inflictor->s.origin, dir);
				// zucc scaled up knockback(kick) of grenades
				T_Damage (ent, inflictor, attacker, dir, ent->s.origin,
				vec3_origin, (int) (points * .75),
				(int) (points * .75), DAMAGE_RADIUS, mod);
			}
		}
	}
}
