//-----------------------------------------------------------------------------
//
// $Id: zcam.c,v 1.2 2003/02/10 02:12:25 ra Exp $
//
//-----------------------------------------------------------------------------
//
// $Log: zcam.c,v $
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

#include "g_local.h"
#include "zcam.h"

/* external function prototypes */
qboolean IsVisible(edict_t * player1, edict_t * player2, float maxrange);
int NumPlayersVisible(edict_t * viewer, float maxrange);
edict_t *PriorityTarget(edict_t * target, qboolean * override);
edict_t *PlayerToFollow(edict_t * ent, qboolean * override);
edict_t *PlayerToTrack(edict_t * ent, edict_t * target1st);

/* constants */
#define DAMP_ANGLE_Y			10
#define DAMP_VALUE_XY			6
#define DAMP_VALUE_Z			3
#define CAMERA_MIN_RANGE		48
#define CAMERA_MAX_RANGE		800
#define CAMERA_SWITCH_TIME		20000
#define CAMERA_DEAD_SWITCH_TIME		2000
#define CAMERA_MIN_SWITCH_TIME		4000
#define SWING_ANGLE_STEP		10.0F
#define SWING_DISTANCE_STEP		5.0F
#define SWING_VIEWANGLE_STEP		6.0F
#define SWING_NOMINAL_DISTANCE		80.0F
#define SWING_NOMINAL_HEIGHT		0.0F
#define SWING_FOV_FACTOR		1.25F
#define SWING_MSG_TIME                  1000

#ifndef min
#define min(a, b)	((a) < (b) ? (a) : (b))
#endif

#define acosf(x)        ((float)acos(x))
#define asinf(x)        ((float)asin(x))

void SetClientViewAngle(edict_t * ent, vec3_t angle)
{
	int i;

	// set the delta angle
	for (i = 0; i < 3; i++) {
		int cmdAngle;

		cmdAngle = ANGLE2SHORT(angle[i]);
		ent->client->ps.pmove.delta_angles[i] = cmdAngle - ent->client->resp.cmd_angles[i];
	}
	VectorCopy(angle, ent->s.angles);
	VectorCopy(ent->s.angles, ent->client->ps.viewangles);
}

/* local data */
static camera_t cameras[MAX_CLIENTS];

/* local functions */

static void CameraShowMode(edict_t * ent)
{
	if (ent->client->camera->mode == CAMERA_MODE_SWING)
		ent->client->camera->swing_msg_time = level.time + SWING_MSG_TIME;

	gi.centerprintf (ent, "Camera Mode-%s", (ent->client->camera->mode == CAMERA_MODE_FLIC) ? "FLIC" : "SWING");
}

static edict_t *ClosestVisible(edict_t * ent, float maxrange, qboolean pvs)
{
	int i;
	edict_t *target, *best = NULL;
	vec3_t distance;
	float current, closest = -1.0F;

	for (i = 1; i < game.maxclients; i++) {
		target = &g_edicts[i];
		if (target != ent && target->client->pers.connected &&
		    target->client->resp.team != NOTEAM && target->solid != SOLID_NOT &&
		    ((pvs) ? loc_CanSee(ent, target) : IsVisible(ent, target, maxrange))) {
			VectorSubtract(target->s.origin, ent->s.origin, distance);
			current = VectorLength(distance);
			if (closest < 0 || current < closest) {
				best = target;
				closest = current;
			}
		}
	}
	return best;
}

// count all players, excluding spectators
static int NumPlayers(void)
{
	int i, count = 0;
	edict_t *ent;

	for (i = 1; i < game.maxclients; i++) {
		ent = &g_edicts[i];
		if (ent->client->pers.connected && ent->client->resp.team != NOTEAM && ent->solid != SOLID_NOT) {
			count++;
		}
	}
	return count;
}

static void PointCamAtOrigin(edict_t * ent, vec3_t location)
{
	vec3_t diff, angles;

	VectorSubtract(location, ent->s.origin, diff);
	vectoangles(diff, angles);
	SetClientViewAngle(ent, angles);
}

