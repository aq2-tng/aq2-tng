///////////////////////////////////////////////////////////////////////
//
//  ACE - Quake II Bot Base Code
//
//  Version 1.0
//
//  Original file is Copyright(c), Steve Yeager 1998, All Rights Reserved
//
//
//	All other files are Copyright(c) Id Software, Inc.
////////////////////////////////////////////////////////////////////////
/*
 * $Header: /LTK2/src/acesrc/acebot_ai.c 6     29/02/00 11:14 Riever $
 *
 * $History: acebot_ai.c $
 * 
 * *****************  Version 6  *****************
 * User: Riever       Date: 29/02/00   Time: 11:14
 * Updated in $/LTK2/src/acesrc
 * Made bot ChooseWeapon function qboolean so it can signal when no weapon
 * was ready for use.
 * Moved weapon selection call to be inside attack function so bot knows
 * whether it has to fire a weapon or use a kick attack.
 * 
 * *****************  Version 5  *****************
 * User: Riever       Date: 24/02/00   Time: 3:05
 * Updated in $/LTK2/src/acesrc
 * Added bot say command support. Changed LTG weighting method.
 * 
 * *****************  Version 4  *****************
 * User: Riever       Date: 23/02/00   Time: 23:16
 * Updated in $/LTK2/src/acesrc
 * Removed random door opening code - bots now MUST have a routefile to
 * get through doors.
 * Added support for response to 'treport' radio message.
 * 
 * *****************  Version 3  *****************
 * User: Riever       Date: 17/02/00   Time: 17:53
 * Updated in $/LTK2/src/acesrc
 * Fixed item list to be in the right order!
 * 
 */
///////////////////////////////////////////////////////////////////////
//
//  acebot_ai.c -      This file contains all of the 
//                     AI routines for the ACE II bot.
//
//
// NOTE: I went back and pulled out most of the brains from
//       a number of these functions. They can be expanded on 
//       to provide a "higher" level of AI. 
////////////////////////////////////////////////////////////////////////

#include "../g_local.h"
#include "../m_player.h"

#include "acebot.h"
#include "botchat.h"

void	ACEAI_Cmd_Choose( edict_t *ent, char *s);
void	ACEMV_ChangeBotAngle (edict_t *ent);
//RiEvEr AQ2 Radio use
void RadioBroadcast(edict_t *ent, int partner, char *msg);
//R AQ2

