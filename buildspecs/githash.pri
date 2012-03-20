# Catch the current git revision hash
GIT_HASH=$$system(git rev-parse HEAD)
DEFINES *= "GIT_REVISION_HASH=\"\\\"$${GIT_HASH}\\\"\""