static void PointCamAtTarget(edict_t * ent)
{
	vec3_t diff, angles;
	float difference;

	if (ent == NULL || ent->client->camera->flic_target == NULL ||
	    (ent->client->camera->flic_watching_the_wall && !IsVisible(ent, ent->client->camera->flic_target, 0)))
		return;

	VectorSubtract(ent->client->camera->flic_target->s.origin, ent->s.origin, diff);
	vectoangles(diff, angles);
	angles[2] = 0;
	difference = angles[1] - ent->s.angles[1];

	while (fabs(difference) > 180) {
		if (difference > 0) {
			difference -= 360;
		} else {
			difference += 360;
		}
	}

	if (fabs(difference) > ent->client->camera->flic_angle_lag) {
		// upto twice the angular velocity when |difference| > 20 deg
		if (difference > 0) {
			angles[1] += (difference < 20) ?
			    ent->client->camera->flic_angle_lag : ((1 + (difference - 20) / 160) *
								   ent->client->camera->flic_angle_lag);
		} else {
			angles[1] -= (difference > -20) ?
			    ent->client->camera->flic_angle_lag : ((1 + (-difference - 20) / 160) *
								   ent->client->camera->flic_angle_lag);
		}
	}

	SetClientViewAngle(ent, angles);
}

static qboolean InSolid(edict_t * ent)
{
	int contents;

	contents = gi.pointcontents(ent->s.origin);
	return ((contents & CONTENTS_SOLID) != 0);
}

static void FindCamPos(edict_t * ent, float angle, vec3_t offset_position, vec3_t cam_pos)
{
	vec3_t forward;

	AngleVectors(tv(ent->client->camera->flic_target->client->ps.viewangles[PITCH],
			ent->client->camera->flic_target->client->ps.viewangles[YAW] + angle,
			ent->client->camera->flic_target->client->ps.viewangles[ROLL]), forward, NULL, NULL);
	forward[2] = 0;

	VectorNormalize(forward);

	cam_pos[0] = ent->client->camera->flic_target->s.origin[0] + (offset_position[0] * forward[0]);
	cam_pos[1] = ent->client->camera->flic_target->s.origin[1] + (offset_position[1] * forward[1]);
	cam_pos[2] = ent->client->camera->flic_target->s.origin[2] + offset_position[2];
}

static void RepositionAtTarget(edict_t * ent, vec3_t offset_position)
{
	vec3_t diff, cam_pos;
	trace_t trace;
	qboolean snapto = false;	// snapto towards target when jumping to new position

	ent->client->camera->flic_watching_the_wall = false;

	// try to be behind target, but if too close
	// try to be on his right/left/front
	FindCamPos(ent, 0, offset_position, cam_pos);
	PRETRACE ();
	trace = gi.trace (ent->client->camera->flic_target->s.origin, NULL, NULL, cam_pos,
			ent->client->camera->flic_target, CONTENTS_SOLID);
	POSTTRACE ();

	if (trace.fraction < 1) {
		VectorSubtract(trace.endpos, ent->client->camera->flic_target->s.origin, diff);
		if (VectorLength(diff) < 40) {
			FindCamPos(ent, 90, offset_position, cam_pos);
			PRETRACE ();
			trace = gi.trace (ent->client->camera->flic_target->s.origin, NULL, NULL, cam_pos,
					ent->client->camera->flic_target, CONTENTS_SOLID);
			POSTTRACE ();
			if (trace.fraction < 1) {
				VectorSubtract(trace.endpos, ent->client->camera->flic_target->s.origin, diff);
				if (VectorLength(diff) < 40) {
					FindCamPos(ent, -90, offset_position, cam_pos);
					PRETRACE ();
					trace = gi.trace (ent->client->camera->flic_target->s.origin, NULL,
							NULL, cam_pos, ent->client->camera->flic_target, CONTENTS_SOLID);
					POSTTRACE ();
					if (trace.fraction < 1) {
						VectorSubtract(trace.endpos,
							       ent->client->camera->flic_target->s.origin,
							       diff);
						if (VectorLength(diff) < 40) {
							FindCamPos(ent, 180, offset_position, cam_pos);
							PRETRACE ();
							trace = gi.trace (ent->client->camera->flic_target->s.origin,
									NULL, NULL, cam_pos,
									ent->client->camera->flic_target, CONTENTS_SOLID);
							POSTTRACE ();
						}
					}
				}
			}
		}
		VectorNormalize(diff);
		VectorMA(trace.endpos, -8, diff, trace.endpos);
		if (trace.plane.normal[2] > 0.8)
			trace.endpos[2] += 4;
		snapto = true;
	}

	if (fabs(trace.endpos[0] - ent->s.origin[0]) > ent->client->camera->flic_xy_lag)
		if (trace.endpos[0] > ent->s.origin[0])
			ent->s.origin[0] += ent->client->camera->flic_xy_lag;
		else
			ent->s.origin[0] -= ent->client->camera->flic_xy_lag;
	else
		ent->s.origin[0] = trace.endpos[0];

	if (fabs(trace.endpos[1] - ent->s.origin[1]) > ent->client->camera->flic_xy_lag)
		if (trace.endpos[1] > ent->s.origin[1])
			ent->s.origin[1] += ent->client->camera->flic_xy_lag;
		else
			ent->s.origin[1] -= ent->client->camera->flic_xy_lag;
	else
		ent->s.origin[1] = trace.endpos[1];

	if (fabs(trace.endpos[2] - ent->s.origin[2]) > ent->client->camera->flic_z_lag)
		if (trace.endpos[2] > ent->s.origin[2])
			ent->s.origin[2] += ent->client->camera->flic_z_lag;
		else
			ent->s.origin[2] -= ent->client->camera->flic_z_lag;
	else
		ent->s.origin[2] = trace.endpos[2];

	PRETRACE ();
	trace = gi.trace (ent->client->camera->flic_target->s.origin, NULL, NULL, ent->s.origin,
			ent->client->camera->flic_target, CONTENTS_SOLID);
	POSTTRACE ();
	if (trace.fraction < 1) {
		VectorSubtract(trace.endpos, ent->client->camera->flic_target->s.origin, diff);
		VectorNormalize(diff);
		VectorMA(trace.endpos, -8, diff, trace.endpos);

		if (trace.plane.normal[2] > 0.8)
			trace.endpos[2] += 4;

		VectorCopy(trace.endpos, ent->s.origin);

		snapto = true;
	}

	if (snapto) {
		vec3_t angles;

		VectorSubtract(ent->client->camera->flic_target->s.origin, ent->s.origin, diff);
		vectoangles(diff, angles);
		SetClientViewAngle(ent, angles);
	}

}

