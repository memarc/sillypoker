
AC_DEFUN(AC_FIND_FILE,
[
$3=NO
for i in $2;
do
  for j in $1;
  do
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])


AC_DEFUN(AC_PATH_QT_LIB,
[
  AC_REQUIRE_CPP()
  AC_REQUIRE([AC_PATH_X])
  AC_MSG_CHECKING(for Qt libraries)

  ac_qt_libraries="no"

  AC_ARG_WITH(qt-libraries,
    [  --with-qt-libraries     where the Qt libraries are located. ],
    [  ac_qt_libraries="$withval" ])

  AC_CACHE_VAL(ac_cv_lib_qtlib, [

    dnl Did the user give --with-qt-libraries?

    if test "$ac_qt_libraries" = no; then

      dnl No they didn't, so lets look for them...

      dnl If you need to add extra directories to check, add them here.

      qt_library_dirs="\
        /usr/lib/qt2/lib \
        /usr/lib \
        /usr/local/lib \
        /usr/lib/qt \
        /usr/lib/qt/lib \
        /usr/local/lib/qt \
        /usr/local/qt/lib \
        /usr/X11/lib \
        /usr/X11/lib/qt \
        /usr/X11R6/lib \
        /usr/X11R6/lib/qt"

      if test "x$QTDIR" != x; then
        qt_library_dirs="$QTDIR/lib $qt_library_dirs"
      fi

      if test "x$QTLIB" != x; then
        qt_library_dirs="$QTLIB $qt_library_dirs"
      fi

      for qt_dir in $qt_library_dirs; do
        for qt_check_lib in $qt_dir/libqt.so.2; do
          if test -r $qt_check_lib; then
            ac_qt_libraries=$qt_dir
            break 2
          fi
        done
      done
    fi

    ac_cv_lib_qtlib=$ac_qt_libraries
  ])

  dnl Define a shell variable for later checks

  if test "$ac_cv_lib_qtlib" = no; then
    have_qt_lib="no"
  else
    have_qt_lib="yes"
  fi

  dnl Check if we have the right lib
  output=`eval "strings $ac_cv_lib_qtlib/libqt.so | grep -l QCString"`
  if test -z "$output"; then
    AC_MSG_ERROR([The Qt lib directory
        $ac_cv_lib_qtlib
        found by configure doesn't contain a Qt 2.xx lib.
        Please check your installation.
        Use the --with-qt-libraries option to overwrite the check.
    ])
  fi

  AC_MSG_RESULT([$ac_cv_lib_qtlib])
  QT_LDFLAGS="-L$ac_cv_lib_qtlib"
  QT_LIBDIR="$ac_cv_lib_qtlib"
  AC_SUBST(QT_LDFLAGS)
  AC_SUBST(QT_LIBDIR)
])

AC_DEFUN(AC_PATH_QT_INC,
[
  AC_REQUIRE_CPP()
  AC_REQUIRE([AC_PATH_X])
  AC_MSG_CHECKING(for Qt includes)

  ac_qt_includes="no"

  AC_ARG_WITH(qt-includes,
    [  --with-qt-includes      where the Qt headers are located. ],
    [  ac_qt_includes="$withval" ])

  AC_CACHE_VAL(ac_cv_header_qtinc, [

    dnl Did the user give --with-qt-includes?

    if test "$ac_qt_includes" = no; then

      dnl No they didn't, so lets look for them...

      dnl If you need to add extra directories to check, add them here.

      qt_include_dirs="\
        /usr/lib/qt2/include \
        /usr/lib/qt/include \
        /usr/include/qt \
        /usr/local/qt/include \
        /usr/local/include/qt \
        /usr/X11/include/qt \
        /usr/X11/include/X11/qt \
        /usr/X11R6/include \
        /usr/X11R6/include/qt \
        /usr/X11R6/include/X11/qt \
        /usr/X11/lib/qt/include"

      if test -n "$QTDIR"; then
        qt_include_dirs="$QTDIR/include $qt_include_dirs"
      fi

      if test -n "$QTINC"; then
        qt_include_dirs="$QTINC $qt_include_dirs"
      fi

      for qt_dir in $qt_include_dirs; do
        if test -r "$qt_dir/qcstring.h"; then
          ac_qt_includes=$qt_dir
          break
        fi
      done
    fi

    ac_cv_header_qtinc=$ac_qt_includes

  ])

  if test "$ac_cv_header_qtinc" = no; then
    have_qt_inc="no"
  else
    have_qt_inc="yes"
  fi

  AC_MSG_RESULT([$ac_cv_header_qtinc])
  QT_INCLUDES="-I$ac_cv_header_qtinc"
  QT_INCDIR="$ac_cv_header_qtinc"
  AC_SUBST(QT_INCLUDES)
  AC_SUBST(QT_INCDIR)
])


AC_DEFUN(AC_PATH_QT_MOC,
[
  ac_qt_moc="no"

  AC_ARG_WITH(qt-moc,
    [  --with-qt-moc           where the Qt 2.xx moc is located. ],
    [  ac_qt_moc="$withval" ])

  if test "$ac_qt_moc" = "no"; then
    dnl search on our own

    if test -z "$QTDIR"; then
      AC_MSG_WARN(environment variable QTDIR is not set, moc might not be found)
    fi

    AC_PATH_PROG(
      MOC,
      moc,
      $QTDIR/bin/moc,
      $QTDIR/bin:/usr/lib/qt2/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH
    )
  else
    AC_MSG_CHECKING(for moc)

    if test -f $ac_qt_moc && test -x $ac_qt_moc; then
      MOC=$ac_qt_moc
    else
      AC_MSG_ERROR(
        --with-qt-moc expects path and name of the moc
      )
    fi

    AC_MSG_RESULT($MOC)
  fi

  if test -z "$MOC"; then
    AC_MSG_ERROR(couldn't find Qt moc. Please use --with-qt-moc)
  fi

  dnl Check if we have the right moc
  output=`eval "strings $MOC | grep -l QCString"`
  if test -z "$output"; then
    AC_MSG_ERROR([

        The Qt meta object compiler (moc)
        $MOC
        found by configure is not the one part of Qt 2.xx.

        It's likely that the found one is the one shipped with
        Qt 1.xx. That one will not work.

        Please check your installation.
        Use the --with-qt-moc option to specify the path and name
        of the moc compiler shipped with your Qt 2.xx lib.
        see ./configure --help for details.
    ])
  fi

  AC_SUBST(MOC)
])

AC_DEFUN(AC_PATH_QT_FINDTR,
[
  AC_PATH_PROG(
    QT_FINDTR,
    findtr,
    echo,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)

  if test "$QT_FINDTR" = "echo"; then
    echo "** findtr could not be found. You're losing the localisation."
  fi

  AC_SUBST(QT_FINDTR)
])

AC_DEFUN(AC_PATH_QT_MSGTOQM,
[
  AC_PATH_PROG(
    QT_MSG2QM,
    msg2qm,
    echo,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)

  if test "$QT_MSG2QM" = "echo"; then
    echo "** msg2qm could not be found. You're losing the localisation."
  fi

  AC_SUBST(QT_MSG2QM)
])

AC_DEFUN(AC_PATH_QT_MERGETR,
[
  AC_PATH_PROG(
    QT_MERGETR,
    mergetr,
    echo,
    $QTDIR/bin:/usr/bin:/usr/X11R6/bin:/usr/lib/qt/bin:/usr/local/qt/bin:$PATH)

  if test "$QT_MERGETR" = "echo"; then
    echo "** mergetr could not be found. You're losing the localisation."
  fi

  AC_SUBST(QT_MERGETR)
])

