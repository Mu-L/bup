# -*-shell-script-*-

#   @(#) configure.inc 1.40@(#)
#   Copyright (c) 1999-2007 David Parsons. All rights reserved.
#   
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions
#   are met:
#  1. Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#  2. Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in
#     the documentation and/or other materials provided with the
#     distribution.
#  3. My name may not be used to endorse or promote products derived
#     from this software without specific prior written permission.
#     
#  THIS SOFTWARE IS PROVIDED BY DAVID PARSONS ``AS IS'' AND ANY
#  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#  PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL DAVID
#  PARSONS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
#  TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
#  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
#  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
#  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
#  IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
#  THE POSSIBILITY OF SUCH DAMAGE.
#


#
# this preamble code is executed when this file is sourced and it picks
# interesting things off the command line.
#
ac_default_path="/sbin:/usr/sbin:/bin:/usr/bin:/usr/local/bin:/usr/X11R6/bin"

ac_standard="--src=DIR		where the source lives (.)
--prefix=DIR		where to install the final product (/usr/local)
--execdir=DIR		where to put executables (prefix/bin)
--sbindir=DIR		where to put static executables (prefix/sbin)
--confdir=DIR		where to put configuration information (/etc)
--libdir=DIR		where to put libraries (prefix/lib)
--libexecdir=DIR	where to put private executables
--mandir=DIR		where to put manpages"

__fail=exit


__ac_tmpdir=''
__ac_clean_up () {
    if test "$__ac_tmpdir"; then
        rm -rf "$__ac_tmpdir"
    fi
}
trap __ac_clean_up EXIT


if dirname B/A 2>/dev/null >/dev/null; then
__ac_dirname() {
    dirname "$1"
}
else
__ac_dirname() {
    echo "$1" | sed -e 's:/[^/]*$::'
}
fi

ac_progname=$0
ac_configure_command=
Q=\'
for x in "$@"; do
    ac_configure_command="$ac_configure_command $Q$x$Q"
done
# ac_configure_command="$*"

__d=`__ac_dirname "$ac_progname"`
if [ "$__d" = "$ac_progname" ]; then
    AC_SRCDIR=`pwd`
else
    AC_SRCDIR=`cd $__d;pwd`
fi

__ac_dir() {
    if test -d "$1"; then
	(cd "$1";pwd)
    else
	echo "$1";
    fi
}

