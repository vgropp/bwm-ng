#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT
AC_CONFIG_MACRO_DIR(./)
AC_LANG([C])

if test -z ${CFLAGS}; then
	CFLAGS="-O3 -Wall"
fi

AC_CONFIG_SRCDIR([src/bwm-ng.c])
AC_CONFIG_HEADER([config.h:config.h.in])


# Checks for libraries.
CURSES_CHECKED="0"

AC_ARG_WITH([curses],AS_HELP_STRING([--with-curses],[with curses (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_LIB([curses],[initscr],,[ CPPFLAGS=$OLD_CPPFLAGS ])
    CURSES_CHECKED="1"
else
    CURSES_CHECKED="1"
fi  
])


if [ test $CURSES_CHECKED != "1" ]; then
AC_ARG_WITH([ncurses],AS_HELP_STRING([--with-ncurses],[prefer ncurses over curses (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_LIB([ncurses],[initscr],,[ CPPFLAGS=$OLD_CPPFLAGS ])
    CURSES_CHECKED="1"
else
    CURSES_CHECKED="1"
fi
])
fi



if [ test $CURSES_CHECKED != "1" ]; then
AC_CHECK_LIB([curses],[initscr],,[
	AC_CHECK_LIB([ncurses],[initscr])
])
fi

AC_ARG_WITH([varargs],AS_HELP_STRING([--with-varargs],[enable varargs.h use (solaris) (ARG=PATH)]),[
echo "checking for varargs.h: $withval"
if [ test $withval != "no" ]; then  
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_HEADER([sys/varargs.h],[AC_DEFINE_UNQUOTED(HAVE_SYS_VARARGS_H)],[ CPPFLAGS=$OLD_CPPFLAGS ])
    
fi
],[
    echo "checking for varargs.h: yes"
    AC_CHECK_HEADER([sys/varargs.h],[AC_DEFINE_UNQUOTED(HAVE_SYS_VARARGS_H)])
])


AC_ARG_WITH([time],AS_HELP_STRING([--with-time],[enable accurate time calculating (ARG=PATH)]),[
echo "checking accurate time calc: $withval"
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_HEADER([time.h],[
        AC_CHECK_HEADER([sys/time.h],[
            AC_CHECK_FUNCS([gettimeofday],,[ CPPFLAGS=$OLD_CPPFLAGS ])
        ],[ CPPFLAGS=$OLD_CPPFLAGS ])
    ],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi
],[
    echo "checking accurate time calc: yes"
    AC_CHECK_HEADER([time.h],[
        AC_CHECK_HEADER([sys/time.h],[
            AC_CHECK_FUNCS([gettimeofday])
        ])
    ])
])

AC_ARG_WITH([getopt_long],AS_HELP_STRING([--with-getopt_long],[enable getopt_long for long options (ARG=PATH)]),[
echo "checking for getopt_long: $withval"
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_HEADERS([getopt.h],[
        AC_CHECK_FUNCS([getopt_long],,[ CPPFLAGS=$OLD_CPPFLAGS ])
    ],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi
],[
AC_CHECK_HEADERS([getopt.h],[
    AC_CHECK_FUNCS([getopt_long])
])
])

#check for getifaddrs
net_if_h_includes="\
            #include <stdio.h>
            #if STDC_HEADERS
            # include <stdlib.h>
            # include <stddef.h>
            #else
            # if HAVE_STDLIB_H
            #  include <stdlib.h>
            # endif
            #endif
            #if HAVE_SYS_SOCKET_H
            # include <sys/socket.h>
            #endif
            "

AC_ARG_WITH([getifaddrs],AS_HELP_STRING([--with-getifaddrs],[enable getifaddrs input (ARG=PATH)]),[
echo "checking for getifaddrs input: $withval"
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_HEADER([ifaddrs.h],[
        AC_CHECK_FUNCS([getifaddrs],[
            AC_CHECK_HEADERS([sys/socket.h])
            AC_CHECK_HEADERS([net/if.h], [
                AC_CHECK_MEMBERS([struct if_data.ifi_ibytes],,[ CPPFLAGS=$OLD_CPPFLAGS ],[
                    $net_if_h_includes
                    #include <net/if.h>
               ])
            ], [ CPPFLAGS=$OLD_CPPFLAGS ],
            [$net_if_h_includes])
        ],[CPPFLAGS=$OLD_CPPFLAGS])
    ],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi
],[
AC_CHECK_HEADER([ifaddrs.h],[
	AC_CHECK_FUNCS([getifaddrs],[
		AC_CHECK_HEADERS([sys/socket.h])
		AC_CHECK_HEADERS([net/if.h], [
	        AC_CHECK_MEMBERS([struct if_data.ifi_ibytes],,,[
				$net_if_h_includes
				#include <net/if.h>
			])
		], [],
		[$net_if_h_includes])
	])
])
])

AC_ARG_WITH([sysctl],AS_HELP_STRING([--with-sysctl],[enable sysctl input (ARG=PATH)]),[
echo "checking for sysctl input: $withval"
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_FUNCS([sysctl],[
        AC_CHECK_HEADERS([sys/socket.h])
        AC_CHECK_HEADERS([net/if.h], [
            AC_CHECK_MEMBERS([struct if_msghdr.ifm_data],,[ CPPFLAGS=$OLD_CPPFLAGS ],[
                $net_if_h_includes
                #include <net/if.h>
            ])
        ], [ CPPFLAGS=$OLD_CPPFLAGS ],
        [$net_if_h_includes])
    ],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi
],[
AC_CHECK_FUNCS([sysctl],[
    AC_CHECK_HEADERS([sys/socket.h])
    AC_CHECK_HEADERS([net/if.h], [
    	AC_CHECK_MEMBERS([struct if_msghdr.ifm_data],,,[
    		$net_if_h_includes
    		#include <net/if.h>
    	])
    ],,[$net_if_h_includes])
])
])

AC_CHECK_FUNCS([ioctl])

AC_ARG_WITH([procnetdev],AS_HELP_STRING([--with-procnetdev],[enable proc/net/dev like input (ARG=PATH)]),[
echo "checking for /proc/net/dev style input: $withval"
if [ test $withval = "yes" ]; then
    AC_DEFINE_UNQUOTED(HAVE__PROC_NET_DEV)
else
    if [ test $withval != "no" ]; then
        AC_CHECK_FILES([$withval],[
        AC_DEFINE_UNQUOTED(PROC_NET_DEV_PATH,$withval)
        AC_DEFINE_UNQUOTED(HAVE__PROC_NET_DEV)
        ])
    fi
fi
],[
AC_CHECK_FILES([/proc/net/dev])
])

DC_ASK_OPTLIB(statgrab, sg_get_network_io_stats, statgrab.h, [     Use libstatgrab to gather statistics.], libstatgrab, HAVE_LIBSTATGRAB, HAVE_STATGRAB_H)

AC_ARG_ENABLE([html],AS_HELP_STRING([--enable-html],[enable html output (default: yes)]),[
echo "enable html output: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED(HTML)
fi
],[
echo "enable html output: yes"
AC_DEFINE_UNQUOTED(HTML)
])
AC_ARG_ENABLE([csv],AS_HELP_STRING([--enable-csv],[enable csv output (default: yes)]),[
echo "enable csv output: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED(CSV)
fi
],[
echo "enable csv output: yes"
AC_DEFINE_UNQUOTED(CSV)
])

AC_ARG_ENABLE([configfile],AS_HELP_STRING([--enable-configfile],[enable configfile support (default: yes)]),[
echo "enable config-file support: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED(CONFIG_FILE)
fi
],[
echo "enable config-file support: yes"
AC_DEFINE_UNQUOTED(CONFIG_FILE)
])

AC_ARG_ENABLE([64bit],AS_HELP_STRING([--enable-64bit],[enable 64bit support (default: no)]),[
echo "enable 64bit support: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED(SYS_64BIT)
fi
])

AC_CHECK_HEADERS([pwd.h],[
    AC_CHECK_FUNCS([getpwuid],[
        AC_DEFINE_UNQUOTED(HAVE_GETPWUID)
    ])
])

AC_ARG_ENABLE([netstatpath],AS_HELP_STRING([--enable-netstatpath],[enable path to netstat as runtime option (default: no), WARNING THIS IS A SECURITY FLAW!]),[
echo "enable path to netstat as option: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED(ALLOW_NETSTATPATH)
fi
],[
echo "enable path to netstat as option: no"
])

NETSTAT_BYTE="0"
AC_ARG_ENABLE([netstatbyte],AS_HELP_STRING([--enable-netstatbyte],[enable netstat with byte info (default: no)]),[
echo "enable netstat with byte info: $enableval"
if [ test $enableval = "yes" ]; then
    NETSTAT_BYTE="1"
fi
])

NETSTAT_LINK="0"
AC_ARG_ENABLE([netstatlink],AS_HELP_STRING([--enable-netstatlink],[enable netstat with link limit (default: no)]),[
echo "enable netstat with link limit: $enableval"
if [ test $enableval = "yes" ]; then
    NETSTAT_LINK="1"
fi
])


NETSTAT_DEFINED="0"
AC_ARG_WITH([netstatbsd],AS_HELP_STRING([--with-netstatbsd],[enable bsd like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "bsd like version with byte info forced: $withval"
        AC_DEFINE_UNQUOTED(HAVE_NETSTAT_PATH,$withval)
    else
        echo "bsd like version with byte info forced"
    fi
    if [ test $NETSTAT_BYTE != "1" ]; then
        AC_DEFINE_UNQUOTED(NETSTAT_BSD)
    else
        AC_DEFINE_UNQUOTED(NETSTAT_BSD_BYTES)
    fi
    if [ test $NETSTAT_LINK != "0" ]; then
        AC_DEFINE_UNQUOTED(NETSTAT_BSD_LINK)
    fi
else 
    NETSTAT_DEFINED="1"
    echo "disable netstat: bsd like version"
fi
])

AC_ARG_WITH([netstatlinux],AS_HELP_STRING([--with-netstatlinux],[enable linux like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "GNU net-tools like version forced: $withval"
        AC_DEFINE_UNQUOTED(HAVE_NETSTAT_PATH,$withval)
    else
        echo "GNU net-tools like version forced"
    fi
    AC_DEFINE_UNQUOTED(NETSTAT_LINUX)
    NETSTAT_DEFINED="1"
else
    NETSTAT_DEFINED="1"
    echo "disable netstat: GNU net-tools like version"
fi
])


AC_ARG_WITH([netstatsolaris],AS_HELP_STRING([--with-netstatsolaris],[enable solaris like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "solaris like version forced: $withval"
        AC_DEFINE_UNQUOTED(HAVE_NETSTAT_PATH,$withval)
    else
        echo "solaris like version forced"
    fi
    AC_DEFINE_UNQUOTED(NETSTAT_SOLARIS)
    NETSTAT_DEFINED="1"
else
    NETSTAT_DEFINED="1"
    echo "disable netstat: solaris like version"
fi
])

AC_ARG_WITH([netstatnetbsd],AS_HELP_STRING([--with-netstatnetbsd],[enable NetBSD/OpenBSD like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "NetBSD/OpenBSD like version forced: $withval"
        AC_DEFINE_UNQUOTED(HAVE_NETSTAT_PATH,$withval)
    else
        echo "NetBSD/OpenBSD like version forced"
    fi
    AC_DEFINE_UNQUOTED(NETSTAT_NETBSD)
    NETSTAT_DEFINED="1"
else
    NETSTAT_DEFINED="1"
    echo "disable netstat: NetBSD/OpenBSD like version"
fi
])



if [ test $NETSTAT_DEFINED != "1" ]; then
AC_PROG_EGREP
if [ test "${EGREP}" ]; then
    AC_CHECK_PROGS([NETSTAT_CMD],[netstat])
    if [ test "${NETSTAT_CMD}" ]; then
		echo -n "checking netstat version... "
        net_test=`${NETSTAT_CMD} -ia 2>/dev/null | ${EGREP} "Iface +MTU +Met +RX-OK +RX-ERR +RX-DRP +RX-OVR +TX-OK +TX-ERR +TX-DRP +TX-OVR +Flg"`
        if [ test "${net_test}" ]; then
            echo "GNU net-tools like version found"
			AC_DEFINE_UNQUOTED(NETSTAT_LINUX)
        else
            net_test=`${NETSTAT_CMD} -iW 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ipkts +Ierrs +Opkts +Oerrs +Coll"`
            if [ test "${net_test}" ]; then
				net_test2=`${NETSTAT_CMD} -iW -b 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ipkts +Ierrs +Ibytes +Opkts +Oerrs +Obytes +Coll"`
				if [ test "${net_test2}" ]; then
					echo "bsd like version with byte info found"
					AC_DEFINE_UNQUOTED(NETSTAT_BSD_BYTES)
				else
	                echo "bsd like version found"
					AC_DEFINE_UNQUOTED(NETSTAT_BSD)
				fi
				echo -n "checking for netstat -f link: "
				net_test3=`${NETSTAT_CMD} -iW -f link 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ipkts +Ierrs +Opkts +Oerrs +Coll"`
				if [ test "${net_test3}" ]; then
					echo "OK"
					AC_DEFINE_UNQUOTED(NETSTAT_BSD_LINK)
				else
					echo "not working"
				fi
			else
                net_test=`${NETSTAT_CMD} -i -f inet -f inet6 2>/dev/null | ${EGREP} "Name +Mtu +Net/Dest +Address +Ipkts +Ierrs +Opkts +Oerrs +Collis +Queue"`
                if [ test "${net_test}" ]; then
                    echo "solaris version found"
                    AC_DEFINE_UNQUOTED(NETSTAT_SOLARIS)
                else 
                    net_test=`${NETSTAT_CMD} -ibd 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ibytes +Obytes +Drops"`
                    if [ test "${net_test}" ]; then
                        echo "Net/OpenBSD version found"
                        AC_DEFINE_UNQUOTED(NETSTAT_NETBSD)
                    else
        				echo "unknown netstat version - disabled
*********************************************
  please mail 'netstat -i' output to author
*********************************************"
                    fi
                fi
            fi
        fi
    fi
fi
fi

QEF_C_NORETURN
AC_C_INLINE

AC_ARG_WITH([strip],AS_HELP_STRING([--with-strip],[use strip after make (ARG=PATH)]),[
echo "enable strip: $enableval"
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        STRIP="${withval} \${TARGET}"
    else
        AC_CHECK_FILES([/usr/ccs/bin/strip],[
            AC_CHECK_PROGS([STRIP],[strip],,,[/usr/ccs/bin])
        ])
        if [ test "${STRIP}" ]; then
            STRIP="${STRIP} \${TARGET}"
        else
        AC_CHECK_PROGS([STRIP],[strip])
        if [ test "${STRIP}" ]; then
            STRIP="${STRIP} \${TARGET}"
        fi
        fi
    fi
fi
],[
echo "enable strip: yes"
AC_CHECK_FILES([/usr/ccs/bin/strip],[
    AC_CHECK_PROGS([STRIP],[strip],,,[/usr/ccs/bin])
])
if [ test "${STRIP}" ]; then
    STRIP="/usr/ccs/bin/${STRIP} \${TARGET}"
else
AC_CHECK_PROGS([STRIP],[strip])
if [ test "${STRIP}" ]; then
    STRIP="${STRIP} \${TARGET}"
fi
fi
])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_OUTPUT
