#-----------------------------------------------------------------------------
# Action makefile 
#
# $Id: Makefile,v 1.11 2001/06/19 21:10:05 igor_rock Exp $
#
# Intended for gcc/Linux, may need modifying for other platforms
# Adapted by PG BUND from Black Cross makefile
# (not sure if it will do it :)
# corrected and expanded by Igor[Rock]
# (needs still some testing/modification for the dist-targets)
#-----------------------------------------------------------------------------
# $Log: Makefile,v $
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

ARCH=i386
CC=gcc
BASE_CFLAGS=-Dstricmp=strcasecmp -DC_ONLY

#use these cflags to optimize it
CFLAGS=$(BASE_CFLAGS) -m486 -O6 -ffast-math -funroll-loops \
	-fomit-frame-pointer -fexpensive-optimizations -malign-loops=2 \
	-malign-jumps=2 -malign-functions=2
#use these when debugging 
#CFLAGS=$(BASE_CFLAGS) -g -Wall

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
	a_ban.o a_cmds.o a_ctf.o a_doorkick.o a_game.o a_items.o a_match.o \
	a_menu.o a_radio.o a_team.o a_tourney.o a_vote.o a_xcmds.o a_xgame.o \
	a_xmenu.o a_xvote.o cgf_sfx_glass.o g_ai.o g_chase.o g_cmds.o \
	g_combat.o g_func.o g_items.o g_main.o g_misc.o g_monster.o \
	g_phys.o g_save.o g_spawn.o g_svcmds.o g_target.o g_trigger.o \
	g_turret.o g_utils.o g_weapon.o g_xmisc.o m_move.o \
	p_client.o p_hud.o p_trail.o p_view.o p_weapon.o q_shared.o

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

# DO NOT DELETE

