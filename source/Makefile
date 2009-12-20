
# Action makefile 
#
# $Id: Makefile,v 1.21 2003/06/15 21:43:53 igor Exp $
#
# Intended for gcc/Linux, may need modifying for other platforms
# Adapted by PG BUND from Black Cross makefile
# (not sure if it will do it :)
# corrected and expanded by Igor[Rock]
# (needs still some testing/modification for the dist-targets)
#-----------------------------------------------------------------------------
# $Log: Makefile,v $
# Revision 1.21  2003/06/15 21:43:53  igor
# added IRC client
#
# Revision 1.20  2003/06/15 15:34:32  igor
# - removed the zcam code from this branch (see other branch)
# - added fixes from 2.72 (source only) version
# - resetted version number to 2.72
# - This version should be exactly like the release 2.72 - just with a few
#   more fixes (which whoever did the source only variant didn't get because
#   he didn't use the CVS as he should. Shame on him.
#
# Revision 1.19  2003/02/10 02:12:25  ra
# Zcam fixes, kick crashbug in CTF fixed and some code cleanup.
#
# Revision 1.18  2002/09/04 11:23:09  ra
# Added zcam to TNG and bumped version to 3.0
#
# Revision 1.17  2002/04/03 08:59:17  freud
# Added DEBUG option in Makefile for debugging symbols and compiler warnings
#
# Revision 1.16  2001/08/18 17:16:58  ra
# Fixed makefile to know about tng_flashlight
#
# Revision 1.15  2001/08/17 22:05:17  ra
# Added new tng_stats files to the makefile and coverted them out of DOS format
#
# Revision 1.14  2001/08/08 12:42:22  slicerdw
# Ctf Should finnaly be fixed now, lets hope so
#
# Revision 1.13  2001/06/26 18:47:30  igor_rock
# added ctf_respawn cvar
#
# Revision 1.12  2001/06/25 11:59:20  slicerdw
# Small Fix
#
# Revision 1.11  2001/06/19 21:10:05  igor_rock
# changed the "is now known" message to the normal namelimit
#
# Revision 1.10  2001/06/16 16:47:05  deathwatch
# Matchmode Fixed
#
# Revision 1.9  2001/06/13 08:39:13  igor_rock
# changed "cvote" to "use_cvote" (like the other votecvars)
#
# Revision 1.8  2001/06/13 08:00:25  igor_rock
# corrected stupid error (wrong changes committed)
#
# Revision 1.7  2001/06/13 07:55:17  igor_rock
# Re-Added a_match.h and a_match.c
# Added CTF Header for a_ctf.h and a_ctf.c
#
# Revision 1.6  2001/05/31 16:58:14  igor_rock
# conflicts resolved
#
# Revision 1.5.2.1  2001/05/20 18:54:19  igor_rock
# added original ctf code snippets from zoid. lib compilesand runs but
# doesn't function the right way.
# Jsut committing these to have a base to return to if something wents
# awfully wrong.
#
# Revision 1.5  2001/05/15 15:49:14  igor_rock
# added itm_flags for deathmatch
#
# Revision 1.4  2001/05/11 20:15:51  ra
#
#
# Make make clean also remove the .so or .dll file.
#
# Revision 1.3  2001/05/08 20:23:54  igor_rock
# added the depend information (will change more often...)
#
# Revision 1.2  2001/05/08 12:55:02  igor_rock
# - corrected location for make install
# - added a_ctf.o
#
# Revision 1.1.1.1  2001/05/06 17:23:57  igor_rock
# This is the PG Bund Edition V1.25 with all stuff laying around here...
#
#-----------------------------------------------------------------------------

#standard package is "as" (short for Action Source?), "PG" is "PG BUND"

PACKAGE="as-PG1.25"

VERSION="1.52"

# Uncomment this for debugging symbols and compiler warnings
#DEBUG=1

ARCH=i386
CC=gcc
BASE_CFLAGS=-DC_ONLY

#use these cflags to optimize it
ifdef DEBUG
	CFLAGS=$(BASE_CFLAGS) -g -O2 -Wall
else
	CFLAGS=$(BASE_CFLAGS) -march=i586 -mcpu=i586 -O6 -ffast-math -funroll-loops \
		-fomit-frame-pointer -fexpensive-optimizations -falign-loops=2 \
		-falign-jumps=2 -falign-functions=2
endif

LDFLAGS=-ldl -lm
SHLIBEXT=so
SHLIBCFLAGS=-fPIC
SHLIBLDFLAGS=-shared

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<

#############################################################################
# SETUP AND BUILD
# GAME
#############################################################################

.c.o:
	$(DO_CC)

GAME_OBJS = \
	a_cmds.o a_ctf.o a_doorkick.o a_game.o a_items.o a_match.o \
	a_menu.o a_radio.o a_team.o a_tourney.o a_vote.o a_xcmds.o a_xgame.o \
	a_xmenu.o a_xvote.o cgf_sfx_glass.o g_ai.o g_chase.o g_cmds.o \
	g_combat.o g_func.o g_items.o g_main.o g_misc.o g_monster.o \
	g_phys.o g_save.o g_spawn.o g_svcmds.o g_target.o g_trigger.o \
	g_turret.o g_utils.o g_weapon.o g_xmisc.o m_move.o \
	p_client.o p_hud.o p_trail.o p_view.o p_weapon.o q_shared.o \
	tng_stats.o tng_flashlight.o tng_irc.o

game$(ARCH).$(SHLIBEXT) : $(GAME_OBJS)
	$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GAME_OBJS)


#############################################################################
# MISC
#############################################################################

clean:
	-rm -f $(GAME_OBJS) game$(ARCH).$(SHLIBEXT)

olddepend:
	gcc -MM $(GAME_OBJS:.o=.c)

depend:
	makedepend -I. -- $(CFLAGS) -- $(GAME_OBJS:.o=.c)

install:
	cp gamei386.so ../action/
	strip ../action/gamei386.so

bindist: game$(ARCH).$(SHLIBEXT)
	strip gamei386.so
	tar -zchvf $(PACKAGE)-$(VERSION)-x86-linux.tar.gz \
		gamei386.so README CHANGES maplist.ini

distclean: clean
	makedepend
	-rm -f *~ Makefile.bak gamei386.so $(PACKAGE)-$(VERSION)*.tar.gz

dist: distclean
	-rm -rf $(PACKAGE)-$(VERSION)
	mkdir $(PACKAGE)-$(VERSION)
	cd $(PACKAGE)-$(VERSION); \
	for f in $(GAME_OBJS:.o=.c) *.h Makefile README CHANGES maplist.ini; \
		do ln -s ../$$f .; done
	tar -zchvf $(PACKAGE)-$(VERSION).tar.gz $(PACKAGE)-$(VERSION)
	rm -rf $(PACKAGE)-$(VERSION)