static void RepositionAtOrigin(edict_t * ent, vec3_t offset_position)
{
	vec3_t cam_pos;
	trace_t trace;

	cam_pos[0] = offset_position[0] + 40;
	cam_pos[1] = offset_position[1] + 40;
	cam_pos[2] = offset_position[2] + 30;

	PRETRACE ();
	trace = gi.trace (offset_position, NULL, NULL, cam_pos, ent->client->camera->flic_target, CONTENTS_SOLID);
	POSTTRACE ();
	if (trace.fraction < 1) {
		vec3_t diff;

		VectorSubtract(trace.endpos, offset_position, diff);
		VectorNormalize(diff);
		VectorMA(trace.endpos, -8, diff, trace.endpos);

		if (trace.plane.normal[2] > 0.8)
			trace.endpos[2] += 4;
	}

	if (fabs(trace.endpos[0] - ent->s.origin[0]) > ent->client->camera->flic_xy_lag) {
		if (trace.endpos[0] > ent->s.origin[0]) {
			ent->s.origin[0] += ent->client->camera->flic_xy_lag;
		} else {
			ent->s.origin[0] -= ent->client->camera->flic_xy_lag;
		}
	} else {
		ent->s.origin[0] = trace.endpos[0];
	}

	if (fabs(trace.endpos[1] - ent->s.origin[1]) > ent->client->camera->flic_xy_lag) {
		if (trace.endpos[1] > ent->s.origin[1]) {
			ent->s.origin[1] += ent->client->camera->flic_xy_lag;
		} else {
			ent->s.origin[1] -= ent->client->camera->flic_xy_lag;
		}
	} else {
		ent->s.origin[1] = trace.endpos[1];
	}

	if (fabs(trace.endpos[2] - ent->s.origin[2]) > ent->client->camera->flic_z_lag) {
		if (trace.endpos[2] > ent->s.origin[2]) {
			ent->s.origin[2] += ent->client->camera->flic_z_lag;
		} else {
			ent->s.origin[2] -= ent->client->camera->flic_z_lag;
		}
	} else {
		ent->s.origin[2] = trace.endpos[2];
	}

	PRETRACE ();
	trace = gi.trace (offset_position, NULL, NULL, ent->s.origin, ent->client->camera->flic_target, CONTENTS_SOLID);
	POSTTRACE ();

	if (trace.fraction < 1) {
		vec3_t diff;

		VectorSubtract(trace.endpos, offset_position, diff);
		VectorNormalize(diff);
		VectorMA(trace.endpos, -8, diff, trace.endpos);

		if (trace.plane.normal[2] > 0.8)
			trace.endpos[2] += 4;

		VectorCopy(trace.endpos, ent->s.origin);
	}

}