///////////////////////////////////////////////////////////////////////
// Main Think function for bot
///////////////////////////////////////////////////////////////////////
void ACEAI_Think (edict_t *self)
{
	usercmd_t ucmd;
	//int curplayer = 0;
	int numenemies = 0;
	//trace_t tr;
	qboolean see_enemies = false;

	// Set up client movement
	VectorCopy(self->client->ps.viewangles,self->s.angles);
	VectorSet (self->client->ps.pmove.delta_angles, 0, 0, 0);
	memset (&ucmd, 0, sizeof (ucmd));
	self->enemy = NULL;
	self->movetarget = NULL;
	
	// Force respawn 
	if (self->deadflag == DEAD_DEAD)
	{
		self->client->buttons = 0;
		ucmd.buttons = BUTTON_ATTACK;
	}

	// Teamplay spectator? Reenabled by Werewolf
//	if( (self->solid == SOLID_NOT) && (teamplay->value) )
//		return;

	if( self->state == STATE_WANDER && self->wander_timeout < level.framenum )
	  ACEAI_PickLongRangeGoal(self); // pick a new long range goal

	//RiEvEr Radio Use
	if( ! self->teamReportedIn && (self->lastRadioTime < level.framenum - 2.0 * HZ) )
	{
		RadioBroadcast(self, 0, "reportin");
		BOTUT_Cmd_Say_f( self, "Equipped with %W and %I. Current health %H.");
		self->lastRadioTime = level.framenum;
		self->teamReportedIn = true;
	}
	//R RU

	// In teamplay pick a random node
	if( self->state == STATE_POSITION )
	{
		if( level.framenum >= self->teamPauseTime)
		{
			// We've waited long enough - let's go kick some ass!
			self->state = STATE_WANDER;
		}
		// Don't go here too often
		if( self->goal_node == INVALID || self->wander_timeout < level.framenum )
			ACEAI_PickLongRangeGoal(self);
	}
	

	// Kill the bot if completely stuck somewhere
	if(VectorLength(self->velocity) > 37) //
		self->suicide_timeout = level.framenum + 10.0 * HZ;


	if( self->suicide_timeout < level.framenum && !teamplay->value )
	{
		self->health = 0;
		player_die (self, self, self, 100000, vec3_origin);
	}

	// Find any short range goal
	ACEAI_PickShortRangeGoal(self);
	
	if (self->old_health < self->health)
		self->old_health = self->health;

	see_enemies = ACEAI_FindEnemy(self, &numenemies);

/*	// Check how many enemies we have in sight
	if ( (self->recheck_timeout < level.time) )
	{
		numenemies = 0;
		self->recheck_timeout = level.time + (3*random());

		for (curplayer=0; curplayer<num_players; curplayer++)
		{
			if( (players[curplayer]->team != self->team) )
				(players[curplayer]->health > 0) && 
				(players[curplayer] != NULL) && 
				(players[curplayer] != self) )
			{
				tr = gi.trace (self->s.origin, NULL, NULL, players[curplayer]->s.origin, self, MASK_SOLID|MASK_OPAQUE);
				if (tr.fraction >= 0.9)
				{
					numenemies += 1;
					LTK_Say(self, "!");
					if (numenemies>1)
						break;
				}
			}
		}
		
		
		if (numenemies>1)
		{
			LTK_Say(self, "Whoa! Many hostiles in sight!");
			if (self->state != STATE_FLEE)
			{
				self->state = STATE_FLEE;
				self->goal_node = INVALID;
				ACEAI_PickSafeGoal(self);
			}

		}

	}
*/

	// Look for enemies
	if( (see_enemies) && 
		(self->client->weaponstate != WEAPON_RELOADING) && 
                (((teamplay->value) && (lights_camera_action <= 1)) || !teamplay->value))
		//&& (self->state != STATE_FLEE) )
	{
		// Moved to the attack function
//		ACEAI_ChooseWeapon(self);
		// Stop the "spin 180 degrees and shoot" attack :)
		// We need to change this to allow the bot to turn and look again
		// so that he can find enemies who are firing at him.
		if( infront( self, self->enemy ))
		{
			ACEMV_Attack (self, &ucmd);
		}
		else// if (self->state != STATE_FLEE)
		//		turn and look!
		{
			self->movetarget = self->enemy;
			VectorSubtract (self->enemy->s.origin, self->s.origin, self->move_vector);
			//self->s.angles[YAW] += 90;
			ACEMV_ChangeBotAngle ( self );
		}

//		if( self->health < self->old_health)
//		{
//			if (self->state != STATE_FLEE)
//			{
//				self->state = STATE_FLEE; //Look for a nearby node that doesn't have LOS to an enemy.
//				self->goal_node = INVALID;
//				ACEAI_PickSafeGoal(self);
//			}
//		}
	}
	else
	{
		// Are we hurt?
		if( (self->health < self->old_health) ||
			(self->client->weaponstate == WEAPON_RELOADING) )
		{
			Cmd_Bandage_f ( self );
			if (self->state != STATE_FLEE)
			{
				self->state = STATE_FLEE;
				self->goal_node = INVALID;
				ACEAI_PickSafeGoal(self);
			}
			self->old_health = self->health;

		}


		// Execute the move, or wander
		if(self->state == STATE_WANDER)
			ACEMV_Wander(self,&ucmd);
		else if( (self->state == STATE_MOVE) || (self->state == STATE_POSITION) || (self->state == STATE_FLEE) )
			ACEMV_Move(self,&ucmd);
		else if (self->state == STATE_COVER)
		{
			if( self->wander_timeout < level.framenum )
			{
				self->state = STATE_WANDER;
			}
		}

	}

/*	if (self->state == STATE_FLEE)
		if (ACEND_DistanceToTargetNode(self) < 32)
	{


	}
*/
//AQ2 ADD
	// Check if there's a door nearby that we cannot trace (like in ACTCITY3)
	//Added by Werewolf
	if( self->last_door_time < level.framenum - (2.0 + random()) * HZ )
	{
	trace_t	tTrace;
	vec3_t	vStart, vDest;
	VectorCopy( nodes[self->current_node].origin, vStart);
	VectorCopy( nodes[self->next_node].origin, vDest);
	// See if we have a clear shot at it
	tTrace = gi.trace( vStart, tv(-16,-16,-8), tv(16,16,8), vDest, self, MASK_PLAYERSOLID);
	  if( tTrace.fraction <1.0 )
	  {
		if(		
			(strcmp(tTrace.ent->classname,"func_door_rotating")!=0)
			&&	(strcmp(tTrace.ent->classname,"func_door")!=0 )
			) //No door found, so we can mess with the opendoor func without any danger
		{
			// Toggle any door that may be nearby
			Cmd_OpenDoor_f ( self );
			self->last_door_time = level.framenum + random() * 2.0 * HZ;
		}
	  }
	}
//AQ2 END

	//debug_printf("State: %d\n",self->state);

	// set approximate ping
	ucmd.msec = 1000 / BOT_FPS;
	self->client->ps.pmove.pm_time = ucmd.msec / 8;
	self->client->ping = ucmd.msec;

	// set bot's view angle
	ucmd.angles[PITCH] = ANGLE2SHORT(self->s.angles[PITCH]);
	ucmd.angles[YAW] = ANGLE2SHORT(self->s.angles[YAW]);
	ucmd.angles[ROLL] = ANGLE2SHORT(self->s.angles[ROLL]);
	
	// send command through id's code
	ClientThink (self, &ucmd);
	
	self->nextthink = level.framenum + (game.framerate / BOT_FPS);
}

