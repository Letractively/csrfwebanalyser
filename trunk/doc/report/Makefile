# Makefile for generating report.pdf

LATEX = latex

.SUFFIXES: .eps

FILES = \
	report.tex \
	abstract.tex \
	results.tex \
	report.bib

default: pdf

test:
	-$(LATEX) report

report.bbl: $(FILES)
	-bibtex report 

pdf: $(FILES) report.bbl
	-$(LATEX) report
	-bibtex   report
	-$(LATEX) report
	-$(LATEX) report
	-$(LATEX) report
	-dvips -t letter -o report.ps report.dvi
	-dvipdf report.dvi report.pdf

clean:
	rm -f *.log *.aux *.blg *.ps *.pdf *.dvi *.bbl
