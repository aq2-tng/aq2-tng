# Action makefile 
#
# Intended for gcc/Linux, may need modifying for other platforms
# Adapted by PG BUND from Black Cross makefile
# (not sure if it will do it :)
# corrected and expanded by Igor[Rock]
# (needs still some testing/modification for the dist-targets)
#-----------------------------------------------------------------------------

GIT_REV=$(shell git rev-parse --short HEAD)
PACKAGE=aq2-tng
REL_VERSION=2.81
VERSION=$(REL_VERSION)~$(GIT_REV)

# remember to update this for every release commit
RELEASE=no

MACHINE=$(shell uname -m)
ARCH=i386
TUNE=$(MACHINE)

ifeq ($(MACHINE),x86_64)
TUNE=athlon64
ARCH=x86_64
endif

ifeq ($(RELEASE),yes)
VERSION=$(REL_VERSION)
endif

CC?=gcc
BASE_CFLAGS=-DC_ONLY -DVERSION=\"$(VERSION)\"

#use these cflags to optimize it
ifdef DEBUG
	CFLAGS=$(BASE_CFLAGS) -g -O2 -Wall
else
	CFLAGS=$(BASE_CFLAGS) -march=$(TUNE) -mtune=$(TUNE) -O6 -ffast-math -funroll-loops \
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

