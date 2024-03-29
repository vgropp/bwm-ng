#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_INIT([bwm-ng],[0.6.3],[bwmng@gropp.org],[bwm-ng])
AC_CONFIG_SRCDIR(src/bwm-ng.c)
AM_INIT_AUTOMAKE([gnu])
AC_CONFIG_HEADERS(config.h)
AC_CONFIG_MACRO_DIR(./)
AC_LANG([C])

AC_ARG_ENABLE([debug],AS_HELP_STRING([--enable-debug],[enable debug (default: no)]),[
echo "enable debugging info: $enableval"
if [ test $enableval = "yes" ]; then
    if test -z ${CFLAGS}; then
        CFLAGS="-g3 -Werror=format-security -Wall"
    else
        CFLAGS="-g3 ${CFLAGS}"
    fi
    DEBUG="1"
fi
],[
if test -z "${CFLAGS}"; then
    CFLAGS="-O2 -Wall -Werror=format-security"
fi
])

AC_OBJEXT
AC_CHECK_TYPES(unsigned long long)

# Checks for libraries.
CURSES_CHECKED="0"

AC_ARG_WITH([curses],AS_HELP_STRING([--with-curses],[with curses (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_LIB([curses],[newterm,timeout],,[ CPPFLAGS=$OLD_CPPFLAGS ])
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
    AC_CHECK_LIB([ncurses],[newterm,timeout],,[ CPPFLAGS=$OLD_CPPFLAGS ])
    CURSES_CHECKED="1"
else
    CURSES_CHECKED="1"
fi
])
fi



if [ test $CURSES_CHECKED != "1" ]; then
AC_CHECK_LIB([curses],[initscr,timeout],,[
	AC_CHECK_LIB([ncurses],[newterm,timeout])
])
fi

AC_CHECK_FUNCS([wattron])
AC_CHECK_FUNCS([curs_set])
AC_CHECK_FUNCS([usleep])
AC_CHECK_FUNCS([strlcpy])
AC_FUNC_FORK

AC_ARG_WITH([time],AS_HELP_STRING([--with-time],[enable accurate time calculating (ARG=PATH)]),[
AC_MSG_CHECKING([for accurate time calc])
AC_MSG_RESULT($withval)
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
    AC_MSG_CHECKING([for accurate time calc])
    echo "yes"
    AC_CHECK_HEADER([time.h],[
        AC_CHECK_HEADER([sys/time.h],[
            AC_CHECK_FUNCS([gettimeofday])
        ])
    ])
])

AC_ARG_WITH([getopt_long],AS_HELP_STRING([--with-getopt_long],[enable getopt_long for long options (ARG=PATH)]),[
AC_MSG_CHECKING([for getopt_long])
AC_MSG_RESULT($withval)
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


#####################################################
# check for inputs
#####################################################

INPUT_FOUND=0
DISK_INPUT_FOUND=0
INPUT_SYSTEM=""
SYSCTL_ADDED=0

#check for getifaddrs
net_if_h_includes="
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
AC_MSG_CHECKING([for getifaddrs input])
AC_MSG_RESULT($withval)
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_HEADER([ifaddrs.h],[
        AC_CHECK_FUNCS([getifaddrs],[
            AC_CHECK_HEADERS([sys/socket.h])
            AC_CHECK_HEADERS([net/if.h], [
                AC_CHECK_MEMBERS([struct if_data.ifi_ibytes],[
                    AC_DEFINE_UNQUOTED([HAVE_WORKING_GETIFADDRS],[1],[getifaddrs works as espected])
						  INPUT_FOUND=1
						  INPUT_SYSTEM="$INPUT_SYSTEM getifaddrs.$OBJEXT"
                ],[ CPPFLAGS=$OLD_CPPFLAGS ],[
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
	        AC_CHECK_MEMBERS([struct if_data.ifi_ibytes],[
                AC_CHECK_WORKING_GETIFADDRS()
            ],,[
				$net_if_h_includes
				#include <net/if.h>
			])
		], [],
		[$net_if_h_includes])
	])
])
])

AC_ARG_WITH([sysctl],AS_HELP_STRING([--with-sysctl],[enable sysctl input (ARG=PATH)]),[
AC_MSG_CHECKING([for sysctl input])
AC_MSG_RESULT($withval)
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_FUNCS([sysctl],[
        AC_CHECK_HEADERS([sys/socket.h])
        AC_CHECK_HEADERS([net/if.h], [
            AC_CHECK_MEMBERS([struct if_msghdr.ifm_data],[
					INPUT_FOUND=1
					INPUT_SYSTEM="$INPUT_SYSTEM sysctl.$OBJEXT"
					SYSCTL_ADDED=1
					AC_DEFINE_UNQUOTED([HAVE_STRUCT_IF_MSGHDR_IFM_DATA],[1],[sysctl input fully available])
				],[ CPPFLAGS=$OLD_CPPFLAGS ],[
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
    	AC_CHECK_MEMBERS([struct if_msghdr.ifm_data],[
					INPUT_FOUND=1
					INPUT_SYSTEM="$INPUT_SYSTEM sysctl.$OBJEXT"
					SYSCTL_ADDED=1
					AC_DEFINE_UNQUOTED([HAVE_STRUCT_IF_MSGHDR_IFM_DATA],[1],[sysctl input fully available])
			],,[
    		$net_if_h_includes
    		#include <net/if.h>
    	])
    ],,[$net_if_h_includes])
	 ])
])


AC_ARG_WITH([sysctldisk],AS_HELP_STRING([--with-sysctldisk],[enable sysctl disk input (ARG=PATH)]),[
AC_MSG_CHECKING([for sysctl disk input])
AC_MSG_RESULT($withval)
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_FUNCS([sysctl],[
		 AC_CHECK_HEADERS([sys/disk.h],[
		SYSCTL_DISK=0
		AC_CHECK_TYPES([struct diskstats],[
							DISK_INPUT_FOUND=1
							if [ test $SYSCTL_ADDED -eq 0 ]; then
								INPUT_SYSTEM="$INPUT_SYSTEM sysctl.$OBJEXT"
							fi
							AC_DEFINE_UNQUOTED([HAVE_STRUCT_DISKSTATS],[1],[sysctl diskinput available])
							AC_CHECK_MEMBERS([struct diskstats.ds_rbytes],,,[#include <sys/disk.h>])
							AC_CHECK_MEMBERS([struct diskstats.ds_name],,,[#include <sys/disk.h>])
							 ],[
								AC_CHECK_TYPES([struct disk_sysctl],[
									 DISK_INPUT_FOUND=1
									 if [ test $SYSCTL_ADDED -eq 0 ]; then
										 INPUT_SYSTEM="$INPUT_SYSTEM sysctl.$OBJEXT"
									 fi
									 AC_DEFINE_UNQUOTED([HAVE_STRUCT_DISK_SYSCTL],[1],[sysctl diskinput available])
									 AC_CHECK_MEMBERS([struct disk_sysctl.dk_rbytes],,,[#include <sys/disk.h>])
									 ],[ CPPFLAGS=$OLD_CPPFLAGS ],[#include <sys/disk.h>])
								],[#include <sys/disk.h>])
		],[ CPPFLAGS=$OLD_CPPFLAGS ])
						  
    ],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi
],[
AC_CHECK_FUNCS([sysctl],[
	 AC_CHECK_HEADERS([sys/disk.h],[
	AC_CHECK_TYPES([struct diskstats],[
						DISK_INPUT_FOUND=1
						if [ test $SYSCTL_ADDED -eq 0 ]; then
							INPUT_SYSTEM="$INPUT_SYSTEM sysctl.$OBJEXT"
						fi
						AC_DEFINE_UNQUOTED([HAVE_STRUCT_DISKSTATS],[1],[sysctl diskinput available])
						AC_CHECK_MEMBERS([struct diskstats.ds_rbytes],,,[#include <sys/disk.h>])
						AC_CHECK_MEMBERS([struct diskstats.ds_name],,,[#include <sys/disk.h>])
						 ],[
							AC_CHECK_TYPES([struct disk_sysctl],[
								 DISK_INPUT_FOUND=1
								 if [ test $SYSCTL_ADDED -eq 0 ]; then
									 INPUT_SYSTEM="$INPUT_SYSTEM sysctl.$OBJEXT"
								 fi
								 AC_DEFINE_UNQUOTED([HAVE_STRUCT_DISK_SYSCTL],[1],[sysctl diskinput available])
								 AC_CHECK_MEMBERS([struct disk_sysctl.dk_rbytes],,,[#include <sys/disk.h>])
							],,[#include <sys/disk.h>])
	],[#include <sys/disk.h>])
	])
])
])

AC_ARG_WITH([kstat],AS_HELP_STRING([--with-kstat],[enable kstat (Solaris) input (ARG=PATH)]),[
AC_MSG_CHECKING([for kstat input])
AC_MSG_RESULT($withval)
if [ test $withval != "no" ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
    fi
    AC_CHECK_LIB([kstat],[kstat_open],[
      AC_DEFINE_UNQUOTED([HAVE_LIBKSTAT],[1],[Define to 1 if you have the 'kstat' library (-lkstat).])
      LIBS="$LIBS -lkstat"
		DISK_INPUT_FOUND=1	
		INPUT_SYSTEM="$INPUT_SYSTEM libkstat.$OBJEXT"
		INPUT_FOUND=1
	],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi
],[
AC_CHECK_LIB([kstat],[kstat_open],[
   AC_DEFINE_UNQUOTED([HAVE_LIBKSTAT],[1],[Define to 1 if you have the 'kstat' library (-lkstat).])
	LIBS="$LIBS -lkstat"
	INPUT_SYSTEM="$INPUT_SYSTEM libkstat.$OBJEXT"
	INPUT_FOUND=1
	DISK_INPUT_FOUND=1
])
])


AC_ARG_WITH([win32],AS_HELP_STRING([--with-win32],[enable win32 input]),[
echo "enable win32 support"
AC_DEFINE_UNQUOTED([WIN32],[1],[enable win32 support])
LIBS="$LIBS -liphlpapi"
INPUT_SYSTEM="$INPUT_SYSTEM win32.$OBJEXT"
INPUT_FOUND=1
])


AC_ARG_WITH([procnetdev],AS_HELP_STRING([--with-procnetdev],[enable /proc/net/dev input (ARG=PATH)]),[
AC_MSG_CHECKING([for /proc/net/dev input])
AC_MSG_RESULT($withval)
if [ test $withval = "yes" ]; then
    AC_DEFINE_UNQUOTED([HAVE__PROC_NET_DEV],[1],[enable /proc/net/dev support])
	 INPUT_SYSTEM="$INPUT_SYSTEM proc_net_dev.$OBJEXT"
	 INPUT_FOUND=1
else
    if [ test $withval != "no" ]; then
        AC_CHECK_FILES([$withval],[
			AC_DEFINE_UNQUOTED([PROC_NET_DEV_PATH],["$withval"],[set path to /proc/net/dev raw-file])
			AC_DEFINE_UNQUOTED([HAVE__PROC_NET_DEV],[1],[enable /proc/net/dev support])
			INPUT_SYSTEM="$INPUT_SYSTEM proc_net_dev.$OBJEXT"
			INPUT_FOUND=1
        ])
    fi
fi
],[
if test "$cross_compiling" != yes; then
	AC_CHECK_FILES([/proc/net/dev],[
		INPUT_FOUND=1
		INPUT_SYSTEM="$INPUT_SYSTEM proc_net_dev.$OBJEXT"
	])
fi
])

AC_ARG_WITH([diskstats],AS_HELP_STRING([--with-diskstats],[enable /proc/diskstats input (diskio) (ARG=PATH)]),[
AC_MSG_CHECKING([for /proc/diskstats input])
AC_MSG_RESULT($withval)
if [ test $withval = "yes" ]; then
    AC_DEFINE_UNQUOTED([HAVE__PROC_DISKSTATS],[1],[enable /proc/diskstats|/proc/partitions support])
    DISK_INPUT_FOUND=1
	 INPUT_SYSTEM="$INPUT_SYSTEM proc_diskstats.$OBJEXT"
else
    if [ test $withval != "no" ]; then
			AC_DEFINE_UNQUOTED([PROC_DISKSTATS_PATH],["$withval"],[set path to /proc/disktstats raw-file])
	      AC_DEFINE_UNQUOTED([HAVE__PROC_DISKSTATS],[1],[enable /proc/diskstats|/proc/partitions support])
         DISK_INPUT_FOUND=1
			INPUT_SYSTEM="$INPUT_SYSTEM proc_diskstats.$OBJEXT"
    fi
fi
]) # check for diskstats on ac_arg_with partitions down below

AC_ARG_WITH([partitions],AS_HELP_STRING([--with-partitions],[enable /proc/partitions input (diskio) (ARG=PATH)]),[
AC_MSG_CHECKING([for /proc/diskstats|/proc/partitions input])
AC_MSG_RESULT($withval)
if [ test $withval = "yes" ]; then
    AC_DEFINE_UNQUOTED([HAVE__PROC_DISKSTATS],[1],[enable /proc/diskstats|/proc/partitions support])
    DISK_INPUT_FOUND=1
	 INPUT_SYSTEM="$INPUT_SYSTEM proc_diskstats.$OBJEXT"
else
    if [ test $withval != "no" ]; then
		AC_DEFINE_UNQUOTED([PROC_PARTITIONS_PATH],["$withval"],[set path to /proc/partitions raw-file])
      AC_DEFINE_UNQUOTED([HAVE__PROC_DISKSTATS],[1],[enable /proc/diskstats|/proc/partitions support])
      DISK_INPUT_FOUND=1
		INPUT_SYSTEM="$INPUT_SYSTEM proc_diskstats.$OBJEXT"
    fi
fi
],[
if test "$cross_compiling" != yes; then	
	AC_CHECK_FILES([/proc/diskstats],[
						 DISK_INPUT_FOUND=1
						 INPUT_SYSTEM="$INPUT_SYSTEM proc_diskstats.$OBJEXT"
						 ],[
	   AC_CHECK_FILES([/proc/partitions],[
		   AC_DEFINE_UNQUOTED([HAVE__PROC_DISKSTATS],[1],[enable /proc/diskstats|/proc/partitions support])
			DISK_INPUT_FOUND=1
			INPUT_SYSTEM="$INPUT_SYSTEM proc_diskstats.$OBJEXT"
		])
	])
fi
])

AC_ARG_WITH([devstat],AS_HELP_STRING([--with-devstat],[enable devstat input (ie FreeBSD) (ARG=PATH)]),[
AC_MSG_CHECKING([for devstat input])
AC_MSG_RESULT($withval)
if [ test $withval != "no" ]; then
	OLD_CPPFLAGS=$CPPFLAGS
   if [ test $withval != "yes" ]; then
        CPPFLAGS="$CPPFLAGS -I$withval"
   fi
	AC_CHECK_HEADERS([sys/dkstat.h],[
		AC_CHECK_HEADERS([devstat.h],[
			AC_CHECK_LIB([devstat],[devstat_getdevs],[
					DISK_INPUT_FOUND=1
					AC_DEFINE_UNQUOTED([HAVE_DEVSTAT_GETDEVS],[1],[Define to 1 if you have the 'devstat' library (-ldevstat).])
					LIBS="$LIBS -ldevstat"
					INPUT_SYSTEM="$INPUT_SYSTEM devstat.$OBJEXT"
				],[
					AC_CHECK_LIB([devstat],[getdevs],[
							DISK_INPUT_FOUND=1
							AC_DEFINE_UNQUOTED([HAVE_GETDEVS],[1],[Define to 1 if you have the 'devstat' library (-ldevstat).])
							LIBS="$LIBS -ldevstat"
							INPUT_SYSTEM="$INPUT_SYSTEM devstat.$OBJEXT"
					])
			])
		],,[#include <sys/dkstat.h>])
	])
fi
],[
AC_CHECK_HEADERS([sys/dkstat.h],[
	AC_CHECK_HEADERS([devstat.h],[
		AC_CHECK_LIB([devstat],[devstat_getdevs],[
				DISK_INPUT_FOUND=1
				AC_DEFINE_UNQUOTED([HAVE_DEVSTAT_GETDEVS],[1],[Define to 1 if you have the 'devstat' library (-ldevstat).])
				LIBS="$LIBS -ldevstat"
				INPUT_SYSTEM="$INPUT_SYSTEM devstat.$OBJEXT"
			],[
				AC_CHECK_LIB([devstat],[getdevs],[
						DISK_INPUT_FOUND=1
						AC_DEFINE_UNQUOTED([HAVE_GETDEVS],[1],[Define to 1 if you have the 'devstat' library (-ldevstat).])
						LIBS="$LIBS -ldevstat"
						INPUT_SYSTEM="$INPUT_SYSTEM devstat.$OBJEXT"
				])
		])
	],,[#include <sys/dkstat.h>])
])
])

AC_ARG_WITH([ioservice],AS_HELP_STRING([--with-ioservice],[Use darwin IOService  to gather statistics]),[
AC_MSG_CHECKING([for ioservice input])
AC_MSG_RESULT($withval)
if [ test $withval != "no" ]; then
	OLD_CPPFLAGS=$CPPFLAGS
	if [ test $withval != "yes" ]; then
		CPPFLAGS="$CPPFLAGS -I$withval"
	fi
	OLDLIBS=$LIBS
	LIBS="$LIBS -framework IOKit -framework CoreFoundation"
	HAVE_IOSERVICE=0
   AC_CHECK_FUNCS([IOServiceGetMatchingServices],[
      AC_CHECK_FUNCS([IOMasterPort],[
         AC_CHECK_FUNCS([IORegistryEntryCreateCFProperties],[
            DISK_INPUT_FOUND=1
            AC_DEFINE_UNQUOTED([HAVE_IOSERVICE],[1],[Define to 1 if you have Darwin IOService support])
				HAVE_IOSERVICE=1
				INPUT_SYSTEM="$INPUT_SYSTEM ioservice.$OBJEXT"
         ])
      ])
   ],,[
       #include <CoreFoundation/CoreFoundation.h>
       #include <IOKit/IOKitLib.h>
       #include <IOKit/storage/IOBlockStorageDriver.h>
   ])
	if [ test $HAVE_IOSERVICE -eq 0 ]; then
		LIBS=$OLDLIBS
		CPPFLAGS=$OLD_CPPFLAGS
	fi
fi
],[
dnl evil hack to check for -framework gcc option, i love apple!!!!111
AC_MSG_CHECKING([whether cc and linker accepts -framework IOKit -framework CoreFoundation])
echo 'int main(){}' > conftest.c
if test -z "`${CC-cc} -o conftest conftest.c -framework IOKit -framework CoreFoundation 2>&1`"; then
	AC_MSG_RESULT(yes)
	OLDLIBS=$LIBS
	LIBS="$LIBS -framework IOKit -framework CoreFoundation"
	AC_CHECK_FUNCS([IOServiceGetMatchingServices],[
		AC_CHECK_FUNCS([IOMasterPort],[
			AC_CHECK_FUNCS([IORegistryEntryCreateCFProperties],[
				DISK_INPUT_FOUND=1
				AC_DEFINE_UNQUOTED([HAVE_IOSERVICE],[1],[Define to 1 if you have Darwin IOService support])
				HAVE_IOSERVICE=1
				INPUT_SYSTEM="$INPUT_SYSTEM ioservice.$OBJEXT"
			])
		])
	])
	if [ test $HAVE_IOSERVICE -eq 0 ]; then
		LIBS=$OLDLIBS
	fi
else
	AC_MSG_RESULT(no)
fi
rm -f conftest*
])

AC_ARG_WITH([libstatgrab],AS_HELP_STRING([--without-libstatgrab],[Use libstatgrab to gather statistics]))

CHECK_LIBSTAT=0
if [ test "x$with_libstatgrab" != "xno" ] && [ test $INPUT_FOUND -ne 1 ]; then
	CHECK_LIBSTAT=1
fi
if [ test "x$with_libstatgrab" != "xno" ] && [ test $DISK_INPUT_FOUND -ne 1 ]; then
    CHECK_LIBSTAT=1
fi

if [test "x$with_libstatgrab" = "xyes"] || [ test $CHECK_LIBSTAT -eq 1 ]; then
    OLD_CPPFLAGS=$CPPFLAGS
    AC_CHECK_LIB([statgrab],[sg_get_network_io_stats,sg_get_disk_io_stats],[
            INPUT_FOUND=1
            DISK_INPUT_FOUND=1
            AC_DEFINE_UNQUOTED([HAVE_LIBSTATGRAB],[1],[Define to 1 if you have the 'statgrab' library (-lstatgrab).])
            LIBS="$LIBS -lstatgrab"
            INPUT_SYSTEM="$INPUT_SYSTEM libstatgrab.$OBJEXT"
        ],[ CPPFLAGS=$OLD_CPPFLAGS ])
fi

if [ test $INPUT_FOUND -ne 1 ] && [ test $DISK_INPUT_FOUND -ne 1 ]; then
	AC_MSG_ERROR(["NO INPUT FOUND"]);
fi

#################################################
# inputs check done
#################################################

#check for getifaddrs
linux_if_h_includes="
            #if HAVE_SYS_SOCKET_H
            # include <sys/socket.h>
            #endif
            "

AC_CHECK_FUNCS([ioctl])
AC_CHECK_HEADERS([sys/socket.h linux/if.h],,,[$linux_if_h_includes])
AC_CHECK_HEADERS([sys/tty.h])
AC_CHECK_HEADERS([linux/tty.h])
AC_CHECK_HEADERS([stropts.h])


AC_ARG_ENABLE([html],AS_HELP_STRING([--enable-html],[enable html output (default: yes)]),[
echo "enable html output: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED([HTML],[1],[enable html output])
fi
],[
echo "enable html output: yes"
AC_DEFINE_UNQUOTED([HTML],[1],[enable html output])
])

AC_ARG_ENABLE([csv],AS_HELP_STRING([--enable-csv],[enable csv output (default: yes)]),[
echo "enable csv output: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED([CSV],[1],[enable CSV output])
fi
],[
echo "enable csv output: yes"
AC_DEFINE_UNQUOTED([CSV],[1],[enable CSV output])
])

AC_ARG_ENABLE([extendedstats],AS_HELP_STRING([--enable-extendedstats],[enable max, sum and avg stats (default: yes)]),[
echo "enable extended stats: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED([EXTENDED_STATS],[1],[enable max, sum and avg stats])
fi
],[
echo "enable extended stats: yes"
AC_DEFINE_UNQUOTED([EXTENDED_STATS],[1],[enable max, sum and avg stats])
])

AC_ARG_ENABLE([configfile],AS_HELP_STRING([--enable-configfile],[enable configfile support (default: yes)]),[
echo "enable config-file support: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED([CONFIG_FILE],[1],[enable use of configfile])
fi
],[
echo "enable config-file support: yes"
AC_DEFINE_UNQUOTED([CONFIG_FILE],[1],[enable use of configfile])
])

AC_ARG_ENABLE([64bit],AS_HELP_STRING([--enable-64bit],[enable 64bit support (default: no)]),[
echo "enable 64bit support: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED([SYS_64BIT],[1],[enable 64bit support])
fi
])

AC_CHECK_HEADERS([pwd.h],[
    AC_CHECK_FUNCS([getpwuid],[
        AC_DEFINE_UNQUOTED([HAVE_GETPWUID],[1],[enable use of getbwuid])
    ])
])

NETSTAT_ADDED=0

AC_ARG_ENABLE([netstatpath],AS_HELP_STRING([--enable-netstatpath],[enable path to netstat as runtime option (default: no), WARNING THIS IS A SECURITY FLAW!]),[
echo "enable path to netstat as option: $enableval"
if [ test $enableval = "yes" ]; then
    AC_DEFINE_UNQUOTED([ALLOW_NETSTATPATH],[1],[allow set path to netstat at runtime])
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
        AC_DEFINE_UNQUOTED([HAVE_NETSTAT_PATH],[$withval],[set path to netstat])
    else
        echo "bsd like version with byte info forced"
    fi
    if [ test $NETSTAT_BYTE != "1" ]; then
        AC_DEFINE_UNQUOTED([NETSTAT_BSD],[1],[use bsd like netstat])
		NETSTAT_DEFINED="1"
		 if [ test $NETSTAT_ADDED -eq 0 ]; then
			 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
			 NETSTAT_ADDED=1
		 fi
    else
        AC_DEFINE_UNQUOTED([NETSTAT_BSD_BYTES],[1],[use bsd like netstat including bytes info])
		NETSTAT_DEFINED="1"
		  if [ test $NETSTAT_ADDED -eq 0 ]; then
			 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
			 NETSTAT_ADDED=1
		  fi 
    fi
    if [ test $NETSTAT_LINK != "0" ]; then
        AC_DEFINE_UNQUOTED([NETSTAT_BSD_LINK],[1],[use bsd like netstat with link switch])
			NETSTAT_DEFINED="1"
		 if [ test $NETSTAT_ADDED -eq 0 ]; then
			 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
			 NETSTAT_ADDED=1
		 fi
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
        AC_DEFINE_UNQUOTED([HAVE_NETSTAT_PATH],[$withval],[set path to netstat])
    else
        echo "GNU net-tools like version forced"
    fi
    AC_DEFINE_UNQUOTED([NETSTAT_LINUX],[1],[use GNU net-tools netstat])
    if [ test $NETSTAT_ADDED -eq 0 ]; then
       INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
       NETSTAT_ADDED=1
 	 fi
    NETSTAT_DEFINED="1"
else
    NETSTAT_DEFINED="1"
    echo "disable netstat: GNU net-tools like version"
fi
])

AC_ARG_WITH([netstatlinuxnew],AS_HELP_STRING([--with-netstatlinuxnew],[enable linux newer versions ie 2016+ like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "GNU net-tools like version forced: $withval"
        AC_DEFINE_UNQUOTED([HAVE_NETSTAT_PATH],[$withval],[set path to netstat])
    else
        echo "GNU net-tools like version forced"
    fi
    AC_DEFINE_UNQUOTED([NETSTAT_LINUX_NEW],[1],[use GNU net-tools netstat new versions])
    if [ test $NETSTAT_ADDED -eq 0 ]; then
       INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
       NETSTAT_ADDED=1
 	 fi
    NETSTAT_DEFINED="1"
else
    NETSTAT_DEFINED="1"
    echo "disable netstat: GNU net-tools like new versions"
fi
])


AC_ARG_WITH([netstatsolaris],AS_HELP_STRING([--with-netstatsolaris],[enable solaris like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "solaris like version forced: $withval"
        AC_DEFINE_UNQUOTED([HAVE_NETSTAT_PATH],[$withval],[set oath to netstat])
    else
        echo "solaris like version forced"
    fi
    AC_DEFINE_UNQUOTED([NETSTAT_SOLARIS],[1],[use Solaris netstat theme])
    NETSTAT_DEFINED="1"
	 if [ test $NETSTAT_ADDED -eq 0 ]; then
		 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
		 NETSTAT_ADDED=1
	 fi

else
    NETSTAT_DEFINED="1"
    echo "disable netstat: solaris like version"
fi
])

AC_ARG_WITH([netstatnetbsd],AS_HELP_STRING([--with-netstatnetbsd],[enable NetBSD/OpenBSD like netstat -i (ARG=PATH)]),[
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        echo "NetBSD/OpenBSD like version forced: $withval"
        AC_DEFINE_UNQUOTED([HAVE_NETSTAT_PATH],[$withval],[set path to netstat])
    else
        echo "NetBSD/OpenBSD like version forced"
    fi
    AC_DEFINE_UNQUOTED([NETSTAT_NETBSD],[1],[use NetBSD/OpenBSD like netstat])
    NETSTAT_DEFINED="1"
	 if [ test $NETSTAT_ADDED -eq 0 ]; then
		 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
		 NETSTAT_ADDED=1
	 fi

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
        AC_MSG_CHECKING([for netstat version])
        net_test=`${NETSTAT_CMD} -ia 2>/dev/null | ${EGREP} "Iface +MTU +RX-OK +RX-ERR +RX-DRP +RX-OVR +TX-OK +TX-ERR +TX-DRP +TX-OVR +Flg"`
        if [ test "${net_test}" ]; then
        		AC_MSG_RESULT([GNU net-tools like newer version found])
				AC_DEFINE_UNQUOTED([NETSTAT_LINUX_NEW],[1],[use GNU net-tools netstat newer versions])
			 	if [ test $NETSTAT_ADDED -eq 0 ]; then
					 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
					 NETSTAT_ADDED=1
			 	fi
        else
        net_test=`${NETSTAT_CMD} -ia 2>/dev/null | ${EGREP} "Iface +MTU +Met +RX-OK +RX-ERR +RX-DRP +RX-OVR +TX-OK +TX-ERR +TX-DRP +TX-OVR +Flg"`
        if [ test "${net_test}" ]; then
        		AC_MSG_RESULT([GNU net-tools like version found])
				AC_DEFINE_UNQUOTED([NETSTAT_LINUX],[1],[use GNU net-tools netstat])
			 	if [ test $NETSTAT_ADDED -eq 0 ]; then
					 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
					 NETSTAT_ADDED=1
			 	fi
        else
            net_test=`${NETSTAT_CMD} -iW 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ipkts +Ierrs +Opkts +Oerrs +Coll"`
            if [ test "${net_test}" ]; then
				net_test2=`${NETSTAT_CMD} -iW -b 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ipkts +Ierrs +Ibytes +Opkts +Oerrs +Obytes +Coll"`
				if [ test "${net_test2}" ]; then
					AC_MSG_RESULT([BSD like version with byte info found])
					AC_DEFINE_UNQUOTED([NETSTAT_BSD_BYTES],[1],[bsd like version with byte info])
				   if [ test $NETSTAT_ADDED -eq 0 ]; then
				  	 	INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
					 	NETSTAT_ADDED=1
				 	fi
				else
	             AC_MSG_RESULT([BSD like version found])
					 AC_DEFINE_UNQUOTED([NETSTAT_BSD],[1],[BSD like netstat])
					 if [ test $NETSTAT_ADDED -eq 0 ]; then
						 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
						 NETSTAT_ADDED=1
					 fi
				fi
				AC_MSG_CHECKING([for netstat -f link])
				net_test3=`${NETSTAT_CMD} -iW -f link 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ipkts +Ierrs +Opkts +Oerrs +Coll"`
				if [ test "${net_test3}" ]; then
					AC_MSG_RESULT([yes])
					AC_DEFINE_UNQUOTED([NETSTAT_BSD_LINK],[1],[enable link info for netstat])
					 if [ test $NETSTAT_ADDED -eq 0 ]; then
						 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
						 NETSTAT_ADDED=1
					 fi
				else
					AC_MSG_RESULT([no])
				fi
			else
                net_test=`${NETSTAT_CMD} -i -f inet -f inet6 2>/dev/null | ${EGREP} "Name +Mtu +Net/Dest +Address +Ipkts +Ierrs +Opkts +Oerrs +Collis +Queue"`
                if [ test "${net_test}" ]; then
                    AC_MSG_RESULT([solaris version found])
                    AC_DEFINE_UNQUOTED([NETSTAT_SOLARIS],[1],[use Solaris netstat])
						 if [ test $NETSTAT_ADDED -eq 0 ]; then
							 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
							 NETSTAT_ADDED=1
						 fi
                else 
                    net_test=`${NETSTAT_CMD} -ibd 2>/dev/null | ${EGREP} "Name +Mtu +Network +Address +Ibytes +Obytes +Drop"`
                    if [ test "${net_test}" ]; then
                        AC_MSG_RESULT([Net/OpenBSD version found])
                        AC_DEFINE_UNQUOTED([NETSTAT_NETBSD],[1],[Use Net/OpenBSD netstat])
								 if [ test $NETSTAT_ADDED -eq 0 ]; then
									 INPUT_SYSTEM="$INPUT_SYSTEM netstat.$OBJEXT"
									 NETSTAT_ADDED=1
								 fi
                    else
        				AC_MSG_RESULT([unknown netstat version - disabled])
echo "*********************************************
  please mail 'netstat -i' output to author
*********************************************"
                    fi
                fi
            fi
          fi
        fi
    fi
fi
fi

AC_QEF_C_NORETURN
AC_C_INLINE

if [ test -z ${DEBUG} ]; then
AC_ARG_WITH([strip],AS_HELP_STRING([--with-strip],[use strip after make (ARG=PATH)]),[
echo "enable strip... "
AC_MSG_RESULT($enableval)
if [ test $withval != "no" ]; then
    if [ test $withval != "yes" ]; then
        STRIP="${withval} \${TARGET}"
    else
		 if test "$cross_compiling" != yes; then
	        AC_CHECK_FILES([/usr/ccs/bin/strip],[
		         AC_CHECK_PROGS([STRIP],[strip],,,[/usr/ccs/bin])
			  ])
		 fi
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
if test "$cross_compiling" != yes; then
	AC_CHECK_FILES([/usr/ccs/bin/strip],[
		 AC_CHECK_PROGS([STRIP],[strip],,,[/usr/ccs/bin])
	])
fi
if [ test "${STRIP}" ]; then
    STRIP="/usr/ccs/bin/${STRIP} \${TARGET}"
else
AC_CHECK_PROGS([STRIP],[strip])
if [ test "${STRIP}" ]; then
    STRIP="${STRIP} \${TARGET}"
fi
fi
])
else
    STRIP="@echo DEBUG mode enabled."
fi

#AC_CHECK_CC_OPT(Wno-long-double)
#AC_CHECK_CC_OPT(Wno-long-long)

AC_SUBST([INPUT_SYSTEM])

AC_CONFIG_FILES([
    Makefile
    src/Makefile
])

AC_OUTPUT