while [ $# -gt 0 ]; do
    unset matched

    case X"$1" in
    X--src|X--srcdir)
	AC_SRCDIR=`__ac_dir "$2"`
	_set_srcdir=1
	shift 2;;

    X--src=*|X--srcdir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_SRCDIR=`__ac_dir "$__d"`
	_set_srcdir=1
	shift 1 ;;

    X--prefix)
	AC_PREFIX=`__ac_dir "$2"`
	_set_prefix=1
	shift 2;;

    X--prefix=*)
	__d=`echo "$1"| sed -e 's/^[^=]*=//'`
	AC_PREFIX=`__ac_dir "$__d"`
	_set_prefix=1
	shift 1;;

    X--confdir)
	AC_CONFDIR=`__ac_dir "$2"`
	_set_confdir=1
	shift 2;;

    X--confdir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_CONFDIR=`__ac_dir "$__d"`
	_set_confdir=1
	shift 1;;

    X--libexec|X--libexecdir)
	AC_LIBEXEC=`__ac_dir "$2"`
	_set_libexec=1
	shift 2;;

    X--libexec=*|X--libexecdir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_LIBEXEC=`__ac_dir "$__d"`
	_set_libexec=1
	shift 1;;

    X--lib|X--libdir)
	AC_LIBDIR=`__ac_dir "$2"`
	_set_libdir=1
	shift 2;;

    X--lib=*|X--libdir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_LIBDIR=`__ac_dir "$__d"`
	_set_libdir=1
	shift 1;;

    X--exec|X--execdir)
	AC_EXECDIR=`__ac_dir "$2"`
	_set_execdir=1
	shift 2;;

    X--exec=*|X--execdir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_EXECDIR=`__ac_dir "$__d"`
	_set_execdir=1
	shift 1;;

    X--sbin|X--sbindir)
	AC_SBINDIR=`__ac_dir "$2"`
	_set_sbindir=1
	shift 2;;

    X--sbin=*|X--sbindir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_SBINDIR=`__ac_dir "$__d"`
	_set_sbindir=1
	shift 1;;

    X--man|X--mandir)
	AC_MANDIR=`__ac_dir "$2"`
	_set_mandir=1
	shift 2;;

    X--man=*|X--mandir=*)
	__d=`echo "$1" | sed -e 's/^[^=]*=//'`
	AC_MANDIR=`__ac_dir "$__d"`
	_set_mandir=1
	shift 1;;

    X--use-*=*)
	_var=`echo "$1"| sed -n 's/^--use-\([A-Za-z][-A-Za-z0-9_]*\)=.*$/\1/p'`
	if [ "$_var" ]; then
	    _val=`echo "$1" | sed -e 's/^--use-[^=]*=\(.*\)$/\1/'`
	    _v=`echo $_var | tr '[a-z]' '[A-Z]' | tr '-' '_'`
	    case X"$_val" in
	    X[Yy][Ee][Ss]|X[Tt][Rr][Uu][Ee]) eval USE_${_v}=T ;;
	    X[Nn][Oo]|X[Ff][Aa][Ll][Ss][Ee]) eval unset USE_${_v} ;;
	    *) echo "Bad value for --use-$_var ; must be yes or no"
	       exit 1 ;;
	    esac
	else
	    echo "Bad option $1.   Use --help to show options" 1>&2
	    exit 1
	fi
	shift 1 ;;

    X--use-*)
        _var=`echo "$1"|sed -n 's/^--use-\([A-Za-z][-A-Za-z0-9_]*\)$/\1/p'`
	_v=`echo $_var | tr '[a-z]' '[A-Z]' | tr '-' '_'`
	eval USE_${_v}=T
	shift 1;;

    X--with-*=*)
	_var=`echo "$1"| sed -n 's/^--with-\([A-Za-z][-A-Za-z0-9_]*\)=.*$/\1/p'`
	if [ "$_var" ]; then
	    _val=`echo "$1" | sed -e 's/^--with-[^=]*=\(.*\)$/\1/'`
	    _v=`echo $_var | tr '[a-z]' '[A-Z]' | tr '-' '_'`
	    eval WITH_${_v}=\"$_val\"
	else
	    echo "Bad option $1.   Use --help to show options" 1>&2
	    exit 1
	fi
	shift 1 ;;

    X--with-*)
	_var=`echo "$1" | sed -n 's/^--with-\([A-Za-z][A-Za-z0-9_-]*\)$/\1/p'`
	if [ "$_var" ]; then
	    _v=`echo $_var | tr '[a-z]' '[A-Z]' | tr '-' '_'`
	    eval WITH_${_v}=1
	else
	    echo "Bad option $1.   Use --help to show options" 1>&2
	    exit 1
	fi
	shift 1 ;;

    X--help)
	echo "$ac_standard"
	test "$ac_help" && echo "$ac_help"
	exit 0;;

    *)	if [ "$LOCAL_AC_OPTIONS" ]; then
	    eval "$LOCAL_AC_OPTIONS"
	else
	    ac_error=T
	fi
	if [ "$ac_error" ]; then
	    echo "Bad option $1.   Use --help to show options" 1>&2
	    exit 1
	fi ;;
    esac
done


#
# echo w/o newline
#
echononl()
{
    ${ac_echo:-echo} "${@}$ac_echo_nonl"
}

#
# log something to the terminal and to a logfile.
#
LOG () {
    echo "$@"
    echo "$@" 1>&5
}

#
# log something to the terminal without a newline, and to a logfile with
# a newline
#
LOGN () {
    echononl "$@" 1>&5
    echo "$@"
}

#
# log something to the terminal
#
TLOG () {
    echo "$@" 1>&5
}

#
# log something to the terminal, no newline
#
TLOGN () {
    echononl "$@" 1>&5
}


#
# AC_CONTINUE tells configure not to bomb if something fails, but to
# continue blithely along
#
AC_CONTINUE () {
    __fail="return"
}

#
# Emulate gnu autoconf's AC_CHECK_HEADERS() function
#
AC_CHECK_HEADERS () {
    AC_PROG_CC

    echo "/* AC_CHECK_HEADERS */" > "$__ac_tmpdir/ngc$$.c"
    for hdr in $*; do
	echo "#include <$hdr>" >> "$__ac_tmpdir/ngc$$.c"
    done
    echo "int main(int argc, char **argv) { return 0; }" >> "$__ac_tmpdir/ngc$$.c"

    LOGN "checking for header $hdr"

    if $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c"; then
	AC_DEFINE 'HAVE_'`echo $hdr | tr 'a-z' 'A-Z' | tr './' '_'` 1
	TLOG " (found)"
	rc=0
    else
	TLOG " (not found)"
	rc=1
    fi
    rm -f "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$"
    return $rc
}


#
# emulate GNU autoconf's AC_CHECK_FUNCS function
#
AC_CHECK_FUNCS () {
    AC_PROG_CC

    F="$1"
    shift
    rm -f "$__ac_tmpdir/ngc$$.c"
    cat >> "$__ac_tmpdir/ngc$$.c" << EOF
char $F(void);
int main(int argc, char **argv)
{
    return $F();
}
EOF

    LOGN "checking for the $F function"

    if $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" $LIBS; then
	AC_DEFINE `echo ${2:-HAVE_$F} | tr 'a-z' 'A-Z'` 1
	TLOG " (found)"
	rc=0
    else
	echo "offending command was:"
	cat "$__ac_tmpdir/ngc$$.c"
	echo "$AC_CC -o $__ac_tmpdir/ngc$$ $__ac_tmpdir/ngc$$.c $LIBS"
	TLOG " (not found)"
	rc=1
    fi
    rm -f "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$"
    return $rc
}


