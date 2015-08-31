All compilation and document production is done using the master makefile
in this directory.  Subdirectories include:
	"src" - all C source files for `fastcap' and the input generators
	"doc" - all latex source files for the FastCap User's Guide
	"examples" - example PATRAN neutral format input files from the Guide
	"bin" - where executables are stored after making

1. To un-tar the fastcap files type (for example)
	uncompress fastcap-2.0-27May92.tar.Z
	tar xvf fastcap-2.0-27May92.tar

2. To compile and install (in `bin') the default version of `fastcap' type
   `make all' -- this actually makes `fastcap' and the input file
   generator programs `busgen' `capgen' and `cubegen' as well.  All four
   executables can be made individually (eg `make fastcap').
   
3. To compile and install `fastcap' with execution timing enabled
   (TIMDAT = ON in src/mulGlobal.h), the makefile must be configured
   to match the operating system.  To do this from this directory,
   type `config 4' for 4.2/3BSD systems, `config 5' for System V or 
   `config aix' for IBM AIX systems.  It is recommended that this
   proceedure always be carried out for IBM AIX systems, even if
   execution timing is disabled (so that xlc is used instead of cc).
   With the makefile configured, compilation is as in step 2.  

4. To produce the FastCap User's Guide type `make manual' to produce three
   .dvi files `doc/ug.dvi' `doc/tcad.dvi' and `doc/mtt.dvi -- the files
   `tcad.dvi' and `mtt.dvi' produce reprints of two related journal
   articles.  The makefile automatically changes the definition of the 
   `\topdir' macro in both `tcad.tex'  `ug.tex' and `mtt.tex' to match 
   the installed top directory -- LaTeX will produce an error if the .dvi 
   files are not produced with the makefile or the awk script responsible
   for changine `\topdir' fails.  If the awk script fails, see work around
   comments in doc/README.  No .dvi files are provided in the release 
   -- they must be produced by the user.

5. To remove object files in `src' type `make clean'.

6. To report bugs send mail to bug-fastcap@rle-vlsi.mit.edu; for routine
   correspondence use fastcap@rle-vlsi.mit.edu

(27 May 92)
