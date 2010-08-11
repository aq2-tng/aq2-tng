# GNU Make required
#
# Tested platforms: Linux x86
#

GIT_REV=$(shell git rev-parse --short HEAD)
ifeq ($(GIT_REV),)
GIT_REV=git
endif
PACKAGE=aq2-tng
REL_VERSION=2.81
VERSION=$(REL_VERSION)~$(GIT_REV)

# remember to update this for every release commit
RELEASE=no

MACHINE=$(shell uname -m)
SYSTEM=$(shell uname -s)
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
	CFLAGS=$(BASE_CFLAGS) -g -Wall
else
	CFLAGS=$(BASE_CFLAGS) -march=$(TUNE) -mtune=$(TUNE) -O6 -ffast-math -funroll-loops \
		-fomit-frame-pointer -fexpensive-optimizations -falign-loops=2 \
		-falign-jumps=2 -falign-functions=2
endif

DO_CC=$(CC) $(CFLAGS) $(SHLIBCFLAGS) -o $@ -c $<
DO_LINK=$(CC) $(CFLAGS) $(SHLIBLDFLAGS) -o $@ $(GAME_OBJS) $(SHLIBLIBS)

SHLIBEXT?=so
SHLIBCFLAGS?=-fPIC
SHLIBLDFLAGS?=-shared

ifdef WIN32
CC=wine cl -IC:\\include -nologo
CFLAGS=$(BASE_CFLAGS) -DWIN32 -O1
LINK=wine link -nologo
ARCH=x86
SHLIBEXT=dll
DO_CC=$(CC) $(CFLAGS) -Fo$@ -c $<
DO_LINK=$(LINK) -LIBPATH:C:\\lib $(GAME_OBJS) ws2_32.lib /DEF:game.def /DLL /OUT:game$(ARCH).$(SHLIBEXT)
endif

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
	tng_stats.o tng_flashlight.o tng_irc.o tng_ini.o tng_balancer.o \
	tng_jump.o g_grapple.o

game$(ARCH).$(SHLIBEXT) : $(GAME_OBJS)
	$(DO_LINK)


#############################################################################
# MISC
#############################################################################

clean:
	-rm -f $(GAME_OBJS) game$(ARCH).$(SHLIBEXT)

install:
	cp gamei386.so ../action/
	strip ../action/gamei386.so

bindist: game$(ARCH).$(SHLIBEXT)
	strip -s game$(ARCH).$(SHLIBEXT)
	tar -zcvf ../$(PACKAGE)-$(VERSION)-$(SYSTEM)-$(MACHINE).tar.gz \
            game$(ARCH).$(SHLIBEXT) ../TNG-manual.txt ../change.txt

distclean: clean
	-rm -f ../$(PACKAGE)-*.{tar.gz,zip}

dist:
	git archive --format=tar --prefix=$(PACKAGE)-$(VERSION)-src/ HEAD | gzip -9 > ../$(PACKAGE)-$(VERSION)-src.tar.gz