///////////////////////////////////////////////////////////////////////
// Evaluate the best long range goal and send the bot on
// its way. This is a good time waster, so use it sparingly. 
// Do not call it for every think cycle.
///////////////////////////////////////////////////////////////////////
void ACEAI_PickLongRangeGoal(edict_t *self)
{

	int i = 0;
	int node = 0;
	float weight = 0.f, best_weight = 0.0;
	int current_node = 0, goal_node = 0;
	edict_t *goal_ent = NULL;
	float cost = 0.f;
	
	// look for a target 
	current_node = ACEND_FindClosestReachableNode(self,NODE_DENSITY,NODE_ALL);

	self->current_node = current_node;

	// Even in teamplay, we wander if no valid node
	if(current_node == -1)
	{
		self->state = STATE_WANDER;
		self->wander_timeout = level.framenum + 1.0 * HZ;
		self->goal_node = -1;
		return;
	}

	//Added by Werewolf
	if( self->state == STATE_WANDER )
	self->state = STATE_POSITION;

	//======================
	// Teamplay POSITION state
	//======================
	if( self->state == STATE_POSITION )
	{
		int counter = 0;
		cost = INVALID;
		self->goal_node = INVALID;

		// Pick a random node to go to
		while( cost == INVALID && counter < 20) // Don't look for too many
		{
			counter++;
			i = (int)(random() * numnodes -1);	// Any of the current nodes will do
			cost = ACEND_FindCost(current_node, i);

			if(cost == INVALID || cost < 2) // ignore invalid and very short hops
			{
				cost = INVALID;
				i = INVALID;
				continue;
			}
		}
		// We have a target node - just go there!
		if( i != INVALID )
		{
			self->state = STATE_MOVE;
			self->tries = 0; // Reset the count of how many times we tried this goal
			ACEND_SetGoal(self,i);
			self->wander_timeout = level.framenum + 1.0 * HZ;
			return;
		}
	}

	///////////////////////////////////////////////////////
	// Items
	///////////////////////////////////////////////////////
	for(i=0;i<num_items;i++)
	{
		if(item_table[i].ent == NULL || item_table[i].ent->solid == SOLID_NOT) // ignore items that are not there.
			continue;
		
		cost = ACEND_FindCost(current_node,item_table[i].node);
		
		if(cost == INVALID || cost < 2) // ignore invalid and very short hops
			continue;
	
		weight = ACEIT_ItemNeed(self, item_table[i].item);


		weight *= ( (rand()%5) +1 ); // Allow random variations
		weight /= cost; // Check against cost of getting there
				
		if(weight > best_weight && item_table[i].node != INVALID)
		{
			best_weight = weight;
			goal_node = item_table[i].node;
			goal_ent = item_table[i].ent;
		}
	}

	///////////////////////////////////////////////////////
	// Players
	///////////////////////////////////////////////////////
	// This should be its own function and is for now just
	// finds a player to set as the goal.
	for(i=0;i<num_players;i++)
	{
		if( (players[i] == self) || (players[i]->solid == SOLID_NOT) )
			continue;

		// If it's dark and he's not already our enemy, ignore him
		if( self->enemy && players[i] != self->enemy)
		{
//Disabled by Werewolf
//			if( players[i]->light_level < 30)
//				continue;
		}

		node = ACEND_FindClosestReachableNode(players[i],NODE_DENSITY,NODE_ALL);
		// RiEvEr - bug fixing
		if( node == INVALID)
			cost = INVALID;
		else
			cost = ACEND_FindCost(current_node, node);

		if(cost == INVALID || cost < 3) // ignore invalid and very short hops
			continue;

		// Stop the centipede effect in teamplay
		if( teamplay->value )
		{
			// Check it's an enemy
			// If not an enemy, don't follow him
			if( OnSameTeam( self, players[i]))
				weight = 0.0;
			else
				weight = 1.5;	//Werewolf: Was 0.3
		}
		else
		  weight = 0.8;		//Werewolf: Was 0.3
		
		weight *= ( (rand()%5) +1 ); // Allow random variations
		weight /= cost; // Check against cost of getting there
		
		if(weight > best_weight && node != INVALID)
		{		
			best_weight = weight;
			goal_node = node;
			goal_ent = players[i];
		}	
	}

	
//Added by Werewolf
//-------------------------------
	if(best_weight == 0.0 || goal_node == INVALID )
	{
		for(i=0;i<num_players;i++)
		{
			if( (players[i] == self) || (players[i]->solid == SOLID_NOT) || (goal_node != INVALID))
				continue;
			if( teamplay->value )
			{
				// Check it's an enemy
				// If not an enemy, don't follow him
				if(!OnSameTeam( self, players[i]))
				{
					goal_node = ACEND_FindClosestReachableNode(players[i],NODE_DENSITY*50,NODE_ALL);
					goal_ent = players[i];
				}
			}
			else
			{
				goal_node = ACEND_FindClosestReachableNode(players[i],NODE_DENSITY*50,NODE_ALL);
				goal_ent = players[i];
			}
		}
	}
//-------------------------------

	// If do not find a goal, go wandering....
	if(best_weight == 0.0 || goal_node == INVALID )
	{
		self->goal_node = INVALID;
		self->state = STATE_WANDER;
		self->wander_timeout = level.framenum + 1.0 * HZ;
		if(debug_mode)
			debug_printf("%s did not find a LR goal, wandering.\n",self->client->pers.netname);
		return; // no path? 
	}
	
	// OK, everything valid, let's start moving to our goal.
	self->state = STATE_MOVE;
	self->tries = 0; // Reset the count of how many times we tried this goal
	 
	if(goal_ent != NULL && debug_mode)
		debug_printf("%s selected a %s at node %d for LR goal.\n",self->client->pers.netname, goal_ent->classname, goal_node);

	ACEND_SetGoal(self,goal_node);

}


