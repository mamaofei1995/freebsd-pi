# The FreeBSD Russian Documentation Project
#
# $FreeBSD: projects/armv6/release/doc/ru_RU.KOI8-R/hardware/Makefile.inc 133479 2004-08-11 10:25:23Z den $
# $FreeBSDru: frdp/release/doc/ru_RU.KOI8-R/hardware/Makefile.inc,v 1.2 2004/08/11 10:04:26 den Exp $
#
# Original revision: 1.3

.ifdef NO_LANGCODE_IN_DESTDIR
DESTDIR?=	${DOCDIR}/hardware/${.CURDIR:T}
.else
DESTDIR?=	${DOCDIR}/ru_RU.KOI8-R/hardware/${.CURDIR:T}
.endif

# Force include of the autogenerated catalog.  It has to be set
# directly in the CATALOG variable since doc.docbook.mk check if each
# catalog file exist for the other *CATALOG variables, and
# catalog-auto does not exist yet when that check is performed,
# causing catalog-auto not to be added to CATALOGS which breaks the
# build.
CATALOGS+=	-c ${DEV-AUTODIR}/catalog-auto