#!/bin/sh
dos2unix `find ./ -type f`
# or
# $ find . -type f -exec dos2unix {} {} \;