///////////////////////////////////////////////////////////////////////
// Pick best goal based on importance and range. This function
// overrides the long range goal selection for items that
// are very close to the bot and are reachable.
///////////////////////////////////////////////////////////////////////
void ACEAI_PickShortRangeGoal(edict_t *self)
{
	edict_t *target = NULL;
	float weight = 0.f, best_weight = 0.0;
	edict_t *best = NULL;
	int index = 0;
	
	// look for a target (should make more efficient later)
	target = findradius(NULL, self->s.origin, 200);
	
	while(target)
	{
		if(target->classname == NULL)
			return;
		
		// Missle avoidance code
		// Set our movetarget to be the rocket or grenade fired at us. 
		if(strcmp(target->classname,"rocket")==0 || strcmp(target->classname,"grenade")==0)
		{
			if(debug_mode) 
				debug_printf("ROCKET ALERT!\n");

			self->movetarget = target;
			return;
		}
	
		if (ACEIT_IsReachable(self,target->s.origin))
		{
			if (infront(self, target))
			{
				index = ACEIT_ClassnameToIndex(target->classname);
				weight = ACEIT_ItemNeed(self, index);
				
				if(weight > best_weight)
				{
					best_weight = weight;
					best = target;
				}
			}
		}

		// next target
		target = findradius(target, self->s.origin, 200);
	}

	if(best_weight)
	{
		self->movetarget = best;
		
		if(debug_mode && self->goalentity != self->movetarget)
			debug_printf("%s selected a %s for SR goal.\n",self->client->pers.netname, self->movetarget->classname);
		
		self->goalentity = best;

	}

}



