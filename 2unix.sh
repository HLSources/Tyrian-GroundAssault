#!/bin/sh
dos2unix `find ./common -type f -iname '*.c*'`
dos2unix `find ./common -type f -iname '*.h'`
dos2unix `find ./dedicated -type f -iname '*.c*'`
dos2unix `find ./dedicated -type f -iname '*.h'`
dos2unix `find ./dlls -type f -iname '*.c*'`
dos2unix `find ./dlls -type f -iname '*.h*'`
dos2unix `find ./engine -type f -iname '*.h'`
dos2unix `find ./game_shared -type f -iname '*.c*'`
dos2unix `find ./game_shared -type f -iname '*.h'`
dos2unix `find ./pm_shared -type f -iname '*.c*'`
dos2unix `find ./pm_shared -type f -iname '*.h'`
dos2unix `find ./xbm -type f -iname '*.c*'`
dos2unix `find ./xbm -type f -iname '*.h'`
# or
# $ find . -type f -exec dos2unix {} {} \;