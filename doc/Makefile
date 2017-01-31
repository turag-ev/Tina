.PHONY: doc clean qtdoc doc-gnav
doc:
	doxygen doxyfile > /dev/null

qtdoc:
	doxygen doxyfile.qtdoc > /dev/null

doc-gnav:
	doxygen doxyfile.gnav > /dev/null
	

clean:
	rm -rf html
	rm -rf qtdoc
	rm -rf html-gnav
