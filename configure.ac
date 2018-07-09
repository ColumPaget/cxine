#AC_INIT(libUseful,colums.projects@googlemail.com,,http://sites.google.com/site/columscode) 
AC_INIT(cxine.c)

VERSION="0.0.1" 
AC_SUBST(VERSION) 

AC_PROG_CC
AC_LANG_C
AC_PROG_MAKE_SET
AC_HEADER_STDC
AC_SYS_LARGEFILE

AC_CHECK_FILE([/usr/X11/include/X11/X.h], [X11ROOT="/usr/X11"],
							AC_CHECK_FILE([/usr/X11R7/include/X11/X.h], [X11ROOT="/usr/X11R7"],
							AC_CHECK_FILE([/usr/X11R6/include/X11/X.h], [X11ROOT="/usr/X11R6"])
							)
							)

if test "X11ROOT" != ""
then
	X11INC="-I$X11ROOT/include"
	X11LIB="-L$X11ROOT/lib"
fi

AC_ARG_WITH(x-includes, [  --with-x-includes       path to X11 includes directory], X11INC="-I$withval" )
AC_ARG_WITH(x-libraries, [  --with-x-libraries     path to X11 libraries directory], X11LIB="-L$withval" )

LDFLAGS="$LDFLAGS $X11LIB"
CFLAGS="$CFLAGS $X11INC"


AC_CHECK_LIB(m,sqrt,,)
cf_have_x11=$ac_cv_lib_m_sqrt


AC_CHECK_LIB(xine,xine_new,,)
cf_have_xine=$ac_cv_lib_xine_xine_new

if test "$cf_have_xine" = "yes"
then
AC_DEFINE([HAVE_XINE])
else 
echo "ERROR: cannot find libxine"
exit
fi

AC_CHECK_LIB(X11,XOpenDisplay,,)
cf_have_x11=$ac_cv_lib_X11_XOpenDisplay
if test "$cf_have_x11" = "yes"
then
AC_DEFINE([HAVE_X11])
else 
echo "ERROR: cannot find libX11"
exit
fi

AC_CHECK_LIB(Xss, XScreenSaverSuspend,,)
cf_have_xss=$ac_cv_lib_Xss_XScreenSaverSuspend
if test "$cf_have_xss" = "yes"
then
AC_DEFINE([HAVE_XSCREENSAVER])
else
echo "WARNING: cannot find libXss. Screensaver control functions will be disabled."
fi







dnl read Makefile.in and write Makefile 
AC_OUTPUT(Makefile)