#
# check to see if some structure exists
#
# usage: AC_CHECK_STRUCT structure {include ...}
#
AC_CHECK_STRUCT () {
    AC_PROG_CC
    struct=$1
    shift

    rm -f "$__ac_tmpdir/ngc$$.c"

    for include in $*; do
	echo "#include <$include>" >> "$__ac_tmpdir/ngc$$.c"
    done

    cat >> "$__ac_tmpdir/ngc$$.c" << EOF
#include <stdio.h>
int main(int argc, char **argv)
{
    struct $struct foo;
    printf("%p", (void *) &foo);
    return 0;    
}
EOF

    LOGN "checking for struct $struct"

    if $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" $AC_LIBS 2>>config.log; then
	AC_DEFINE HAVE_STRUCT_`echo ${struct} | tr 'a-z' 'A-Z'`
	TLOG " (found)"
	rc=0
    else
	TLOG " (not found)"
	rc=1
    fi
    rm -f "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$"
    return $rc
}


#
# check to see if some structure contains a field
#
# usage: AC_CHECK_FIELD structure field {include ...}
#
AC_CHECK_FIELD () {
    AC_PROG_CC

    struct=$1
    field=$2
    shift 2

    rm -f "$__ac_tmpdir/ngc$$.c"

    for include in $*;do
	echo "#include <$include>" >> "$__ac_tmpdir/ngc$$.c"
    done

    cat >> "$__ac_tmpdir/ngc$$.c" << EOF
#include <stdio.h>
int main(int argc, char **argv)
{
    struct $struct foo;

    printf("%p", (void *) &(foo.$field));
    return 0;
}
EOF

    LOGN "checking that struct $struct has a $field field"

    if $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" $AC_LIBS 2>>config.log; then
	AC_DEFINE HAVE_`echo ${struct}_$field | tr 'a-z' 'A-Z'`
	TLOG " (yes)"
	rc=0
    else
	TLOG " (no)"
	rc=1
    fi
    rm -f "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$"
    return $rc
}


#
# check that the C compiler works
#
AC_PROG_CC () {
    test "$AC_CC" && return 0

    cat > "$__ac_tmpdir/ngc$$.c" << \EOF
#include <stdio.h>
int main(int argc, char **argv)
{
    puts("hello, sailor");
    return 0;
}
EOF

    TLOGN "checking the C compiler"

    unset AC_CFLAGS AC_LDFLAGS

    if [ "$CC" ] ; then
	AC_CC="$CC"
    elif [ "$WITH_PATH" ]; then
	AC_CC=`acLookFor cc`
    elif [ "`acLookFor cc`" ]; then
	# don't specify the full path if the user is looking in their $PATH
	# for a C compiler.
	AC_CC=cc
    fi

    # finally check for POSIX c89
    test "$AC_CC" || AC_CC=`acLookFor c89`

    if [ ! "$AC_CC" ]; then
	TLOG " (no C compiler found)"
	$__fail 1
    fi
    echo "checking out the C compiler"

    $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c"
    status=$?

    TLOGN " ($AC_CC)"
    if [ $status -eq 0 ]; then
	TLOG " ok"

	# check that the CFLAGS and LDFLAGS aren't bogus

	unset AC_CFLAGS AC_LDFLAGS

	if [ "$CFLAGS" ]; then
	    test "$CFLAGS" && echo "validating CFLAGS=${CFLAGS}"
	    if $AC_CC $CFLAGS -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" ; then
		AC_CFLAGS=${CFLAGS:-"-g"}
		test "$CFLAGS" && echo "CFLAGS=\"${CFLAGS}\" are okay"
	    elif [ "$CFLAGS" ]; then
		echo "ignoring bogus CFLAGS=\"${CFLAGS}\""
	    fi
	else
	    AC_CFLAGS=-g
	fi
	if [ "$LDFLAGS" ]; then
	    echo "validating LDFLAGS=${LDFLAGS}"
            $AC_CC $AC_CFLAGS -c -o "$__ac_tmpdir/ngc$$.o" "$__ac_tmpdir/ngc$$.c"
            if $AC_CC $AC_CFLAGS $LDFLAGS -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.o"; then
		AC_LDFLAGS=${LDFLAGS:-"-g"}
		test "$LDFLAGS" && TLOG "LDFLAGS=\"${LDFLAGS}\" are okay"
	    elif [ "$LDFLAGS" ]; then
		TLOG "ignoring bogus LDFLAGS=\"${LDFLAGS}\""
	    fi
	else
	    AC_LDFLAGS=${CFLAGS:-"-g"}
	fi
    else
	AC_FAIL " does not compile code properly"
    fi

    AC_SUB 'CC' "$AC_CC"

    rm -f "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$.o"

    return $status
}


