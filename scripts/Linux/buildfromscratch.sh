#!/bin/bash
find . -type f -name Makefile -exec rm -vrf {} \;
find . -type f -name .qmake.cache -exec rm -vrf {} \;
rm -vrf ./build
rm -vrf ./bin
