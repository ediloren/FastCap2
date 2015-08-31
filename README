
                       F a s t C a p   2 . 0 w r
                       -------------------------
                               1-23-2003

Modifications by:        Stephen R. Whiteley  (stevew@wrcad.com)
                         Whitleley Research Inc.

** This is the fastcap-2.0-18Sept92 distribution with the following
   changes:

3-12-2001 release:

  1)  A new list file format option allows some or all input files to
      be included in that one file.  This avoids the former need for
      sometimes large numbers of separate input files.  The format is
      as follows:

        (regular list file)
        End
        File <the_first_file>
        (contents of <the_first_file>)
        End
        File <the_second_file>
        (contents of <the_second_file>)
        End
        ...
  
      The characters following 'E' in "End" and following 'F' in "File"
      are optional.  Like the other keys, 'E' and 'F' are
      case-insensitive, and must be in the first text column.  The
      files are those referenced in the list file, and can be appended
      in any order.  Referenced files that are not appended are
      expected to be disk files.  The "End" after a neutral file is
      optional, as is the "End" after the final appended file.

      The modified list file is input in the regular way, i.e., with
      "-lfilename".

  2)  Blank lines are now allowed in list files (though maybe not in
      appended files, I haven't checked).

  3)  All references to "malloc.h" in the source code have been changed
      to "stdlib.h" to avoid a cascade of warnings during compilation.

  4)  Added static declarations to mulSetup.c to avoid annoying
      compiler warnings.

  5)  This version of the fastcap program exits with status 0 if there
      were no errors, nonzero otherwise.  The original fastcap exit
      status was undefined.

  6)  The latex files for the manual have been eliminated, and the
      postscript documents provided instead.

  7)  Changed name "stime" in resusage.h to "s__time" to avoid name
      clash from library include files in Red Hat Linux 6.0.

  8)  Ported to Microsoft Windows using the mingw (www.mingw.org)
      version of gcc.  To build under mingw, use "config mingw".  The
      source code was made sensitive to the following defines, which
      are all necessary for mingw.

      a) NO_SBRK            The library lacks the sbrk() function.
      b) NO_GETHOSTNAME     Skip the call to gethostname().  Actually,
                            mingw has this, but it requires loading
                            winsock2.

1-21-2003 release:

  9)  The internal memory allocator (ualloc) causes an immediate
      core dump on Solaris 8 systems, and was therefor removed.
      If you really want the internal allocator (no sane reason to
      use it) define USE_UALLOC in the makefile.

1-23-2003 release:

  10) A premature EOF could hang the program, should no longer be
      true.

  11) Dummy surfaces did not have the surf pointer set, so if these
      were removed in placeq(), the print line would cause a seg
      fault.  The pointers are now set in read_panels().

4-27-2004 release:

  12) In uglyalloc.c, changed comparison expression to avoid bad
      cast when int and char* have different sizes.

      All programs compile without warnings on 64-bit sparc gcc-3.x
      (Solaris 8) using CC = gcc and CFLAGS = -O -m64 -DOTHER.

1-11-2009 release:

  13) Misc. small tweaks to avoid compiler warning messages from,
      e.g., gcc-4.2.

  14) Added msw_package directory containing stuff for generating
      distribution packages for Windows (mostly for my convenience).

1-1-2014 release:

  15) Added function prototypes to all source, tweaks and fixes
      for clean build under OS X 10.9.

7-6-2014 release:

  16) Fixed an old fastcap bug that could seg. fault when reading
      triangular panels.  Other misc. small fixes imported from
      Enrico Di Lorenzo's FastCap2 source code.

7-7-2014 release:

  17) Fix a compiler warning under Apple OS X Mavericks introduced in
  the last update.