#
# acLookFor actually looks for a program, without setting anything.
#
acLookFor () {
    path="${AC_PATH:-$ac_default_path}"
    case "X$1" in
    X-[rx]) __mode=$1
	    shift
	    ;;
    *)	    __mode=-x
	    ;;
    esac
    oldifs="$IFS"
    for program in $*; do
	IFS=":"
	for x in $path; do
	    if [ $__mode "$x/$program" -a -f "$x/$program" ]; then
		echo "$x/$program"
		break 2
	    fi
	done
    done
    IFS="$oldifs"
    unset __mode
}


#
# check that a program exists and set its path
#
MF_PATH_INCLUDE () {
    SYM=$1; shift

    case X$1 in
    X-[rx]) __mode=$1
	    shift
	    ;;
    *)      unset __mode
	    ;;
    esac

    TLOGN "looking for $1"

    DEST=`acLookFor $__mode $*`

    __sym=`echo "$SYM" | tr '[a-z]' '[A-Z]'`
    if [ "$DEST" ]; then
	TLOG " ($DEST)"
	echo "$1 is $DEST"
	AC_MAK $SYM
	AC_DEFINE PATH_$__sym \""$DEST"\"
	AC_SUB $__sym "$DEST"
	eval CF_$SYM="'$DEST'"
	return 0
    else
	#AC_SUB $__sym ''
	echo "$1 is not found"
	TLOG " (not found)"
	return 1
    fi
}


#
# AC_INIT starts the ball rolling
#
# After AC_INIT, fd's 1 and 2 point to config.log
# and fd 5 points to what used to be fd 1
#
AC_INIT () {
    __ac_tmpdir=$(mktemp -d configure-XXXXXX)
    __config_files="config.cmd config.sub config.h config.mak config.log"
    __config_detritus="config.h.tmp"
    rm -f $__config_files $__config_detritus
    __cwd=`pwd`
    exec 5>&1 1>$__cwd/config.log 2>&1
    AC_CONFIGURE_FOR=__AC_`echo $1 | sed -e 's/\..$//' | tr 'a-z' 'A-Z' | tr ' ' '_'`_D

    # check to see whether to use echo -n or echo ...\c
    #
    echo -n hello > $$
    echo world >> $$
    if grep "helloworld" $$ >/dev/null; then
	ac_echo="echo -n"
	echo "[echo -n] works"
    else
	ac_echo="echo"
	echo 'hello\c' > $$
	echo 'world' >> $$
	if grep "helloworld" $$ >/dev/null; then
	    ac_echo_nonl='\c'
	    echo "[echo ...\\c] works"
	fi
    fi
    rm -f $$

    LOG "Configuring for [$1]"

    rm -f $__cwd/config.h
    cat > $__cwd/config.h.tmp << EOF
/*
 * configuration for $1${2:+" ($2)"}, generated `date`
 * by ${LOGNAME:-`whoami`}@`hostname`
 */
#ifndef $AC_CONFIGURE_FOR
#define $AC_CONFIGURE_FOR 1


EOF

    unset __share
    if [ -d $AC_PREFIX/share/man ]; then
	for t in 1 2 3 4 5 6 7 8 9; do
	    if [ -d $AC_PREFIX/share/man/man$t ]; then
		__share=/share
	    elif [ -d $AC_PREFIX/share/man/cat$t ]; then
		__share=/share
	    fi
	done
    else
	__share=
    fi

    if [ -d $AC_PREFIX/libexec ]; then
	__libexec=libexec
    else
	__libexec=lib
    fi


    AC_PREFIX=${AC_PREFIX:-/usr/local}
    AC_EXECDIR=${AC_EXECDIR:-$AC_PREFIX/bin}
    AC_SBINDIR=${AC_SBINDIR:-$AC_PREFIX/sbin}
    AC_LIBDIR=${AC_LIBDIR:-$AC_PREFIX/lib}
    AC_MANDIR=${AC_MANDIR:-$AC_PREFIX$__share/man}
    AC_LIBEXEC=${AC_LIBEXEC:-$AC_PREFIX/$__libexec}
    AC_CONFDIR=${AC_CONFDIR:-/etc}

    AC_PATH=${WITH_PATH:-$PATH}
    AC_PROG_CPP
    AC_PROG_INSTALL

    ac_os=`uname -s | sed 's/[-_].*//; s/[^a-zA-Z0-9]/_/g'`
    _os=`echo $ac_os | tr '[a-z]' '[A-Z]'`
    AC_DEFINE OS_$_os	1
    eval OS_${_os}=1
    unset _os
}


