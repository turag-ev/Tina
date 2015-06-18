.PHONY: doc clean qtdoc
doc:
	doxygen doxyfile > /dev/null

qtdoc:
	doxygen doxyfile.qtdoc > /dev/null
	
tests:
	cd tests
	make -j4

clean:
	rm -rf doc/html
	rm -rf doc/qtdoc