//Werewolf
void ACEAI_PickSafeGoal(edict_t *self)
{

	int i;
	float best_weight=0.0;
	int current_node,goal_node = INVALID;
	
	// look for a target 
	current_node = ACEND_FindClosestReachableNode(self,NODE_DENSITY,NODE_ALL);

	self->current_node = current_node;

	i = INVALID;
	i = ACEND_FindClosestReachableSafeNode(self,NODE_DENSITY,NODE_ALL);
	if( i != INVALID )
	{
		self->state = STATE_FLEE;
		self->tries = 0; // Reset the count of how many times we tried this goal
		ACEND_SetGoal(self,i);
		self->wander_timeout = level.framenum + 2.0 * HZ;
//		LTK_Say (self, "Under fire, extracting!");

		return;
	}


	

	// If do not find a goal, go wandering....
	if(best_weight == 0.0 || goal_node == INVALID )
	{
		self->goal_node = INVALID;
		self->state = STATE_WANDER;
		self->wander_timeout = level.framenum + 1.0 * HZ;
		if(debug_mode)
			debug_printf("%s did not find a LR goal, wandering.\n",self->client->pers.netname);
		return; // no path? 
	}
	
	// OK, everything valid, let's start moving to our goal.
	self->state = STATE_FLEE;
	self->tries = 0; // Reset the count of how many times we tried this goal
	 
//	if(goal_ent != NULL && debug_mode)
//		debug_printf("%s selected a %s at node %d for LR goal.\n",self->client->pers.netname, goal_ent->classname, goal_node);

	ACEND_SetGoal(self,goal_node);

}
//-----------------------------------------------------------