#
# AC_LIBRARY checks to see if a given library exists and contains the
# given function.
# usage: AC_LIBRARY function library [alternate ...]
#
AC_LIBRARY() {
    SRC=$1
    shift

    __acllibs=
    __aclhdrs=

    for x in "$@"; do
	case X"$x" in
	X-l*) __acllibs="$__acllibs $x" ;;
	*)    __aclhdrs="$__aclhdrs $x" ;;
	esac
    done

    # first see if the function can be found in any of the
    # current libraries
    AC_QUIET AC_CHECK_FUNCS $SRC $__aclhdrs && return 0

    # then search through the list of libraries
    __libs="$LIBS"
    for x in $__acllibs; do
	LIBS="$__libs $x"
	if AC_QUIET AC_CHECK_FUNCS $SRC $__aclhdrs; then
	    AC_LIBS="$AC_LIBS $x"
	    return 0
	fi
    done
    return 1
}


#
# AC_PROG_LEX checks to see if LEX exists, and if it's lex or flex.
#
AC_PROG_LEX() {
    TLOGN "looking for lex "

    DEST=`acLookFor lex`
    if [ "$DEST" ]; then
	AC_MAK LEX
	AC_DEFINE PATH_LEX \"$DEST\"
	AC_SUB 'LEX' "$DEST"
	echo "lex is $DEST"
    else
	DEST=`acLookFor flex`
	if [ "$DEST" ]; then
	    AC_MAK FLEX
	    AC_DEFINE 'LEX' \"$DEST\"
	    AC_SUB 'LEX', "$DEST"
	    echo "lex is $DEST"
	else
	    AC_SUB LEX ''
	    echo "neither lex or flex found"
	    TLOG " (not found)"
	    return 1
	fi
    fi

    if AC_LIBRARY yywrap -ll -lfl; then
	TLOG "($DEST)"
	return 0
    fi
    TLOG "(no lex library found)"
    return 1
}


#
# AC_PROG_YACC checks to see if YACC exists, and if it's bison or
# not.
#
AC_PROG_YACC () {

    TLOGN "looking for yacc "

    DEST=`acLookFor yacc`
    if [ "$DEST" ]; then
	AC_MAK YACC
	AC_DEFINE PATH_YACC \"$DEST\"
	AC_SUB 'YACC' "$DEST"
	TLOG "($DEST)"
	echo "yacc is $DEST"
    else
	DEST=`acLookFor bison`
	if [ "$DEST" ]; then
	    AC_MAK BISON
	    AC_DEFINE 'YACC' \"$DEST\"
	    AC_SUB 'YACC' "$DEST -y"
	    echo "yacc is $DEST -y"
	    TLOG "($DEST -y)"
	else
	    AC_SUB 'YACC' ''
	    echo "neither yacc or bison found"
	    TLOG " (not found)"
	    return 1
	fi
    fi
    return 0
}


#
# AC_PROG_LN_S checks to see if ln exists, and, if so, if ln -s works
#
AC_PROG_LN_S () {
    test "$AC_FIND_PROG" || AC_PROG_FIND

    test "$AC_FIND_PROG" || return 1
    
    TLOGN "looking for \"ln -s\""
    DEST=`acLookFor ln`

    if [ "$DEST" ]; then
	rm -f "$__ac_tmpdir/b$$"
	"$DEST" -s "$__ac_tmpdir/a$$" "$__ac_tmpdir/b$$"
	if [ "`$AC_FIND_PROG $__ac_tmpdir/b$$ -type l -print`" ]; then
	    TLOG " ($DEST)"
	    echo "$DEST exists, and ln -s works"
	    AC_SUB 'LN_S' "$DEST -s"
	    rm -f "$__ac_tmpdir/b$$"
	else
	    AC_SUB 'LN_S' ''
	    TLOG " ($DEST exists, but -s does not seem to work)"
	    echo "$DEST exists, but ln -s doesn't seem to work"
	    rm -f "$__ac_tmpdir/b$$"
	    return 1
	fi
    else
	AC_SUB 'LN_S' ''
	echo "ln not found"
	TLOG " (not found)"
	return 1
    fi
}


#
# AC_PROG_FIND looks for the find program and sets the FIND environment
# variable
#
AC_PROG_FIND () {
    if test -z "$AC_FIND_PROG"; then
	MF_PATH_INCLUDE FIND find
	rc=$?
	AC_FIND_PROG="$DEST"
	return $rc
    fi
    return 0
}


#
# AC_PROG_AWK looks for the awk program and sets the AWK environment
# variable
#
AC_PROG_AWK () {
    if test -z "$AC_AWK_PROG"; then
	MF_PATH_INCLUDE AWK awk
	rc=$?
	AC_AWK_PROG="$DEST"
	return $rc
    fi
    return 0
}


#
# AC_PROG_SED looks for the sed program and sets the SED environment
# variable
#
AC_PROG_SED () {
    if test -z "$AC_SED_PROG"; then
	MF_PATH_INCLUDE SED sed
	rc=$?
	AC_SED_PROG="$DEST"
	return $rc
    fi
    return 0
}


#
# AC_HEADER_SYS_WAIT looks for sys/wait.h
#
AC_HEADER_SYS_WAIT () {
    AC_CHECK_HEADERS sys/wait.h || return 1
}

