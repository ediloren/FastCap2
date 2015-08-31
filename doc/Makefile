# Makefile to produce .dvi files
# ug - users guide
# tcad - Trans on CAD paper in memo form with pages offset (for ug appendix)
# manual - ug and tcad
# NOTE: \topdir must be set up in both ug.tex and tcad.tex before any latexing
# - this is now done automatically before latexing (see below ug.dvi, tcad.dvi)
SHELL = /bin/sh

UG = .
TCAD = .
MTT = .
BIB1 = .
BIB2 = .

UGP = ug-
UGPM = ug
TCADP = tcad-
TCADPM = tcad
MTTP = mtt-
MTTPM = mtt
BAR =


UGTEX = $(UG)/$(UGPM).tex $(UG)/$(UGP)all.tex $(UG)/$(UGP)intro.tex \
$(UG)/$(UGP)run.tex $(UG)/$(UGP)compile.tex 

TCADTEX = $(TCAD)/$(TCADPM).tex $(TCAD)/$(TCADP)abstract.tex \
$(TCAD)/$(TCADP)accel.tex $(TCAD)/$(TCADP)form.tex \
$(TCAD)/$(TCADP)gcr.tex $(TCAD)/$(TCADP)integral.tex \
$(TCAD)/$(TCADP)intro.tex $(TCAD)/$(TCADP)math.tex \
$(TCAD)/$(TCADP)ref.tex $(TCAD)/$(TCADP)results.tex \
$(TCAD)/$(TCADP)moments.tex

TCADSTY = $(TCAD)/memo.sty

MTTTEX = $(MTT)/$(MTTPM).tex $(MTT)/$(MTTP)abstract.tex \
$(MTT)/$(MTTP)intro.tex $(MTT)/$(MTTP)multi.tex \
$(MTT)/$(MTTP)adapt.tex $(MTT)/$(MTTP)precond.tex \
$(MTT)/$(MTTP)results.tex $(MTT)/$(MTTP)conclu.tex

MTTBIB = $(BIB1)/capieee.bib $(BIB1)/capart.bib \
$(BIB1)/capbook.bib $(BIB1)/capother.bib \
$(BIB2)/local.bib

MTTSTY = $(MTT)/doublespace.sty $(MTT)/ieee.sty $(MTT)/newthanks.sty \
$(MTT)/maketitle_mod.sty


default:
	@ echo Specify which documents to produce:
	@ echo " ug.dvi - users guide"
	@ echo " tcad.dvi - Trans on CAD paper"
	@ echo " mtt.dvi - MTT paper"
	@ echo " manual - ug.dvi tcad.dvi and mtt.dvi"

manual: ug.dvi tcad.dvi mtt.dvi

ug.dvi: $(UGTEX)
	cd $(UG) ; pwd > temp_ ; \
	awk '{s = sprintf("\\\\newcommand{\\\\topdir}{%s}", $$1)} END {printf "{if($$0 ~ /\\\\newcommand{\\\\topdir}/) print \"%s\" ; else print $$0 }", s }' \
	temp_ > temp__ ; \
	awk -f temp__ ug.tex > temp_ ; \
	mv temp_ ug.tex ; \
	/bin/rm temp__
	cd $(UG) ; latex ug ; latex ug ; latex ug 

tcad.dvi: $(TCADTEX) $(TCADSTY)
	cd $(TCAD) ; pwd > temp_ ; \
	awk '{s = sprintf("\\\\newcommand{\\\\topdir}{%s}", $$1)} END {printf "{if($$0 ~ /\\\\newcommand{\\\\topdir}/) print \"%s\" ; else print $$0 }", s }' \
	temp_ > temp__ ; \
	awk -f temp__ tcad.tex > temp_ ; \
	mv temp_ tcad.tex ; \
	/bin/rm temp__
	cd $(TCAD) ; latex tcad ; latex tcad ; latex tcad

mtt.dvi: $(MTTTEX) $(MTTBIB) $(MTTSTY)
	cd $(MTT) ; pwd > temp_ ; \
	awk '{s = sprintf("\\\\newcommand{\\\\topdir}{%s}", $$1)} END {printf "{if($$0 ~ /\\\\newcommand{\\\\topdir}/) print \"%s\" ; else print $$0 }", s }' \
	temp_ > temp__ ; \
	awk -f temp__ mtt.tex > temp_ ; \
	mv temp_ mtt.tex ; \
	/bin/rm temp__
	cd $(MTT) ; latex mtt ; bibtex mtt ; latex mtt ; latex mtt

clean:
	cd $(UG) ; /bin/rm -f *.dvi *.log *.toc *.aux *.bak *.lot *.lof
	cd $(TCAD) ; /bin/rm -f *.dvi *.log *.toc *.aux *.bak *.lot *.lof
	cd $(MTT) ; /bin/rm -f *.dvi *.log *.toc *.aux *.bak *.lot *.lof
