#SVN_VERSION=$$system(svn info -r HEAD . | grep 'Changed\ Rev' | cut -b 19-)
SVN_VERSION=unknown
DEFINES *= "SVN_VERSION=\"\\\"$${SVN_VERSION}\\\"\""