#
# AC_TYPE_PID_T checks to see if the pid_t type exists
#
AC_TYPE_PID_T () {
    cat > "$__ac_tmpdir/pd$$.c" << EOF
#include <sys/types.h>
int main(int argc, char **argv)
{
    pid_t me;
    return 0;
}
EOF

    LOGN "checking for pid_t"

    if $AC_CC -c "$__ac_tmpdir/pd$$.c" -o "$__ac_tmpdir/pd$$.o"; then
	TLOG " (found)"
	rc=0
    else
	echo "typedef int pid_t;" >> $__cwd/config.h.tmp
	TLOG " (not found)"
	rc=1
    fi
    rm -f "$__ac_tmpdir/pd$$.o" "$__ac_tmpdir/pd$$.c"
    return $rc
}


#
# AC_C_CONST checks to see if the compiler supports the const keyword
#
AC_C_CONST () {
    cat > "$__ac_tmpdir/pd$$.c" << EOF
const char me=1;
EOF
    LOGN "checking for \"const\" keyword"

    if $AC_CC -c "$__ac_tmpdir/pd$$.c" -o "$__ac_tmpdir/pd$$.o"; then
	TLOG " (yes)"
	rc=0
    else
	AC_DEFINE 'const' '/**/'
	TLOG " (no)"
	rc=1
    fi
    rm -f "$__ac_tmpdir/pd$$.o" "$__ac_tmpdir/pd$$.c"
    return $rc
}


#
# AC_SCALAR_TYPES checks to see if the compiler can generate 2 and 4 byte ints.
#
AC_SCALAR_TYPES () {
    cat > "$__ac_tmpdir/pd$$.c" << EOF
#include <stdio.h>
int main(int argc, char **argv)
{
    unsigned long v_long;
    unsigned int v_int;
    unsigned short v_short;

    if (sizeof v_long == 4)
	puts("#define DWORD	unsigned long");
    else if (sizeof v_int == 4)
	puts("#define DWORD	unsigned int");
    else
	exit(1);

    if (sizeof v_int == 2)
	puts("#define WORD	unsigned int");
    else if (sizeof v_short == 2)
	puts("#define WORD	unsigned short");
    else
	exit(2);
    puts("#define BYTE	unsigned char");
    exit(0);
}
EOF
    rc=1
    LOGN "defining WORD & DWORD scalar types"
    if $AC_CC "$__ac_tmpdir/pd$$.c" -o "$__ac_tmpdir/pd$$"; then
	if "$__ac_tmpdir/pd$$"  >> $__cwd/config.h.tmp; then
	    rc=0
	fi
    fi
    case "$rc" in
    0) TLOG "" ;;
    *) TLOG " ** FAILED **" ;;
    esac
    rm -f "$__ac_tmpdir/pd$$" "$__ac_tmpdir/pd$$.c"
}


#
# AC_OUTPUT generates makefiles from makefile.in's
#
AC_OUTPUT () {
    cd $__cwd
    AC_SUB 'LIBS'    "$AC_LIBS"
    AC_SUB 'CONFIGURE_FILES' "$__config_files"
    AC_SUB 'CONFIGURE_DETRITUS' "$__config_detritus"
    AC_SUB 'GENERATED_FILES' "$*"
    AC_SUB 'CFLAGS'  "$AC_CFLAGS"
    AC_SUB 'FCFLAGS' "$AC_FCFLAGS"
    AC_SUB 'CPPFLAGS' "$AC_CPPFLAGS"
    AC_SUB 'CXXFLAGS' "$AC_CXXFLAGS"
    AC_SUB 'LDFLAGS' "$AC_LDFLAGS"
    AC_SUB 'srcdir'  "$AC_SRCDIR"
    AC_SUB 'prefix'  "$AC_PREFIX"
    AC_SUB 'exedir'  "$AC_EXECDIR"
    AC_SUB 'sbindir' "$AC_SBINDIR"
    AC_SUB 'libdir'  "$AC_LIBDIR"
    AC_SUB 'libexec' "$AC_LIBEXEC"
    AC_SUB 'confdir' "$AC_CONFDIR"
    AC_SUB 'mandir'  "$AC_MANDIR"

    if [ -r config.sub ]; then
	test "$AC_SED_PROG" || AC_PROG_SED
	test "$AC_SED_PROG" || return 1

	echo                                   >> config.h.tmp
	echo "#endif/* ${AC_CONFIGURE_FOR} */" >> config.h.tmp

	rm -f config.cmd
	Q=\'
	cat - > config.cmd << EOF
#! /bin/sh
${CXX:+CXX=${Q}${CXX}${Q}} ${CPPFLAGS:+CPPFLAGS=${Q}${CPPFLAGS}${Q}} ${CXXFLAGS:+CXXFLAGS=${Q}${CXXFLAGS}${Q}} ${FC:+FC=${Q}${FC}${Q}} ${FCFLAGS:+FCFLAGS=${Q}${FCFLAGS}${Q}} ${CC:+CC=${Q}${CC}${Q}} ${CFLAGS:+CFLAGS=${Q}${CFLAGS}${Q}} $ac_progname $ac_configure_command
EOF
	chmod +x config.cmd

	__d=$AC_SRCDIR
	for makefile in $*;do
	    if test -r $__d/${makefile}.in; then
		LOG "generating $makefile"
		./config.md `__ac_dirname ./$makefile` 2>/dev/null
		$AC_SED_PROG -f config.sub < $__d/${makefile}.in > $makefile
		__config_files="$__config_files $makefile"
	    else
		LOG "WARNING: ${makefile}.in does not exist!"
	    fi
	done
	unset __d

    else
	echo 
    fi
    cp $__cwd/config.h.tmp $__cwd/config.h
}

