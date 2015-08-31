# if the src/Makefile is of the wrong type and the new type makefile
# `Makefile.type' is OK, copy  Makefile.type into Makefile
# - string of if/elifs, rather than case statement, is used for compatability
# - if a new type of makefile is needed, use call it `Makefile.[newtype]'
# - imported variables: AWK = $1 ; TYPE = $2 ; TOP = $3 ; RM = $4
#	# see if the makefile of right type (non-null temp_ if OK)
	$1 "/MAKETYPE = $2/" Makefile > temp_	
#	# check the cases - temp_ size, existance of Makefile.$2 are vars
	if test -s temp_ -a ! -r Makefile.$2 ; \
	then echo "Seems like the current makefile is type $2" ; \
	echo "  but Makefile.$2 doesn't exist?????" ; \
	elif test ! -s temp_ -a ! -r Makefile.$2 ; \
	then echo "There is no makefile of type $2" ; \
	echo "  specify 4, 5, aix or other" ; \
	elif test -s temp_ -a -r Makefile.$2 ; \
	then : ; \
	elif test ! -s temp_ -a -r Makefile.$2 ; \
	then mv Makefile Makefile.bak ; \
	cp Makefile.$2 Makefile ; \
	mv $3/Makefile $3/Makefile.bak ; \
	$1 "{ if ( \$0 ~ /TYPE =/ ) { print \"TYPE = $2\" } else print \$0 }" $3/Makefile.bak > $3/Makefile ; \
	echo "Type $2 makefile set up" ; \
	else : ; fi
	$4 temp_