///////////////////////////////////////////////////////////////////////
// Scan for enemy (simplifed for now to just pick any visible enemy)
///////////////////////////////////////////////////////////////////////
// Modified by RiEvEr
// Chooses nearest enemy or last person to shoot us
//
qboolean ACEAI_FindEnemy(edict_t *self, int *total)
{
	int i;
	edict_t		*bestenemy = NULL;
	float		bestweight = 99999;
	float		weight;
	vec3_t		dist;

/*	// If we already have an enemy and it is the last enemy to hurt us
	if (self->enemy && 
		(self->enemy == self->client->attacker) &&
		(!self->enemy->deadflag) &&
		(self->enemy->solid != SOLID_NOT)
		)
	{
		return true;
	}
*/
	for(i=0;i<=num_players;i++)
	{
		if(players[i] == NULL || players[i] == self || 
		   players[i]->solid == SOLID_NOT)
		   continue;
	
		// If it's dark and he's not already our enemy, ignore him
		if( self->enemy && players[i] != self->enemy)
		{
			if( players[i]->light_level < 30)
				continue;
		}

/*		if(ctf->value && 
		   self->client->resp.ctf_team == players[i]->client->resp.ctf_team)
		   continue;*/
// AQ2 ADD
		if(teamplay->value && OnSameTeam( self, players[i]) )
		   continue;
// AQ2 END

		if((players[i]->deadflag == DEAD_NO) && ai_visible(self, players[i]) && 
			gi.inPVS(self->s.origin, players[i]->s.origin)	)
		{
// RiEvEr
			// Now we assess this enemy
			VectorSubtract(self->s.origin, players[i]->s.origin, dist);
			weight = VectorLength( dist );

			// Can we hear their weapon firing?
			qboolean weapon_loud = false;
			if( players[i]->client->weaponstate == WEAPON_FIRING )
			{
				switch( players[i]->client->weapon->typeNum )
				{
					case DUAL_NUM:
					case M4_NUM:
					case M3_NUM:
					case HC_NUM:
						weapon_loud = true;
						break;
					case KNIFE_NUM:
					case GRENADE_NUM:
						break;
					default:
						weapon_loud = !INV_AMMO( players[i], SIL_NUM );
				}
			}

			// Can we see this enemy, or are they making noise that we shouldn't ignore?
			if( infront( self, players[i] ) || weapon_loud || ((weight < 300) && !INV_AMMO( players[i], SLIP_NUM )) )
			{
				total+=1;
				// See if it's better than what we have already
				if (weight < bestweight)
				{
					bestweight = weight;
					bestenemy = players[i];
				}
			}
		}
	}
	// If we found a good enemy set it up
	if( bestenemy)
	{
		self->enemy = bestenemy;
		return true;
	}
	// Check if we've been shot from behind or out of view
	if( self->client->attacker )
	{
		// Check if it was recent
		if( self->client->push_timeout > 0)
		{
			if(!self->client->attacker->deadflag && ai_visible(self, self->client->attacker) && 
				gi.inPVS(self->s.origin, self->client->attacker->s.origin)	)
			{
				self->enemy = self->client->attacker;
				return true;
			}
		}
	}
//R
	// Otherwise signal "no enemy available"
	return false;
  
}

///////////////////////////////////////////////////////////////////////
// Hold fire with RL/BFG?
///////////////////////////////////////////////////////////////////////
//@@ Modify this to check for hitting teammates in teamplay games.
//Modification request gladly completed by Werewolf :)
qboolean ACEAI_CheckShot(edict_t *self)
{
	trace_t tr;

//AQ2	tr = gi.trace (self->s.origin, tv(-8,-8,-8), tv(8,8,8), self->enemy->s.origin, self, MASK_OPAQUE);
//	tr = gi.trace (self->s.origin, tv(-8,-8,-8), tv(8,8,8), self->enemy->s.origin, self, MASK_SOLID|MASK_OPAQUE);
//	tr = gi.trace (self->s.origin, vec3_origin, vec3_origin, self->enemy->s.origin, self, MASK_SOLID|MASK_OPAQUE); // Werewolf: was enabled
	tr = gi.trace (self->s.origin, tv(-8,-8,-8), tv(8,8,8), self->enemy->s.origin, self, MASK_PLAYERSOLID); //Check for friendly fire

	//We would hit something
	if (tr.fraction < 0.9)
		//If we're in teamplay the the accidentally hit player is a teammate, hold fire
		if( (teamplay->value) && (OnSameTeam( self, tr.ent)) )
			return false;
		//In deathmatch, don't shoot through glass and stuff
//		else if ((!teamplay->value) && (tr.ent->solid==SOLID_BSP))
//			return false;

//	tr = gi.trace (self->s.origin, vec3_origin, vec3_origin, self->enemy->s.origin, self, MASK_SOLID|MASK_OPAQUE); // Check for Hard stuff
	tr = gi.trace (self->s.origin, tv(-8,-8,-8), tv(8,8,8), self->enemy->s.origin, self, MASK_SOLID|MASK_OPAQUE);
	if (tr.fraction < 0.9)
		if (tr.ent->solid==SOLID_BSP)
			return false;



	//else shoot
	return true;
}

