//-----------------------------------------------------------------------------
//
// $Id: zcam_target.c,v 1.2 2003/02/10 02:12:25 ra Exp $
//
//-----------------------------------------------------------------------------
//
// $Log: zcam_target.c,v $
// Revision 1.2  2003/02/10 02:12:25  ra
// Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
//
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
 *  Ported to ActionQuake by JBravo.
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
	edict_t *ent;

	if (viewer == NULL) {
		return 0;
	}

	for (i = 1; i < game.maxclients; i++) {
		ent = &g_edicts[i];
		if (viewer->s.number != i && ent->client->pers.connected &&
			ent->client->resp.team != NOTEAM && ent->solid != SOLID_NOT) {
				if (IsVisible(viewer, ent, maxrange)) {
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
	edict_t *ent;

	for (i = 1; i < game.maxclients; i++) {
		ent = &g_edicts[i];
		if (ent->client->pers.connected && ent->client->resp.team != NOTEAM && ent->solid != SOLID_NOT && HasFlag(ent)) {
			if (favorite == NULL ||
			    NumPlayersVisible(favorite, MAX_VISIBLE_RANGE) < NumPlayersVisible(ent, MAX_VISIBLE_RANGE)) {
				favorite = ent;
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

	for (i = 1; i < game.maxclients; i++) {
		viewer = &g_edicts[i];
		if (!viewer->inuse || !viewer->client)
			continue;
		if (!viewer->client->pers.connected)
			continue;
		if (viewer->client->resp.team != NOTEAM && viewer->solid != SOLID_NOT) {
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

	for (i = 1; i < game.maxclients; i++) {
		target2nd = &g_edicts[i];
		if (!target2nd->inuse || !target2nd->client)
			continue;
		if (target1st != target2nd && target2nd->client->pers.connected &&
		    target2nd->client->resp.team != NOTEAM && target2nd->solid != SOLID_NOT &&
		    loc_CanSee(target1st, target2nd)) {
			VectorSubtract(target1st->s.origin, target2nd->s.origin, distance);
			current = VectorLength(distance);
			if (target1st->client->resp.team == target2nd->client->resp.team) {
				if (HasFlag(target2nd))
					best1 = target2nd;
				if (!(best1 && HasFlag(best1))) {
					if (closest1 < 0 || current < closest1) {
						best1 = target2nd;
						closest1 = current;
					}
				}
			} else {
				if (HasFlag(target2nd)) {
					best2 = target2nd;
				}
				if (!(best2 && HasFlag(best2))) {
					if (closest2 < 0 || current < closest2) {
						best2 = target2nd;
						closest2 = current;
					}
				}
			}
		}
	}
	return ((best2) ? best2 : best1);
}