static void SwitchToNewTarget(edict_t * ent, edict_t * new_target)
{
	if (ent->client->camera->flic_target == NULL) {
		ent->client->camera->flic_target = new_target;
		ent->client->camera->flic_last_switch_time = level.time + CAMERA_MIN_SWITCH_TIME;
	} else if (ent->client->camera->flic_target != new_target) {
		if (ent->client->camera->flic_last_switch_time < level.time) {
			ent->client->camera->flic_target = new_target;
			ent->client->camera->flic_last_switch_time = level.time + CAMERA_MIN_SWITCH_TIME;
		}
	}
	if (ent->client->camera->flic_target == NULL)
		ent->client->camera->flic_last_switch_time = 0;
}

static void CameraFlicThink(edict_t * ent)
{
	vec3_t camera_offset;
	int num_visible;
	edict_t *new_target;

	// move towards target if inside solid 
	if (ent->client->camera->flic_target && !ent->client->camera->flic_watching_the_dead && InSolid(ent)) {
		RepositionAtTarget(ent, tv(-60, -60, 40));
	}

	num_visible = NumPlayersVisible(ent, MAX_VISIBLE_RANGE);
	new_target = PlayerToFollow(ent, &ent->client->camera->flic_override);

	// only watch the dead if it's the one we followed
	if (!ent->client->camera->flic_watching_the_dead
	    && ent->client->camera->flic_target && ent->client->camera->flic_target->solid == SOLID_NOT) {
		ent->client->camera->flic_watching_the_dead = true;
		ent->client->camera->flic_last_move_time = level.time + CAMERA_DEAD_SWITCH_TIME;
		PointCamAtTarget(ent);
	} else if (ent->client->camera->flic_watching_the_dead) {
		if (ent->client->camera->flic_last_move_time < level.time || InSolid(ent)) {
			ent->client->camera->flic_watching_the_dead = false;
		} else {
			if (ent->client->camera->flic_target->solid == SOLID_NOT) {
				VectorCopy(ent->client->camera->flic_target->s.origin,
					   ent->client->camera->flic_dead_origin);
			}
			PointCamAtOrigin(ent, ent->client->camera->flic_dead_origin);
			RepositionAtOrigin(ent, ent->client->camera->flic_dead_origin);
		}
	} else if (num_visible < 2) {
		camera_offset[0] = -60;
		camera_offset[1] = -60;
		camera_offset[2] = 40;

		if (ent->client->camera->flic_last_move_time >= level.time) {
			edict_t *closest_target;

			if (new_target != NULL
			    && (ent->client->camera->flic_override
				|| NumPlayersVisible(new_target, MAX_VISIBLE_RANGE) > 1)) {
				SwitchToNewTarget(ent, new_target);
				RepositionAtTarget(ent, camera_offset);
				PointCamAtTarget(ent);
			} else if ((closest_target = ClosestVisible(ent, MAX_VISIBLE_RANGE, false)) != NULL) {
				SwitchToNewTarget(ent, closest_target);
				RepositionAtTarget(ent, camera_offset);
				PointCamAtTarget(ent);
			} else if (new_target != NULL) {
				// look for someone new!
				SwitchToNewTarget(ent, new_target);
				RepositionAtTarget(ent, camera_offset);
				PointCamAtTarget(ent);
				ent->client->camera->flic_last_move_time = 0;
			}
		} else if (new_target != NULL) {
			// just keep looking for action!
			camera_offset[0] = -60;
			camera_offset[1] = -60;
			camera_offset[2] = 40;
			SwitchToNewTarget(ent, new_target);
			RepositionAtTarget(ent, camera_offset);
			PointCamAtTarget(ent);
		}
	}
	// if we are done during a battle.
	else if (ent->client->camera->flic_last_move_time < level.time
		 || (ent->client->camera->flic_target 
		     && !loc_CanSee(ent, ent->client->camera->flic_target))
		 || (ent->client->camera->flic_target && InSolid(ent))) {
		if (new_target != NULL) {
			camera_offset[0] = -60;
			camera_offset[1] = -60;
			camera_offset[2] = 80;
			ent->client->camera->flic_target = NULL;
			SwitchToNewTarget(ent, new_target);
			RepositionAtTarget(ent, camera_offset);
			PointCamAtTarget(ent);
			ent->client->camera->flic_last_move_time = level.time + CAMERA_SWITCH_TIME;
		}
	} else if (ent->client->camera->flic_target != NULL) {
		if (IsVisible(ent, ent->client->camera->flic_target, 0)) {
			float distance;
			vec3_t diff;

			VectorSubtract(ent->s.origin, ent->client->camera->flic_target->s.origin,
				       diff);
			distance = VectorLength(diff);
			if (distance < CAMERA_MIN_RANGE || distance > CAMERA_MAX_RANGE)
				RepositionAtTarget(ent, tv(-60, -60, 80));
			PointCamAtTarget(ent);
		} else
			ent->client->camera->flic_last_move_time = 0;
	}
}