a_ban.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_ban.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_ban.o: /usr/include/bits/types.h
a_ban.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_ban.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_ban.o: /usr/include/assert.h /usr/include/math.h
a_ban.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_ban.o: /usr/include/bits/mathcalls.h
a_ban.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_ban.o: /usr/include/stdio.h
a_ban.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_ban.o: /usr/include/libio.h /usr/include/_G_config.h
a_ban.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_ban.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_ban.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_ban.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_ban.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_ban.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_ban.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_cmds.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_cmds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_cmds.o: /usr/include/bits/types.h
a_cmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_cmds.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_cmds.o: /usr/include/assert.h /usr/include/math.h
a_cmds.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_cmds.o: /usr/include/bits/mathcalls.h
a_cmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_cmds.o: /usr/include/stdio.h
a_cmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_cmds.o: /usr/include/libio.h /usr/include/_G_config.h
a_cmds.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_cmds.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_cmds.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_cmds.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_cmds.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_cmds.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_cmds.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_ctf.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_ctf.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_ctf.o: /usr/include/bits/types.h
a_ctf.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_ctf.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_ctf.o: /usr/include/assert.h /usr/include/math.h
a_ctf.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_ctf.o: /usr/include/bits/mathcalls.h
a_ctf.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_ctf.o: /usr/include/stdio.h
a_ctf.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_ctf.o: /usr/include/libio.h /usr/include/_G_config.h
a_ctf.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_ctf.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_ctf.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_ctf.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_ctf.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_ctf.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_ctf.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_doorkick.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_doorkick.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_doorkick.o: /usr/include/bits/types.h
a_doorkick.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_doorkick.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_doorkick.o: /usr/include/assert.h /usr/include/math.h
a_doorkick.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_doorkick.o: /usr/include/bits/mathcalls.h
a_doorkick.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_doorkick.o: /usr/include/stdio.h
a_doorkick.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_doorkick.o: /usr/include/libio.h /usr/include/_G_config.h
a_doorkick.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_doorkick.o: /usr/include/stdlib.h /usr/include/sys/types.h
a_doorkick.o: /usr/include/time.h /usr/include/sys/select.h
a_doorkick.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
a_doorkick.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
a_doorkick.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
a_doorkick.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
a_doorkick.o: a_match.h a_ctf.h
a_game.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_game.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_game.o: /usr/include/bits/types.h
a_game.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_game.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_game.o: /usr/include/assert.h /usr/include/math.h
a_game.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_game.o: /usr/include/bits/mathcalls.h
a_game.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_game.o: /usr/include/stdio.h
a_game.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_game.o: /usr/include/libio.h /usr/include/_G_config.h
a_game.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_game.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_game.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_game.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_game.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_game.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_game.o: a_ban.h a_vote.h a_match.h a_ctf.h cgf_sfx_glass.h
a_items.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_items.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_items.o: /usr/include/bits/types.h
a_items.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_items.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_items.o: /usr/include/assert.h /usr/include/math.h
a_items.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_items.o: /usr/include/bits/mathcalls.h
a_items.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_items.o: /usr/include/stdio.h
a_items.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_items.o: /usr/include/libio.h /usr/include/_G_config.h
a_items.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_items.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_items.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_items.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_items.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_items.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_items.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_match.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_match.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_match.o: /usr/include/bits/types.h
a_match.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_match.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_match.o: /usr/include/assert.h /usr/include/math.h
a_match.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_match.o: /usr/include/bits/mathcalls.h
a_match.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_match.o: /usr/include/stdio.h
a_match.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_match.o: /usr/include/libio.h /usr/include/_G_config.h
a_match.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_match.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_match.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_match.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_match.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_match.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_match.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_menu.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_menu.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_menu.o: /usr/include/bits/types.h
a_menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_menu.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_menu.o: /usr/include/assert.h /usr/include/math.h
a_menu.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_menu.o: /usr/include/bits/mathcalls.h
a_menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_menu.o: /usr/include/stdio.h
a_menu.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_menu.o: /usr/include/libio.h /usr/include/_G_config.h
a_menu.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_menu.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_menu.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_menu.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_menu.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_menu.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_menu.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_radio.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_radio.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_radio.o: /usr/include/bits/types.h
a_radio.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_radio.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_radio.o: /usr/include/assert.h /usr/include/math.h
a_radio.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_radio.o: /usr/include/bits/mathcalls.h
a_radio.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_radio.o: /usr/include/stdio.h
a_radio.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_radio.o: /usr/include/libio.h /usr/include/_G_config.h
a_radio.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_radio.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_radio.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_radio.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_radio.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_radio.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_radio.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_team.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_team.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_team.o: /usr/include/bits/types.h
a_team.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_team.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_team.o: /usr/include/assert.h /usr/include/math.h
a_team.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_team.o: /usr/include/bits/mathcalls.h
a_team.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_team.o: /usr/include/stdio.h
a_team.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_team.o: /usr/include/libio.h /usr/include/_G_config.h
a_team.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_team.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_team.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_team.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_team.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_team.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_team.o: a_ban.h a_vote.h a_match.h a_ctf.h cgf_sfx_glass.h
a_tourney.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_tourney.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_tourney.o: /usr/include/bits/types.h
a_tourney.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_tourney.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_tourney.o: /usr/include/assert.h /usr/include/math.h
a_tourney.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_tourney.o: /usr/include/bits/mathcalls.h
a_tourney.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_tourney.o: /usr/include/stdio.h
a_tourney.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_tourney.o: /usr/include/libio.h /usr/include/_G_config.h
a_tourney.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_tourney.o: /usr/include/stdlib.h /usr/include/sys/types.h
a_tourney.o: /usr/include/time.h /usr/include/sys/select.h
a_tourney.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
a_tourney.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
a_tourney.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
a_tourney.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
a_tourney.o: a_match.h a_ctf.h
a_vote.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_vote.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_vote.o: /usr/include/bits/types.h
a_vote.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_vote.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_vote.o: /usr/include/assert.h /usr/include/math.h
a_vote.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_vote.o: /usr/include/bits/mathcalls.h
a_vote.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_vote.o: /usr/include/stdio.h
a_vote.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_vote.o: /usr/include/libio.h /usr/include/_G_config.h
a_vote.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_vote.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_vote.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_vote.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_vote.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_vote.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_vote.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_xcmds.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_xcmds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_xcmds.o: /usr/include/bits/types.h
a_xcmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_xcmds.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_xcmds.o: /usr/include/assert.h /usr/include/math.h
a_xcmds.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_xcmds.o: /usr/include/bits/mathcalls.h
a_xcmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_xcmds.o: /usr/include/stdio.h
a_xcmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_xcmds.o: /usr/include/libio.h /usr/include/_G_config.h
a_xcmds.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_xcmds.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_xcmds.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_xcmds.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_xcmds.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_xcmds.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_xcmds.o: a_ban.h a_vote.h a_match.h a_ctf.h m_player.h
a_xgame.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_xgame.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_xgame.o: /usr/include/bits/types.h
a_xgame.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_xgame.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_xgame.o: /usr/include/assert.h /usr/include/math.h
a_xgame.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_xgame.o: /usr/include/bits/mathcalls.h
a_xgame.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_xgame.o: /usr/include/stdio.h
a_xgame.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_xgame.o: /usr/include/libio.h /usr/include/_G_config.h
a_xgame.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_xgame.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_xgame.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_xgame.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_xgame.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_xgame.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_xgame.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_xmenu.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_xmenu.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_xmenu.o: /usr/include/bits/types.h
a_xmenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_xmenu.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_xmenu.o: /usr/include/assert.h /usr/include/math.h
a_xmenu.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_xmenu.o: /usr/include/bits/mathcalls.h
a_xmenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_xmenu.o: /usr/include/stdio.h
a_xmenu.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_xmenu.o: /usr/include/libio.h /usr/include/_G_config.h
a_xmenu.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_xmenu.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_xmenu.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_xmenu.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_xmenu.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_xmenu.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_xmenu.o: a_ban.h a_vote.h a_match.h a_ctf.h
a_xvote.o: g_local.h /usr/include/ctype.h /usr/include/features.h
a_xvote.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
a_xvote.o: /usr/include/bits/types.h
a_xvote.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
a_xvote.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
a_xvote.o: /usr/include/assert.h /usr/include/math.h
a_xvote.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
a_xvote.o: /usr/include/bits/mathcalls.h
a_xvote.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
a_xvote.o: /usr/include/stdio.h
a_xvote.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
a_xvote.o: /usr/include/libio.h /usr/include/_G_config.h
a_xvote.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
a_xvote.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
a_xvote.o: /usr/include/sys/select.h /usr/include/bits/select.h
a_xvote.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
a_xvote.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
a_xvote.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
a_xvote.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_ai.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_ai.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_ai.o: /usr/include/bits/types.h
g_ai.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_ai.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_ai.o: /usr/include/assert.h /usr/include/math.h
g_ai.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_ai.o: /usr/include/bits/mathcalls.h
g_ai.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_ai.o: /usr/include/stdio.h
g_ai.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_ai.o: /usr/include/libio.h /usr/include/_G_config.h
g_ai.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_ai.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_ai.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_ai.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_ai.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_ai.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h
g_ai.o: a_vote.h a_match.h a_ctf.h
g_chase.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_chase.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_chase.o: /usr/include/bits/types.h
g_chase.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_chase.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_chase.o: /usr/include/assert.h /usr/include/math.h
g_chase.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_chase.o: /usr/include/bits/mathcalls.h
g_chase.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_chase.o: /usr/include/stdio.h
g_chase.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_chase.o: /usr/include/libio.h /usr/include/_G_config.h
g_chase.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_chase.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_chase.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_chase.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_chase.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_chase.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_chase.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_cmds.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_cmds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_cmds.o: /usr/include/bits/types.h
g_cmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_cmds.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_cmds.o: /usr/include/assert.h /usr/include/math.h
g_cmds.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_cmds.o: /usr/include/bits/mathcalls.h
g_cmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_cmds.o: /usr/include/stdio.h
g_cmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_cmds.o: /usr/include/libio.h /usr/include/_G_config.h
g_cmds.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_cmds.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_cmds.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_cmds.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_cmds.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_cmds.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_cmds.o: a_ban.h a_vote.h a_match.h a_ctf.h m_player.h
g_combat.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_combat.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_combat.o: /usr/include/bits/types.h
g_combat.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_combat.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_combat.o: /usr/include/assert.h /usr/include/math.h
g_combat.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_combat.o: /usr/include/bits/mathcalls.h
g_combat.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_combat.o: /usr/include/stdio.h
g_combat.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_combat.o: /usr/include/libio.h /usr/include/_G_config.h
g_combat.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_combat.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_combat.o: /usr/include/time.h /usr/include/sys/select.h
g_combat.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_combat.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_combat.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_combat.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_combat.o: a_match.h a_ctf.h cgf_sfx_glass.h
g_func.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_func.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_func.o: /usr/include/bits/types.h
g_func.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_func.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_func.o: /usr/include/assert.h /usr/include/math.h
g_func.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_func.o: /usr/include/bits/mathcalls.h
g_func.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_func.o: /usr/include/stdio.h
g_func.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_func.o: /usr/include/libio.h /usr/include/_G_config.h
g_func.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_func.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_func.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_func.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_func.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_func.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_func.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_items.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_items.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_items.o: /usr/include/bits/types.h
g_items.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_items.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_items.o: /usr/include/assert.h /usr/include/math.h
g_items.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_items.o: /usr/include/bits/mathcalls.h
g_items.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_items.o: /usr/include/stdio.h
g_items.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_items.o: /usr/include/libio.h /usr/include/_G_config.h
g_items.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_items.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_items.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_items.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_items.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_items.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_items.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_main.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_main.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_main.o: /usr/include/bits/types.h
g_main.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_main.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_main.o: /usr/include/assert.h /usr/include/math.h
g_main.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_main.o: /usr/include/bits/mathcalls.h
g_main.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_main.o: /usr/include/stdio.h
g_main.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_main.o: /usr/include/libio.h /usr/include/_G_config.h
g_main.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_main.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_main.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_main.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_main.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_main.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_main.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_misc.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_misc.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_misc.o: /usr/include/bits/types.h
g_misc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_misc.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_misc.o: /usr/include/assert.h /usr/include/math.h
g_misc.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_misc.o: /usr/include/bits/mathcalls.h
g_misc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_misc.o: /usr/include/stdio.h
g_misc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_misc.o: /usr/include/libio.h /usr/include/_G_config.h
g_misc.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_misc.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_misc.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_misc.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_misc.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_misc.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_misc.o: a_ban.h a_vote.h a_match.h a_ctf.h cgf_sfx_glass.h
g_monster.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_monster.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_monster.o: /usr/include/bits/types.h
g_monster.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_monster.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_monster.o: /usr/include/assert.h /usr/include/math.h
g_monster.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_monster.o: /usr/include/bits/mathcalls.h
g_monster.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_monster.o: /usr/include/stdio.h
g_monster.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_monster.o: /usr/include/libio.h /usr/include/_G_config.h
g_monster.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_monster.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_monster.o: /usr/include/time.h /usr/include/sys/select.h
g_monster.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_monster.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_monster.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_monster.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_monster.o: a_match.h a_ctf.h
g_phys.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_phys.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_phys.o: /usr/include/bits/types.h
g_phys.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_phys.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_phys.o: /usr/include/assert.h /usr/include/math.h
g_phys.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_phys.o: /usr/include/bits/mathcalls.h
g_phys.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_phys.o: /usr/include/stdio.h
g_phys.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_phys.o: /usr/include/libio.h /usr/include/_G_config.h
g_phys.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_phys.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_phys.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_phys.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_phys.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_phys.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_phys.o: a_ban.h a_vote.h a_match.h a_ctf.h cgf_sfx_glass.h
g_save.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_save.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_save.o: /usr/include/bits/types.h
g_save.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_save.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_save.o: /usr/include/assert.h /usr/include/math.h
g_save.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_save.o: /usr/include/bits/mathcalls.h
g_save.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_save.o: /usr/include/stdio.h
g_save.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_save.o: /usr/include/libio.h /usr/include/_G_config.h
g_save.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_save.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_save.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_save.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_save.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_save.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_save.o: a_ban.h a_vote.h a_match.h a_ctf.h cgf_sfx_glass.h
g_spawn.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_spawn.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_spawn.o: /usr/include/bits/types.h
g_spawn.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_spawn.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_spawn.o: /usr/include/assert.h /usr/include/math.h
g_spawn.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_spawn.o: /usr/include/bits/mathcalls.h
g_spawn.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_spawn.o: /usr/include/stdio.h
g_spawn.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_spawn.o: /usr/include/libio.h /usr/include/_G_config.h
g_spawn.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_spawn.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_spawn.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_spawn.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_spawn.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_spawn.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_spawn.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_svcmds.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_svcmds.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_svcmds.o: /usr/include/bits/types.h
g_svcmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_svcmds.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_svcmds.o: /usr/include/assert.h /usr/include/math.h
g_svcmds.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_svcmds.o: /usr/include/bits/mathcalls.h
g_svcmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_svcmds.o: /usr/include/stdio.h
g_svcmds.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_svcmds.o: /usr/include/libio.h /usr/include/_G_config.h
g_svcmds.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_svcmds.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_svcmds.o: /usr/include/time.h /usr/include/sys/select.h
g_svcmds.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_svcmds.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_svcmds.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_svcmds.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_svcmds.o: a_match.h a_ctf.h
g_target.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_target.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_target.o: /usr/include/bits/types.h
g_target.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_target.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_target.o: /usr/include/assert.h /usr/include/math.h
g_target.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_target.o: /usr/include/bits/mathcalls.h
g_target.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_target.o: /usr/include/stdio.h
g_target.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_target.o: /usr/include/libio.h /usr/include/_G_config.h
g_target.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_target.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_target.o: /usr/include/time.h /usr/include/sys/select.h
g_target.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_target.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_target.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_target.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_target.o: a_match.h a_ctf.h
g_trigger.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_trigger.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_trigger.o: /usr/include/bits/types.h
g_trigger.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_trigger.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_trigger.o: /usr/include/assert.h /usr/include/math.h
g_trigger.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_trigger.o: /usr/include/bits/mathcalls.h
g_trigger.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_trigger.o: /usr/include/stdio.h
g_trigger.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_trigger.o: /usr/include/libio.h /usr/include/_G_config.h
g_trigger.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_trigger.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_trigger.o: /usr/include/time.h /usr/include/sys/select.h
g_trigger.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_trigger.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_trigger.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_trigger.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_trigger.o: a_match.h a_ctf.h
g_turret.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_turret.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_turret.o: /usr/include/bits/types.h
g_turret.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_turret.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_turret.o: /usr/include/assert.h /usr/include/math.h
g_turret.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_turret.o: /usr/include/bits/mathcalls.h
g_turret.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_turret.o: /usr/include/stdio.h
g_turret.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_turret.o: /usr/include/libio.h /usr/include/_G_config.h
g_turret.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_turret.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_turret.o: /usr/include/time.h /usr/include/sys/select.h
g_turret.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_turret.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_turret.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_turret.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_turret.o: a_match.h a_ctf.h
g_utils.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_utils.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_utils.o: /usr/include/bits/types.h
g_utils.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_utils.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_utils.o: /usr/include/assert.h /usr/include/math.h
g_utils.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_utils.o: /usr/include/bits/mathcalls.h
g_utils.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_utils.o: /usr/include/stdio.h
g_utils.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_utils.o: /usr/include/libio.h /usr/include/_G_config.h
g_utils.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_utils.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_utils.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_utils.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_utils.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_utils.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_utils.o: a_ban.h a_vote.h a_match.h a_ctf.h
g_weapon.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_weapon.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_weapon.o: /usr/include/bits/types.h
g_weapon.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_weapon.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_weapon.o: /usr/include/assert.h /usr/include/math.h
g_weapon.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_weapon.o: /usr/include/bits/mathcalls.h
g_weapon.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_weapon.o: /usr/include/stdio.h
g_weapon.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_weapon.o: /usr/include/libio.h /usr/include/_G_config.h
g_weapon.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_weapon.o: /usr/include/stdlib.h /usr/include/sys/types.h
g_weapon.o: /usr/include/time.h /usr/include/sys/select.h
g_weapon.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
g_weapon.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
g_weapon.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
g_weapon.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
g_weapon.o: a_match.h a_ctf.h cgf_sfx_glass.h
g_xmisc.o: g_local.h /usr/include/ctype.h /usr/include/features.h
g_xmisc.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
g_xmisc.o: /usr/include/bits/types.h
g_xmisc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
g_xmisc.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
g_xmisc.o: /usr/include/assert.h /usr/include/math.h
g_xmisc.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
g_xmisc.o: /usr/include/bits/mathcalls.h
g_xmisc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
g_xmisc.o: /usr/include/stdio.h
g_xmisc.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
g_xmisc.o: /usr/include/libio.h /usr/include/_G_config.h
g_xmisc.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
g_xmisc.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
g_xmisc.o: /usr/include/sys/select.h /usr/include/bits/select.h
g_xmisc.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
g_xmisc.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
g_xmisc.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
g_xmisc.o: a_ban.h a_vote.h a_match.h a_ctf.h m_player.h
m_move.o: g_local.h /usr/include/ctype.h /usr/include/features.h
m_move.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
m_move.o: /usr/include/bits/types.h
m_move.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
m_move.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
m_move.o: /usr/include/assert.h /usr/include/math.h
m_move.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
m_move.o: /usr/include/bits/mathcalls.h
m_move.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
m_move.o: /usr/include/stdio.h
m_move.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
m_move.o: /usr/include/libio.h /usr/include/_G_config.h
m_move.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
m_move.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
m_move.o: /usr/include/sys/select.h /usr/include/bits/select.h
m_move.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
m_move.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
m_move.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
m_move.o: a_ban.h a_vote.h a_match.h a_ctf.h
p_client.o: g_local.h /usr/include/ctype.h /usr/include/features.h
p_client.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
p_client.o: /usr/include/bits/types.h
p_client.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
p_client.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
p_client.o: /usr/include/assert.h /usr/include/math.h
p_client.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
p_client.o: /usr/include/bits/mathcalls.h
p_client.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
p_client.o: /usr/include/stdio.h
p_client.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
p_client.o: /usr/include/libio.h /usr/include/_G_config.h
p_client.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
p_client.o: /usr/include/stdlib.h /usr/include/sys/types.h
p_client.o: /usr/include/time.h /usr/include/sys/select.h
p_client.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
p_client.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
p_client.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
p_client.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
p_client.o: a_match.h a_ctf.h m_player.h cgf_sfx_glass.h
p_hud.o: g_local.h /usr/include/ctype.h /usr/include/features.h
p_hud.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
p_hud.o: /usr/include/bits/types.h
p_hud.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
p_hud.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
p_hud.o: /usr/include/assert.h /usr/include/math.h
p_hud.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
p_hud.o: /usr/include/bits/mathcalls.h
p_hud.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
p_hud.o: /usr/include/stdio.h
p_hud.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
p_hud.o: /usr/include/libio.h /usr/include/_G_config.h
p_hud.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
p_hud.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
p_hud.o: /usr/include/sys/select.h /usr/include/bits/select.h
p_hud.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
p_hud.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
p_hud.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
p_hud.o: a_ban.h a_vote.h a_match.h a_ctf.h
p_trail.o: g_local.h /usr/include/ctype.h /usr/include/features.h
p_trail.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
p_trail.o: /usr/include/bits/types.h
p_trail.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
p_trail.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
p_trail.o: /usr/include/assert.h /usr/include/math.h
p_trail.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
p_trail.o: /usr/include/bits/mathcalls.h
p_trail.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
p_trail.o: /usr/include/stdio.h
p_trail.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
p_trail.o: /usr/include/libio.h /usr/include/_G_config.h
p_trail.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
p_trail.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
p_trail.o: /usr/include/sys/select.h /usr/include/bits/select.h
p_trail.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
p_trail.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
p_trail.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
p_trail.o: a_ban.h a_vote.h a_match.h a_ctf.h
p_view.o: g_local.h /usr/include/ctype.h /usr/include/features.h
p_view.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
p_view.o: /usr/include/bits/types.h
p_view.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
p_view.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
p_view.o: /usr/include/assert.h /usr/include/math.h
p_view.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
p_view.o: /usr/include/bits/mathcalls.h
p_view.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
p_view.o: /usr/include/stdio.h
p_view.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
p_view.o: /usr/include/libio.h /usr/include/_G_config.h
p_view.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
p_view.o: /usr/include/stdlib.h /usr/include/sys/types.h /usr/include/time.h
p_view.o: /usr/include/sys/select.h /usr/include/bits/select.h
p_view.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
p_view.o: /usr/include/alloca.h game.h a_team.h a_game.h a_menu.h a_radio.h
p_view.o: a_xcmds.h a_xgame.h g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h
p_view.o: a_ban.h a_vote.h a_match.h a_ctf.h m_player.h
p_weapon.o: g_local.h /usr/include/ctype.h /usr/include/features.h
p_weapon.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
p_weapon.o: /usr/include/bits/types.h
p_weapon.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
p_weapon.o: /usr/include/endian.h /usr/include/bits/endian.h q_shared.h
p_weapon.o: /usr/include/assert.h /usr/include/math.h
p_weapon.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
p_weapon.o: /usr/include/bits/mathcalls.h
p_weapon.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
p_weapon.o: /usr/include/stdio.h
p_weapon.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
p_weapon.o: /usr/include/libio.h /usr/include/_G_config.h
p_weapon.o: /usr/include/bits/stdio_lim.h /usr/include/string.h
p_weapon.o: /usr/include/stdlib.h /usr/include/sys/types.h
p_weapon.o: /usr/include/time.h /usr/include/sys/select.h
p_weapon.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
p_weapon.o: /usr/include/sys/sysmacros.h /usr/include/alloca.h game.h
p_weapon.o: a_team.h a_game.h a_menu.h a_radio.h a_xcmds.h a_xgame.h
p_weapon.o: g_xmisc.h a_tourney.h a_xvote.h a_xmenu.h a_ban.h a_vote.h
p_weapon.o: a_match.h a_ctf.h m_player.h
q_shared.o: q_shared.h /usr/include/assert.h /usr/include/features.h
q_shared.o: /usr/include/sys/cdefs.h /usr/include/gnu/stubs.h
q_shared.o: /usr/include/math.h /usr/include/bits/huge_val.h
q_shared.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
q_shared.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/float.h
q_shared.o: /usr/include/stdio.h
q_shared.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stddef.h
q_shared.o: /usr/lib/gcc-lib/i486-linux/2.7.2.3/include/stdarg.h
q_shared.o: /usr/include/bits/types.h /usr/include/libio.h
q_shared.o: /usr/include/_G_config.h /usr/include/bits/stdio_lim.h
q_shared.o: /usr/include/string.h /usr/include/stdlib.h
q_shared.o: /usr/include/sys/types.h /usr/include/time.h
q_shared.o: /usr/include/endian.h /usr/include/bits/endian.h
q_shared.o: /usr/include/sys/select.h /usr/include/bits/select.h
q_shared.o: /usr/include/bits/sigset.h /usr/include/sys/sysmacros.h
q_shared.o: /usr/include/alloca.h