///////////////////////////////////////////////////////////////////////
// Choose the best weapon for bot (simplified)
// Modified by Werewolf to use sniper zoom
///////////////////////////////////////////////////////////////////////
//RiEvEr - Changed to boolean.
qboolean ACEAI_ChooseWeapon(edict_t *self)
{	
	float range;
	vec3_t v;
	
	// if no enemy, then what are we doing here?
	if(!self->enemy)
		return (true);
//AQ2 CHANGE	
	// Currently always favor the dual pistols!
	//@@ This will become the "bot choice" weapon
//	if(ACEIT_ChangeDualSpecialWeapon(self,FindItem(DUAL_NAME)))
//   	   return;
//AQ2 END

	// Base selection on distance.
	VectorSubtract (self->s.origin, self->enemy->s.origin, v);
	range = VectorLength(v);
		

	// Longest range 
	if(range > 1000)
	{
		if(ACEIT_ChangeSniperSpecialWeapon(self,FindItem(SNIPER_NAME)))
		{
			if (self->client->resp.sniper_mode!=SNIPER_2X)
			{
			gi.sound(self, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
			self->client->resp.sniper_mode = SNIPER_2X;
			self->client->desired_fov = 45;
			}
			return (true);
		}
		
		if(ACEIT_ChangeM3SpecialWeapon(self,FindItem(M3_NAME)))
			return (true);
		
		if(ACEIT_ChangeM4SpecialWeapon(self,FindItem(M4_NAME)))
			return (true);
		
		if(ACEIT_ChangeMP5SpecialWeapon(self,FindItem(MP5_NAME)))
   		   return (true);

		if(ACEIT_ChangeMK23SpecialWeapon(self,FindItem(MK23_NAME)))
   		   return (true);
	}
	
	// Longer range 
	if(range > 700)
	{		
		if(ACEIT_ChangeM3SpecialWeapon(self,FindItem(M3_NAME)))
			return (true);
		
		if(ACEIT_ChangeM4SpecialWeapon(self,FindItem(M4_NAME)))
			return (true);
		
		if(ACEIT_ChangeMP5SpecialWeapon(self,FindItem(MP5_NAME)))
   		   return (true);

		if(ACEIT_ChangeSniperSpecialWeapon(self,FindItem(SNIPER_NAME)))
		{
			if (self->client->resp.sniper_mode!=SNIPER_2X)
			{
			gi.sound(self, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
			self->client->resp.sniper_mode = SNIPER_2X;
			self->client->desired_fov = 45;
			}
			return (true);
		}
	
		if(ACEIT_ChangeMK23SpecialWeapon(self,FindItem(MK23_NAME)))
   		   return (true);

	}
	
	// Long range 
	if(range > 500)
	{		
		if(ACEIT_ChangeMP5SpecialWeapon(self,FindItem(MP5_NAME)))
   		   return (true);

		if(ACEIT_ChangeM3SpecialWeapon(self,FindItem(M3_NAME)))
			return (true);
		
		if(ACEIT_ChangeM4SpecialWeapon(self,FindItem(M4_NAME)))
			return (true);
		
		if(ACEIT_ChangeSniperSpecialWeapon(self,FindItem(SNIPER_NAME)))
		{
			if (self->client->resp.sniper_mode!=SNIPER_1X)
			{
			gi.sound(self, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
			self->client->resp.sniper_mode = SNIPER_1X;
			self->client->desired_fov = 90;
			}
			return (true);
		}
	
		if(ACEIT_ChangeMK23SpecialWeapon(self,FindItem(MK23_NAME)))
   		   return (true);

		if(ACEIT_ChangeWeapon(self,FindItem(GRENADE_NAME)))
			return (true);
	}
	
	// Medium range 
	if(range > 200)
	{		
		if(ACEIT_ChangeM4SpecialWeapon(self,FindItem(M4_NAME)))
			return (true);
		
		if(ACEIT_ChangeMP5SpecialWeapon(self,FindItem(MP5_NAME)))
   		   return (true);

		if(ACEIT_ChangeM3SpecialWeapon(self,FindItem(M3_NAME)))
			return (true);
		
		if(ACEIT_ChangeWeapon(self,FindItem(GRENADE_NAME)))
			return (true);

		if(ACEIT_ChangeDualSpecialWeapon(self,FindItem(DUAL_NAME)))
			return (true);

		if(ACEIT_ChangeMK23SpecialWeapon(self,FindItem(MK23_NAME)))
   		   return (true);

		if(ACEIT_ChangeSniperSpecialWeapon(self,FindItem(SNIPER_NAME)))
		{
			if (self->client->resp.sniper_mode!=SNIPER_1X)
			{
			gi.sound(self, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
			self->client->resp.sniper_mode = SNIPER_1X;
			self->client->desired_fov = 90;
			}
			return (true);
		}
	
	}
	
	// Short range	   
	if(ACEIT_ChangeHCSpecialWeapon(self,FindItem(HC_NAME)))
		return (true);
	
	if(ACEIT_ChangeDualSpecialWeapon(self,FindItem(DUAL_NAME)))
		return (true);

	if(ACEIT_ChangeM3SpecialWeapon(self,FindItem(M3_NAME)))
		return (true);
	
	if(ACEIT_ChangeM4SpecialWeapon(self,FindItem(M4_NAME)))
		return (true);
	
	if(ACEIT_ChangeMP5SpecialWeapon(self,FindItem(MP5_NAME)))
   	   return (true);
	
	if(ACEIT_ChangeWeapon(self,FindItem(GRENADE_NAME)))
   	   return (true);

	if(ACEIT_ChangeMK23SpecialWeapon(self,FindItem(MK23_NAME)))
   	   return (true);
	
	if(ACEIT_ChangeWeapon(self,FindItem(KNIFE_NAME)))
   	   return (true);

	if(ACEIT_ChangeSniperSpecialWeapon(self,FindItem(SNIPER_NAME)))
		{
			if (self->client->resp.sniper_mode!=SNIPER_1X)
			{
			gi.sound(self, CHAN_ITEM, gi.soundindex("misc/lensflik.wav"), 1, ATTN_NORM, 0);
			self->client->resp.sniper_mode = SNIPER_1X;
			self->client->desired_fov = 90;
			}
			return (true);
		}


	
	// We have no weapon available for use.
	if(debug_mode)
		safe_bprintf(PRINT_HIGH,"%s: No weapon available...\n",self->client->pers.netname);
	return (false);

}

void ACEAI_Cmd_Choose (edict_t *ent, char *s)
{
    // only works in teamplay
    if (!teamplay->value)
            return;
    
    if ( Q_stricmp(s, MP5_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(MP5_NAME);
    else if ( Q_stricmp(s, M3_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(M3_NAME);
    else if ( Q_stricmp(s, M4_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(M4_NAME);
    else if ( Q_stricmp(s, HC_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(HC_NAME);
    else if ( Q_stricmp(s, SNIPER_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(SNIPER_NAME);
    else if ( Q_stricmp(s, KNIFE_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(KNIFE_NAME);
    else if ( Q_stricmp(s, DUAL_NAME) == 0 )
            ent->client->pers.chosenWeapon = FindItem(DUAL_NAME);
    else if ( Q_stricmp(s, KEV_NAME) == 0 )
            ent->client->pers.chosenItem = FindItem(KEV_NAME);
    else if ( Q_stricmp(s, LASER_NAME) == 0 )
            ent->client->pers.chosenItem = FindItem(LASER_NAME);
    else if ( Q_stricmp(s, SLIP_NAME) == 0 )
            ent->client->pers.chosenItem = FindItem(SLIP_NAME);
    else if ( Q_stricmp(s, SIL_NAME) == 0 )
            ent->client->pers.chosenItem = FindItem(SIL_NAME);
    else if ( Q_stricmp(s, BAND_NAME) == 0 )
            ent->client->pers.chosenItem = FindItem(BAND_NAME);
}
