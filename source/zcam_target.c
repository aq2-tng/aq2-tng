//-----------------------------------------------------------------------------
//
// $Id: zcam_target.c,v 1.1 2002/09/04 11:23:10 ra Exp $
//
//-----------------------------------------------------------------------------
//
// $Log: zcam_target.c,v $
// Revision 1.1  2002/09/04 11:23:10  ra
// Added zcam to TNG and bumped version to 3.0
//
//
//-----------------------------------------------------------------------------
/* 
 *  ZCam v1.0.4
 *  Spectator Camera for Quake III Arena
 *  Copyright (C), 2001 by Avi "Zung!" Rozen
 *	
 *  http://www.telefragged.com/zungbang/zcam
 *
 *  Credits:
 *  FLIC  camera mode is based on code  taken from q2cam by Paul Jordan
 *  SWING camera mode is based on ideas taken from CreepCam for Quake I 
 *
 */

/* Camera Target Selection */

#include "g_local.h"
#include "zcam.h"

/* IsVisible:
 * Is player #1 visible by player #2 ? 
 */
qboolean IsVisible(edict_t * player1, edict_t * player2, float maxrange)
{
	vec3_t length;
	float distance;
	trace_t trace;

	// check for looking through non-transparent water
	if (!loc_CanSee(player1, player2))
		return false;

	PRETRACE ();
	trace = gi.trace (player1->s.origin, NULL, NULL, player2->s.origin, player1, MASK_SOLID);
	POSTTRACE ();

	VectorSubtract(player1->s.origin, player2->s.origin, length);
	distance = VectorLength(length);

	return ((maxrange == 0 || distance < maxrange) && trace.fraction == 1.0f);
}

/* NumPlayersVisible:
 * Return the number of players visible by the specified viewer.
 */
int NumPlayersVisible(edict_t * viewer, float maxrange)
{
	int count = 0;
	int i;

	if (viewer == NULL) {
		return 0;
	}

	for (i = 0; i < game.maxclients; i++) {
		if (viewer->s.number != i
		    && game.clients[i].pers.connected
		    && game.clients[i].resp.team != NOTEAM && game.clients[i].ps.pmove.pm_type != PM_DEAD) {
			if (IsVisible(viewer, &g_edicts[i], maxrange)) {
				count++;
			}
		}
	}

	return count;
}

/* PriorityTarget:
 * Override camera target selection. Returns the new favorite
 * player (or the one already selected).
 * Currently used to select flag carriers in CTF.
 */
edict_t *PriorityTarget(edict_t * target, qboolean * override)
{
	int i;
	edict_t *favorite = NULL;

	for (i = 0; i < game.maxclients; i++) {
		if (game.clients[i].pers.connected && game.clients[i].resp.team != NOTEAM) {
// LAGA		    && game.clients[i].ps.pmove.pm_type == PM_NORMAL && (game.clients[i].ps.powerups[PW_REDFLAG]
// LAGA								    || game.clients[i].ps.powerups[PW_BLUEFLAG])) {
			if (favorite == NULL
			    || NumPlayersVisible(favorite, MAX_VISIBLE_RANGE) < NumPlayersVisible(&g_edicts[i],
												  MAX_VISIBLE_RANGE)) {
				favorite = &g_edicts[i];
			}
		}
	}

	/* set override flag */
	if (favorite)
		*override = true;
	else
		*override = false;
	return (favorite) ? favorite : target;
}

/* PlayerToFollow:
 * Select camera target. Selects the player that sees the maximum
 * number of other players, unless a priority target exists (as 
 * determined by the function PriorityTarget above).
 */
edict_t *PlayerToFollow(edict_t * ent, qboolean * override)
{
	edict_t *viewer;
	edict_t *best = NULL;
	int i, players, best_count = -1;

	*override = false;

	for (i = 0; i < game.maxclients; i++) {
		/* don't switch to dead people */
		viewer = &g_edicts[i];

		if (!viewer->inuse || !viewer->client)
			continue;
		if (!viewer->client->pers.connected)
			continue;

		if (viewer->client->resp.team != NOTEAM && viewer->client->ps.pmove.pm_type == PM_NORMAL) {
			players = NumPlayersVisible(viewer, MAX_VISIBLE_RANGE);
			if (players > best_count) {
				best_count = players;
				best = viewer;
			} else if (players == best_count) {
				if (best->client->resp.score < viewer->client->resp.score) {
					best = viewer;
				}
			}
		}
	}

	if (best == NULL)
		return best;

	return PriorityTarget(best, override);
}

/* PlayerToTrack:
 *  select swing camera target in the following priority order:
 *  - enemy flag carrier
 *  - closest enemy player
 *  - own team flag carrier
 *  - closest own team player
 *  (in DM this amount to picking the closest player)
 */
edict_t *PlayerToTrack(edict_t * ent, edict_t * target1st)
{
	int i;
	edict_t *target2nd;
	edict_t *best1 = NULL;
	edict_t *best2 = NULL;
	vec3_t distance;
	float current, closest1 = -1.0F, closest2 = -1.0F;

	for (i = 0; i < game.maxclients; i++) {
		target2nd = &g_edicts[i];
		if (!target2nd->inuse || !target2nd->client)
			continue;
		if (target1st != target2nd
		    && target2nd->client->pers.connected
		    && target2nd->client->resp.team != NOTEAM
		    && target2nd->client->ps.pmove.pm_type == PM_NORMAL
		    && loc_CanSee(target1st, target2nd)) {
			VectorSubtract(target1st->s.origin, target2nd->s.origin, distance);
			current = VectorLength(distance);
			if (target1st->client->resp.team == target2nd->client->resp.team) {
// LAGA				if (target2nd->client->ps.powerups[PW_REDFLAG]
//				    || target2nd->client->ps.powerups[PW_BLUEFLAG]) {
//					best1 = target2nd;
//				}

// LAGA				if (!(best1 && (best1->client->ps.powerups[PW_REDFLAG]
//						|| best1->client->ps.powerups[PW_BLUEFLAG]))
				    if (closest1 < 0 || current < closest1) {
					best1 = target2nd;
					closest1 = current;
				}
			} else {
// LAGA				if (target2nd->client->ps.powerups[PW_REDFLAG]
//				    || target2nd->client->ps.powerups[PW_BLUEFLAG]) {
//					best2 = target2nd;
//				}

// LAGA				if (!(best2 && (best2->client->ps.powerups[PW_REDFLAG]
//						|| best2->client->ps.powerups[PW_BLUEFLAG]))
				    if (closest2 < 0 || current < closest2) {
					best2 = target2nd;
					closest2 = current;
				}
			}
		}
	}
	return ((best2) ? best2 : best1);
}