void CameraFlicBegin(edict_t * ent)
{
	int clientNum = ent - g_edicts;

	cameras[clientNum].mode = CAMERA_MODE_FLIC;
	cameras[clientNum].flic_target = NULL;
	cameras[clientNum].swing_target = NULL;
	camera_begin(ent);
	CameraShowMode(ent);
}

static void CameraStaticThink(edict_t * ent)
{
	trace_t trace;
	vec3_t end_floor, end_ceiling;
	static vec3_t mins = { -4, -4, -4 };
	static vec3_t maxs = { 4, 4, 4 };

	end_floor[0] = ent->s.origin[0];
	end_floor[1] = ent->s.origin[1];
	end_floor[2] = ent->s.origin[2] - 40000;

	PRETRACE ();
	trace = gi.trace (ent->s.origin, mins, maxs, end_floor, ent, CONTENTS_SOLID);
	POSTTRACE ();

	VectorCopy(trace.endpos, end_floor);

	end_ceiling[0] = end_floor[0];
	end_ceiling[1] = end_floor[1];
	end_ceiling[2] = end_floor[2] + 175;

	PRETRACE ();
	trace = gi.trace (end_floor, mins, maxs, end_ceiling, ent, CONTENTS_SOLID);
	POSTTRACE ();
	VectorCopy(trace.endpos, ent->s.origin);

	if (ent->client->camera->flic_last_move_time < level.time) {
		vec3_t angles;

		ent->client->camera->flic_last_move_time = level.time + 2000;
		angles[0] = 45;
		angles[1] = 0;
		angles[2] = 0;
		SetClientViewAngle(ent, angles);
	}
}

/* limit angle to [-180, 180] */
static float AngleMod180(float angle)
{
	while (fabs(angle) > 180)
		if (angle > 0)
			angle -= 360;
		else
			angle += 360;
	return angle;
}

/* subtract angle b from a to give minimum difference
   assume a and b are in [-180, 180] */
static float AngleDiff(float a, float b)
{
	float c, c1, c2, c3;

	c1 = a - b;
	c2 = a - (b + 360);
	c3 = a - (b - 360);
	c = c1;
	if (fabs(c2) < fabs(c))
		c = c2;
	if (fabs(c3) < fabs(c))
		c = c3;
	return c;
}

static float AngleClamp(float angle, float limit)
{
	if (angle > limit)
		return limit;
	if (angle < -limit)
		return -limit;
	return angle;
}

static edict_t *CameraSwingTarget(edict_t * ent)
{
	edict_t *target1st, *target2nd;

	target1st = ent->client->camera->swing_target;
	target2nd = (target1st != NULL) ? PlayerToTrack(ent, target1st) : NULL;

	if (target2nd != ent->client->camera->swing_secondry_target &&
	    target2nd != NULL && ent->client->camera->swing_msg_time <= level.time) {
		ent->client->camera->swing_msg_time = 0;
		gi.centerprintf (ent, "Following %s - Tracking %s", target1st->client->pers.netname, target2nd->client->pers.netname);
	}

	return target2nd;
}

