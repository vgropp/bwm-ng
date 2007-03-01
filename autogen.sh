#! /bin/sh

# Copyright (c) 2002  Daniel Elstner  <daniel.elstner@gmx.net>
#   * minor changes by Daniel Holbach <dh@mailempfang.de>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License VERSION 2 as
# published by the Free Software Foundation.  You are not allowed to
# use any other version of the license; unless you got the explicit
# permission from the author to do so.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


# Be Bourne compatible. (stolen from autoconf)
if test -n "${ZSH_VERSION+set}" && (emulate sh) >/dev/null 2>&1; then
  emulate sh
  NULLCMD=:
  # Zsh 3.x and 4.x performs word splitting on ${1+"$@"}, which
  # is contrary to our usage.  Disable this feature.
  alias -g '${1+"$@"}'='"$@"'
elif test -n "${BASH_VERSION+set}" && (set -o posix) >/dev/null 2>&1; then
  set -o posix
fi

PROJECT=bwm-ng
autoconf=autoconf
autoheader=autoheader
aclocal=
automake=
auto_version=0


srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

origdir=`pwd`
cd "$srcdir"

AUTOMAKE_FLAGS="--add-missing --gnu $AUTOMAKE_FLAGS"

if test -z "$AUTOGEN_SUBDIR_MODE" && test "x$*" = x
then
  echo "I am going to run ./configure with no arguments - if you wish "
  echo "to pass any to it, please specify them on the $0 command line."
fi

if test -n "$AUTOCONF_VERSION"
then
   echo "test for autoconf version"
   for suffix in "" -2.62 -2.61 -2.60 -2.59 -2.58 -2.57 -2.56 -2.55 -2.54 -2.53 -2.52 -2.51
   do
      autoconf_version=`autoconf$suffix --version </dev/null 2>/dev/null | head -n1 | cut -d " " -f 4`
      autoheader_version=`autoheader$suffix --version </dev/null 2>/dev/null | head -n1 | cut -d " " -f 4`
      version=`echo "$autoconf_version" | cut -c-3 | tr -d .`
      if test -n "$autoconf_version" && test -n "$autoheader_version" && test "$autoconf_version" = "$autoheader_version" && \
         test -n "$version" && \
         test -z "`echo -n $version | sed -n s/[0-9]*//p`"
      then
         autoconf=autoconf$suffix
         autoheader=autoheader$suffix
         if test -n "$suffix"
         then
            echo "found version `echo $suffix | cut -c2-`"
            export AUTOCONF_VERSION=`echo $suffix | cut -c2-`
         fi
         break
      fi
   done
fi

if test -n "$AUTOMAKE_VERSION"
then
	echo "test for automake version"
	for suffix in "" -1.9 -1.8 -1.7 
	do
	  aclocal_version=`aclocal$suffix --version </dev/null 2>/dev/null | head -n1 | cut -d " " -f 4`
	  automake_version=`automake$suffix --version </dev/null 2>/dev/null | head -n1 | cut -d " " -f 4`
	  if test -n "$aclocal_version" && test -n "$automake_version" && test "$aclocal_version" = "$automake_version" && \
		  test -n `echo "$aclocal_version" | cut -c-3 | tr -d .` 
	  then
		  auto_version=`echo "$aclocal_version" | cut -c-3 | tr -d .`
		  if test -z "`echo -n $auto_version | sed -n s/[0-9]*//p`" && test $auto_version -ge 16
		  then
			  aclocal=aclocal$suffix
			  automake=automake$suffix
			  if test -n "$suffix"
			  then
				  echo "found version `echo $suffix | cut -c2-`"
				  export AUTOMAKE_VERSION=`echo $suffix | cut -c2-`
			  fi
			  break
		  fi
	  fi
	done
else
	aclocal=aclocal
	automake=automake
fi

if test -z "$aclocal" || test -z "$automake"
then
	echo "*******************************"
	echo "AT LEAST automake 1.7 REQUIRED!"
	echo "*******************************"
	exit 1
fi

set_option=':'
test -n "${BASH_VERSION+set}" && set_option='set'

$set_option -x

echo trying autoreconf to setup build enviroment
autoreconf -v -i -s -W all 
if test $? -eq 0 
then
   echo running configure now
	if test -z "$AUTOGEN_SUBDIR_MODE"
	then
	  "$srcdir/configure" ${1+"$@"} || exit 1
	  $set_option +x
	  echo
	  echo "Now type 'make' to compile $PROJECT."
	fi	
   exit 0
fi

echo looks like there is no autoreconf, trying to setup all now


rm -f config.guess config.sub depcomp install-sh missing mkinstalldirs
rm -f config.cache acconfig.h
rm -rf autom4te.cache

WARNINGS=all
export WARNINGS

"$aclocal" $ACLOCAL_FLAGS   || exit 1
"$autoheader"           || exit 1
"$automake" $AUTOMAKE_FLAGS || exit 1
"$autoconf"         || exit 1
cd "$origdir"           || exit 1

if test -z "$AUTOGEN_SUBDIR_MODE"
then
  "$srcdir/configure" ${1+"$@"} || exit 1
  $set_option +x
  echo
  echo "Now type 'make' to compile $PROJECT."
fi

exit 0
