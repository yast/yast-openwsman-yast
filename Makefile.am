SUBDIRS = src

AUTOMAKE_OPTIONS = no-dist-gzip dist-bzip2

package: dist
	cp openwsman-yast.spec /usr/src/packages/SPECS
	cp openwsman-yast*.tar.bz2 /usr/src/packages/SOURCES
	rpmbuild -ba openwsman-yast.spec

etags: TAGS
	find . -name "*.[chCH]" -print | etags -