static void CameraSwingAngle(edict_t * ent)
{
	edict_t *target, *player;
	vec3_t player2target, vDiff;
	float best_angle, chase_diff, target_distance, target_yaw;
	float gamma, best_angle1, best_angle2, chase_diff1, chase_diff2;

	target = ent->client->camera->swing_secondry_target;
	player = ent->client->camera->swing_target;

	// no 2nd target
	if (target == NULL)	// return to swing_angle == 0
	{
		if ((fabs(ent->client->camera->swing_angle) < SWING_ANGLE_STEP))
			ent->client->camera->swing_angle = 0;
		else if (ent->client->camera->swing_angle > 0)
			ent->client->camera->swing_angle =
			    AngleMod180(ent->client->camera->swing_angle - SWING_ANGLE_STEP);
		else
			ent->client->camera->swing_angle =
			    AngleMod180(ent->client->camera->swing_angle + SWING_ANGLE_STEP);
		return;
	}

	VectorSubtract(target->s.origin, player->s.origin, vDiff);
	target_distance = VectorLength(vDiff);
	vectoangles(vDiff, player2target);
	target_yaw = AngleMod180(player2target[YAW] - ent->client->camera->swing_last_viewangles[YAW]);
	if (target_distance > ent->client->camera->swing_distance)
		gamma =
		    SWING_FOV_FACTOR * (acosf(ent->client->camera->swing_distance / target_distance)) * 180.0F / M_PI;
	else
		gamma =
		    SWING_FOV_FACTOR * (90.0F -
					((asinf(0.5 * target_distance / ent->client->camera->swing_distance)) * 180.0F /
					 M_PI));
	best_angle1 = AngleMod180(target_yaw - (180 - gamma));
	best_angle2 = AngleMod180(target_yaw + (180 - gamma));
	chase_diff1 = AngleDiff(ent->client->camera->swing_angle, best_angle1);
	chase_diff2 = AngleDiff(ent->client->camera->swing_angle, best_angle2);

	if (fabs(chase_diff1) < fabs(chase_diff2)) {
		chase_diff = chase_diff1;
		best_angle = best_angle1;
	} else {
		chase_diff = chase_diff2;
		best_angle = best_angle2;
	}

	// chase_diff is used to determine the direction to move to
	// in order to decrease/increase yaw separation
	if (fabs(chase_diff) < SWING_ANGLE_STEP)
		ent->client->camera->swing_angle = best_angle;
	else if (chase_diff > 0)
		ent->client->camera->swing_angle = AngleMod180(ent->client->camera->swing_angle - SWING_ANGLE_STEP);
	else
		ent->client->camera->swing_angle = AngleMod180(ent->client->camera->swing_angle + SWING_ANGLE_STEP);
	return;
}

static void CameraSwingViewangles(edict_t * ent)
{
	float desired_yaw, desired_pitch, yaw_diff, pitch_diff;

	// determine desired yaw and pitch
	if (ent->client->camera->swing_secondry_target) {
		vec3_t vDiff, target_angles;

		VectorSubtract(ent->client->camera->swing_secondry_target->s.origin, ent->s.origin,
			       vDiff);
		vectoangles(vDiff, target_angles);
		desired_yaw = AngleMod180(target_angles[YAW] -
					  (ent->client->camera->swing_angle + ent->client->ps.viewangles[YAW])) / 2;
		desired_yaw = AngleClamp(desired_yaw, 45);
		desired_pitch = AngleMod180(target_angles[PITCH] - ent->client->ps.viewangles[PITCH]) / 2;
		desired_pitch = AngleClamp(desired_pitch, 45);
	} else {
		desired_yaw = 0;
		desired_pitch = 0;
	}

	// move yaw
	yaw_diff = AngleMod180(ent->client->camera->swing_yaw - desired_yaw);
	if (fabs(yaw_diff) < SWING_VIEWANGLE_STEP)
		ent->client->camera->swing_yaw = desired_yaw;
	else if (yaw_diff > 0)
		ent->client->camera->swing_yaw = AngleMod180(ent->client->camera->swing_yaw - SWING_VIEWANGLE_STEP);
	else
		ent->client->camera->swing_yaw = AngleMod180(ent->client->camera->swing_yaw + SWING_VIEWANGLE_STEP);

	// move pitch
	pitch_diff = AngleMod180(ent->client->camera->swing_pitch - desired_pitch);
	if (fabs(pitch_diff) < SWING_VIEWANGLE_STEP)
		ent->client->camera->swing_pitch = desired_pitch;
	else if (pitch_diff > 0)
		ent->client->camera->swing_pitch = AngleMod180(ent->client->camera->swing_pitch - SWING_VIEWANGLE_STEP);
	else
		ent->client->camera->swing_pitch = AngleMod180(ent->client->camera->swing_pitch + SWING_VIEWANGLE_STEP);
}

