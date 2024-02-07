@echo off

set opts=-FC -GR- -EHa- -nologo -Zi
set code=%cd%
pushd .\build\

del *.pdb > NUL 2> NUL

cl %opts% %code%\main.c -Femain.exe /link -incremental:no

echo > temp.lock

cl -LD %opts% %code%\game.c /link -incremental:no -PDB:game_pdb_%random%.pdb

del temp.lock

popd
