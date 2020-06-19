.PHONY: doc clean qtdoc doc-gnav sc_doc

doc:
	doxygen doxyfile > /dev/null

qtdoc:
	doxygen doxyfile.qtdoc > /dev/null

doc-gnav:
	doxygen doxyfile.gnav > /dev/null

sc_doc: doc
	doxygen doxyfile_systemcontrol > /dev/null

clean:
	rm -rf html
	rm -rf qtdoc
	rm -rf html-gnav
	rm -rf systemcontrol
	rm -rf tina.tag