#
# AC_CHECK_FLOCK checks to see if flock() exists and if the LOCK_NB argument
# works properly.
#
AC_CHECK_FLOCK() {

    AC_CHECK_HEADERS sys/types.h sys/file.h fcntl.h

    cat << EOF > $$.c
#include <stdio.h>
#include <sys/file.h>
#include <sys/types.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
    int x = open("$$.c", O_RDWR, 0666);
    int y = open("$$.c", O_RDWR, 0666);

    if (flock(x, LOCK_EX) != 0)
	exit(1);
    if (flock(y, LOCK_EX|LOCK_NB) == 0)
	exit(1);
    exit(0);
}
EOF

    LOGN "checking for flock()"
    HAS_FLOCK=0
    if $AC_CC -o flock $$.c ; then
	if ./flock ; then
	    LOG " (found)"
	    HAS_FLOCK=1
	    AC_DEFINE HAS_FLOCK
	else
	    LOG " (bad)"
	fi
    else
	LOG " (no)"
    fi

    rm -f flock $$.c

    case "$HAS_FLOCK" in
    0) return 1 ;;
    *) return 0 ;;
    esac
}


#
# AC_CHECK_RESOLVER finds out whether the berkeley resolver is
# present on this system.
#
AC_CHECK_RESOLVER () {
    AC_PROG_CC

    TLOGN "checking for the Berkeley resolver library"

    cat > "$__ac_tmpdir/ngc$$.c" << EOF
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

int main(int argc, char **argv)
{
    char bfr[256];

    res_init();
    res_query("hello", C_IN, T_A, bfr, sizeof bfr);
}
EOF

    # first see if res_init() and res_query() actually exist...

    if $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c"; then
	__extralib=
    elif $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" -lresolv; then
	__extralib=-lresolv
	AC_LIBS="$AC_LIBS -lresolv"
    else
	TLOG " (not found)"
	rm -f "$__ac_tmpdir/ngc$$.c"
	return 1
    fi

    # if res_init() and res_query() actually exist, check to
    # see if the HEADER structure is defined ...

    cat > "$__ac_tmpdir/ngc$$.c" << EOF
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>

int main(int argc, char **argv)
{
    HEADER hhh;
    res_init();
}
EOF

    if $AC_CC -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" $__extralib; then
	TLOG " (found)"
    elif $AC_CC -DBIND_8_COMPAT -o "$__ac_tmpdir/ngc$$" "$__ac_tmpdir/ngc$$.c" $__extralib; then
	TLOG " (bind9 with BIND_8_COMPAT)"
	AC_DEFINE BIND_8_COMPAT 1
    else
	TLOG " (broken)"
	rm -f "$__ac_tmpdir/ngc$$.c"
	return 1
    fi
    rm -f "$__ac_tmpdir/ngc$$.c"
    return 0
}


