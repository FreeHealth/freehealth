TEMPLATE = subdirs

CONFIG += ordered

unix { SUBDIRS = quazip }
else { SUBDIRS = zlib-1.2.3 quazip }