void CameraSwingCycle(edict_t * ent, int dir)
{
	int clientnum = 0;
	int original = 0;
	edict_t *other;

	if (dir != 1 && dir != -1)
		dir = 1;

	if (ent->client->camera->mode != CAMERA_MODE_SWING) {
		ent->client->camera->mode = CAMERA_MODE_SWING;
		CameraShowMode(ent);

		// start with current flic target
		if (ent->client->camera->flic_target != NULL) {
			if (ent->client->camera->flic_target->client->pers.connected &&
				ent->client->camera->flic_target->client->resp.team != NOTEAM &&
				ent->client->camera->flic_target->solid != SOLID_NOT) {
		        		ent->client->camera->swing_target = ent->client->camera->flic_target;
					camera_begin(ent);
					return;
			}
		}
		ent->client->camera->swing_target = NULL;
	}

	if (ent->client->camera->swing_target != NULL) {
		clientnum = ent->client->camera->swing_target->s.number;
		original = clientnum;
	}

	do {
		clientnum += dir;
		if (clientnum >= game.maxclients)
			clientnum = 0;
		if (clientnum < 0)
			clientnum = game.maxclients - 1;
		other = &g_edicts[clientnum];
		if (!other->inuse || !other->client)
			continue;
		// can only follow connected clients
		// can't follow another spectator
		if (!other->client->pers.connected || other->client->resp.team == NOTEAM ||
			other->solid == SOLID_NOT)
			continue;

		// this is good, we can use it
		ent->client->camera->swing_target = &g_edicts[clientnum];
		camera_begin(ent);
		return;
	} while (clientnum != original);

	ent->client->camera->swing_target = NULL;
}

static void CameraSwingThink(edict_t * ent)
{
	edict_t *target;
	vec3_t o, ownerv, goal, vDiff, forward;
	vec3_t right, oldgoal, angles, viewangles;
	trace_t trace;
	static vec3_t mins = { -4, -4, -4 };
	static vec3_t maxs = { 4, 4, 4 };

	// validate current target
	if (ent->client->camera->swing_target == NULL) {
		if (ent->client->camera->swing_target != NULL &&
			(!ent->client->camera->swing_target->client->pers.connected ||
			ent->client->camera->swing_target->client->resp.team == NOTEAM ||
			ent->client->camera->swing_target->solid == SOLID_NOT)) {
			// target is not valid: try the next client
			CameraSwingCycle(ent, 1);
			if (ent->client->camera->swing_target == NULL)
				return;
		}
		CameraSwingCycle(ent, 1);
		if (ent->client->camera->swing_target == NULL)
			return;
	}
	// we have a valid target: find secondry target
	target = ent->client->camera->swing_target;
	ent->client->camera->swing_secondry_target = CameraSwingTarget(ent);

	// update viewangles as long as target is alive
	if (target->solid != SOLID_NOT) {
		VectorCopy(target->client->ps.viewangles, ent->client->camera->swing_last_viewangles);
	}
	CameraSwingAngle(ent);
	ent->client->camera->swing_height = SWING_NOMINAL_HEIGHT;

	VectorCopy(target->s.origin, ownerv);
	VectorCopy(ent->s.origin, oldgoal);
	VectorCopy(ent->client->camera->swing_last_viewangles, angles);

	angles[YAW] += ent->client->camera->swing_angle;
	angles[PITCH] = 0;

	AngleVectors(angles, forward, right, NULL);
	VectorNormalize(forward);
	VectorMA(ownerv, -ent->client->camera->swing_distance, forward, o);

	if (o[2] < target->s.origin[2] + 20)
		o[2] = target->s.origin[2] + 20;

	PRETRACE ();
	trace = gi.trace (ownerv, mins, maxs, o, ent->client->camera->swing_target, CONTENTS_SOLID);
	POSTTRACE ();

	if (trace.fraction != 1.0) {
		VectorCopy(trace.endpos, o);
		o[2] += (1.0 - trace.fraction) * 32;
		// try another trace to this position, because a tunnel may have the ceiling
		// close enough that this is poking out
		PRETRACE ();
		trace = gi.trace (ownerv, mins, maxs, o, ent->client->camera->swing_target, CONTENTS_SOLID);
		POSTTRACE ();
		VectorCopy(trace.endpos, o);

		VectorSubtract(ent->client->camera->swing_target->s.origin, ent->s.origin, vDiff);
		ent->client->camera->swing_distance = min(VectorLength(vDiff), SWING_NOMINAL_DISTANCE);
	} else {
		float distance = ent->client->camera->swing_distance - SWING_NOMINAL_DISTANCE;

		if (fabs(distance) < SWING_DISTANCE_STEP)
			ent->client->camera->swing_distance = SWING_NOMINAL_DISTANCE;
		else if (distance > 0)
			ent->client->camera->swing_distance -= SWING_DISTANCE_STEP;
		else
			ent->client->camera->swing_distance += SWING_DISTANCE_STEP;
	}

	VectorCopy(o, goal);

	// set camera angles
	VectorCopy(ent->client->camera->swing_last_viewangles, viewangles);
	if (ent->client->camera->swing_secondry_target != NULL)
		viewangles[PITCH] = 0;
	SetClientViewAngle(ent, viewangles);
	CameraSwingViewangles(ent);
	viewangles[PITCH] = AngleClamp(ent->client->camera->swing_pitch, 85);
	viewangles[YAW] += ent->client->camera->swing_yaw + ent->client->camera->swing_angle;

	SetClientViewAngle(ent, viewangles);

	// set camera position
	VectorCopy(goal, ent->s.origin);
}