#
# AC_PROG_INSTALL finds the install program and guesses whether it's a 
# Berkeley or GNU install program
#
AC_PROG_INSTALL () {

    DEST=`acLookFor install`

    LOGN "checking for install"
    unset IS_BSD
    if [ "$DEST" ]; then
	# BSD install or GNU install?  Let's find out...
	touch "$__ac_tmpdir/a$$"

	"$DEST" "$__ac_tmpdir/a$$" "$__ac_tmpdir/b$$"

	if test -r "$__ac_tmpdir/a$$"; then
	    LOG " ($DEST)"
	else
	    IS_BSD=1
	    LOG " ($DEST) bsd install"
	fi
	rm -f "$__ac_tmpdir/a$$" "$__ac_tmpdir/b$$"
    else
	DEST=`acLookFor ginstall`
	if [ "$DEST" ]; then
	    LOG " ($DEST)"
	else
	    DEST="false"
	    LOG " (not found)"
	fi
    fi

    if [ "$IS_BSD" ]; then
	PROG_INSTALL="$DEST -c"
    else
	PROG_INSTALL="$DEST"
    fi

    AC_SUB 'INSTALL' "$PROG_INSTALL"
    AC_SUB 'INSTALL_PROGRAM' "$PROG_INSTALL -s -m 755"
    AC_SUB 'INSTALL_DATA' "$PROG_INSTALL -m 444"

    # finally build a little directory installer
    # if mkdir -p works, use that, otherwise use install -d,
    # otherwise build a script to do it by hand.
    # in every case, test to see if the directory exists before
    # making it.

    if mkdir -p $$a/b; then
	# I like this method best.
	__mkdir="mkdir -p"
	rmdir $$a/b
	rmdir $$a
    elif $PROG_INSTALL -d $$a/b; then
	__mkdir="$PROG_INSTALL -d"
	rmdir $$a/b
	rmdir $$a
    fi

    __config_files="$__config_files config.md"
    AC_SUB 'INSTALL_DIR' "$__cwd/config.md"
    echo "#! /bin/sh"                                   > $__cwd/config.md
    echo "# script generated" `date` "by configure.sh" >> $__cwd/config.md
    echo                                               >> $__cwd/config.md
    if [ "$__mkdir" ]; then
	echo "test -d \"\$1\" || $__mkdir \"\$1\""     >> $__cwd/config.md
	echo "exit $?"                                 >> $__cwd/config.md
    else
	cat - >> $__cwd/config.md << \EOD
pieces=`IFS=/; for x in $1; do echo $x; done`
dir=
for x in $pieces; do
    dir="$dir$x"
    mkdir $dir || exit 1
    dir="$dir/"
done
exit 0
EOD
    fi
    chmod +x $__cwd/config.md
}

#
# acCheckCPP is a local that runs a C preprocessor with a given set of
# compiler options
#
acCheckCPP () {
	cat > "$__ac_tmpdir/ngc$$.c" << EOF
#define FOO BAR

FOO
EOF

    if $1 $2 "$__ac_tmpdir/ngc$$.c" > "$__ac_tmpdir/ngc$$.o"; then
	if grep -v '#define' "$__ac_tmpdir/ngc$$.o" | grep -s BAR >/dev/null; then
	    echo "CPP=[$1], CPPFLAGS=[$2]"
	    AC_SUB 'CPP' "$1"
	    AC_SUB 'CPPFLAGS' "$2"
	    rm "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$.o"
	    return 0
	fi
    fi
    rm "$__ac_tmpdir/ngc$$.c" "$__ac_tmpdir/ngc$$.o"
    return 1
}


#
# AC_PROG_CPP checks for cpp, then checks to see which CPPFLAGS are needed
# to run it as a filter.
#
AC_PROG_CPP () {
    if [ "$AC_CPP_PROG" ]; then
	DEST="$AC_CPP_PROG"
    else
	__ac_path="$AC_PATH"
	AC_PATH="/lib:/usr/lib:${__ac_path:-$ac_default_path}"
	DEST=`acLookFor cpp`
	AC_PATH="$__ac_path"
    fi

    unset fail
    LOGN "Looking for cpp"
    if [ "$DEST" ]; then
	TLOGN " ($DEST)"
	acCheckCPP "$DEST" "$CPPFLAGS" || \
		 acCheckCPP "$DEST" -traditional-cpp -E || \
		 acCheckCPP "$DEST" -E || \
		 acCheckCPP "$DEST" -traditional-cpp -pipe || \
	         acCheckCPP "$DEST" -pipe || fail=1

	if [ "$fail" ]; then
	    AC_FAIL " (can't run cpp as a pipeline)"
	else
	    TLOG " ok"
	    return 0
	fi
    fi
    AC_FAIL " (not found)"
}

#
# AC_FAIL spits out an error message, then __fail's 
AC_FAIL() {
    LOG "$*" 
    $__fail 1
}

#
# AC_SUB writes a substitution into config.sub
AC_SUB() {
    (   echononl "s;@$1@;"
	_subst=`echo $2 | sed -e 's/;/\\;/g'`
	echononl "$_subst"
	echo ';g' ) >> $__cwd/config.sub
}

#
# AC_MAK writes a define into config.mak
AC_MAK() {
    echo "HAVE_$1 = 1" >> $__cwd/config.mak
}

#
# AC_DEFINE adds a #define to config.h
AC_DEFINE() {
    local name="$1" value="${2:-1}"
    if ! printf -v "ac_defined_$name" '%s' "$value"; then
        AC_FATAL 'AC_DEFINE unable to set "ac_defined_$name" to "$value"'
    fi
    echo "#define $name $value" >> $__cwd/config.h.tmp
}

#
# AC_INCLUDE adds a #include to config.h
AC_INCLUDE() {
    echo "#include \"$1\"" >> $__cwd/config.h.tmp
}

#
# AC_CONFIG adds a configuration setting to all the config files
AC_CONFIG() {
    AC_DEFINE "PATH_$1" \""$2"\"
    AC_MAK "$1"
    AC_SUB "$1" "$2"
}

#
# AC_QUIET does something quietly
AC_QUIET() {
    eval $* 5>/dev/null
}
