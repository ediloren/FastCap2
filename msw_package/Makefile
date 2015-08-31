
# This will create a Windows package in the current directory.  It needs
# 1) the inno packager installed in /inno-5.5.1
# 2) a utd utility accessible somewhere (Cygwin?)
# 
# You can tune mkwinpkg and/or fastcap.iss.in to your needs.


package::
	./mkwinpkg
	rm -rf data fastcap.iss

clean::
	rm -rf data fastcap.iss *.exe *.log

distclean::
	rm -rf data fastcap.iss *.exe *.log
