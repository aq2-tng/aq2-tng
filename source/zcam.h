//-----------------------------------------------------------------------------
//
// $Id: zcam.h,v 1.1 2002/09/04 11:23:10 ra Exp $
//
//-----------------------------------------------------------------------------
//
// $Log: zcam.h,v $
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

#ifndef __ZCAM_H__
#define __ZCAM_H__

#define MAX_VISIBLE_RANGE		1000

void camera_init (void);
void camera_shutdown (void);
void camera_state_save (gclient_t *client);
void camera_state_load (gclient_t *client);
void camera_begin (edict_t *ent);
void camera_disconnect (edict_t *ent);
void camera_think (edict_t *ent);
void camera_cmd (edict_t *ent);
void CameraSwingCycle (edict_t *ent, int dir);
void CameraFlicBegin(edict_t * ent);

/* camera mode */
typedef enum camera_mode_e {
	CAMERA_MODE_FLIC,
	CAMERA_MODE_SWING
} camera_mode_t;

/* camera data */
typedef struct camera_s {
	camera_mode_t mode;	// camera mode

	// swing mode
	float swing_distance;	// distance behind player
	float swing_height;	// height of camera
	float swing_angle;	// angle of camera
	float swing_yaw;	// yaw of camera
	float swing_pitch;	// pitch of camera
	edict_t *swing_target;
	edict_t *swing_secondry_target;
	vec3_t swing_last_viewangles;
	float swing_msg_time;

	// flic mode
	edict_t *flic_target;
	qboolean flic_watching_the_dead;
	qboolean flic_watching_the_wall;
	vec3_t flic_dead_origin;
	float flic_xy_lag;
	float flic_z_lag;
	float flic_angle_lag;
	float flic_last_move_time;
	float flic_last_switch_time;
	qboolean flic_override;	// signal that player must be followed
} camera_t;
#endif /* __ZCAM_H__ */

