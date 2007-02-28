 ## Process this file with automake to produce Makefile.in

bin_PROGRAMS = bwm-ng

bwm_ng_SOURCES = bwm-ng.c process.c output.c options.c help.c curses_tools.c

EXTRA_bwm_ng_SOURCES = input/retrieve.c input/devstat.c input/libkstat.c input/netstat.c input/proc_net_dev.c input/sysctl.c input/ioservice.c input/libstatgrab.c input/proc_diskstats.c input/win32.c input/getifaddrs.c

bwm_ng_LDADD = retrieve.$(OBJEXT) $(INPUT_SYSTEM)
bwm_ng_DEPENDENCIES = retrieve.$(OBJEXT) $(INPUT_SYSTEM)

noinst_HEADERS = bwm-ng.h process.h output.h options.h help.h curses_tools.h types.h defines.h global_vars.h input/devstat.h input/libkstat.h input/netstat.h input/proc_net_dev.h input/sysctl.h input/ioservice.h input/libstatgrab.h input/proc_diskstats.h input/retrieve.h input/win32.h input/getifaddrs.h