/* camera API */

void camera_init(void)
{
	int i;

	for (i = 1; i < game.maxclients; i++) {
		cameras[i].mode = CAMERA_MODE_FLIC;
		cameras[i].flic_target = NULL;
		cameras[i].swing_target = NULL;
		cameras[i].swing_msg_time = 0;
		camera_begin(&g_edicts[i]);
	}
}

void camera_shutdown(void)
{
}

void camera_begin(edict_t * ent)
{
	int i;

	if (!ent->inuse || !ent->client)
		return;

	i = ent->s.number;

	ent->client->camera = &cameras[i];
	ent->client->camera->flic_watching_the_dead = false;
	ent->client->camera->flic_watching_the_wall = false;
	ent->client->camera->flic_xy_lag = DAMP_VALUE_XY;
	ent->client->camera->flic_z_lag = DAMP_VALUE_Z;
	ent->client->camera->flic_angle_lag = DAMP_ANGLE_Y;
	ent->client->camera->flic_last_move_time = 0;
	ent->client->camera->flic_last_switch_time = 0;

	ent->client->camera->swing_secondry_target = NULL;
	ent->client->camera->swing_distance = SWING_NOMINAL_DISTANCE;
	ent->client->camera->swing_height = 0.0F;
	ent->client->camera->swing_angle = 0.0F;
	ent->client->camera->swing_yaw = 0.0F;
	ent->client->camera->swing_pitch = 0.0F;
	VectorClear(ent->client->camera->swing_last_viewangles);
}

void camera_disconnect(edict_t * ent)
{
	edict_t *other;
	int i;

	// reset camera state for disconnected clients
	CameraFlicBegin(ent);

	// force rethink on all cameras
	for (i = 1; i < game.maxclients; i++) {
		other = &g_edicts[i];
		if (!other->client->camera)
			continue;
		if (other->client->pers.connected && other->solid == SOLID_NOT) {
			if (other->client->camera->mode == CAMERA_MODE_FLIC) {
				other->client->camera->flic_last_move_time = level.time;
				if (other->client->camera->flic_target == ent) {
					other->client->camera->flic_target = NULL;
					other->client->camera->swing_target = NULL;
				}
			} else if (other->client->camera->mode == CAMERA_MODE_SWING) {
				if (other->client->camera->swing_target == ent) {
					CameraSwingCycle(other, 1);
				} else if (other->client->camera->swing_secondry_target == ent) {
					other->client->camera->swing_secondry_target = NULL;
				}
			}
		}
	}
}

void camera_think(edict_t * ent)
{
	if (NumPlayers() == 0) {
		CameraStaticThink(ent);
		return;
	}

	switch (ent->client->camera->mode) {
	case CAMERA_MODE_FLIC:
		CameraFlicThink(ent);
		break;

	case CAMERA_MODE_SWING:
		CameraSwingThink(ent);
		break;

	default:
		break;
	}
}

void camera_cmd(edict_t * ent)
{
	if (gi.argc () == 2) {
		char arg[MAX_STRING_CHARS];

		strcpy (arg, gi.argv (1));
		if (ent->client->resp.team == NOTEAM) {
			if (Q_stricmp(arg, "prev") == 0)
				CameraSwingCycle(ent, -1);
			else if (Q_stricmp(arg, "next") == 0)
				CameraSwingCycle(ent, 1);
			else if (Q_stricmp(arg, "flic") == 0) {
				if (ent->client->camera->mode != CAMERA_MODE_FLIC)
					CameraFlicBegin(ent);
			} else if (Q_stricmp(arg, "swing") == 0) {
				if (ent->client->camera->mode != CAMERA_MODE_SWING)
					CameraSwingCycle(ent, 1);
			}
		} else {
			if (Q_stricmp(arg, "flic") == 0)
				ent->client->camera->mode = CAMERA_MODE_FLIC;
			else if (Q_stricmp(arg, "swing") == 0)
				ent->client->camera->mode = CAMERA_MODE_SWING;
		}
	}
}
