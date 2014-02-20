cd autofill
make
if errorlevel 1 goto done
cd ..
cd blockmonkey
make
if errorlevel 1 goto done
cd ..
cd bumpmap
make
if errorlevel 1 goto done
cd ..
cd chrome
make
if errorlevel 1 goto done
cd ..
cd ci8fb
make
if errorlevel 1 goto done
cd ..
cd detail
make
if errorlevel 1 goto done
cd ..
cd eeptest
make
if errorlevel 1 goto done
cd ..
cd fault
make
if errorlevel 1 goto done
cd ..
cd fogworld
make
if errorlevel 1 goto done
cd ..
cd ginv
make
if errorlevel 1 goto done
cd ..
cd ground
make
if errorlevel 1 goto done
cd ..
cd gs2dex
make
if errorlevel 1 goto done
cd ..
cd gtdemo
make
if errorlevel 1 goto done
cd ..
cd lights
make
if errorlevel 1 goto done
cd ..
cd mipmap
make
if errorlevel 1 goto done
cd ..
cd morphcube
make
if errorlevel 1 goto done
cd ..
cd morphfaces
make
if errorlevel 1 goto done
cd ..
cd motortest
make
if errorlevel 1 goto done
cd ..
cd nnsample1
make
if errorlevel 1 goto done
cd ..
cd nnsample2
make
if errorlevel 1 goto done
cd ..
cd nosPak
call makenos
if errorlevel 1 goto done
cd ..
cd onetri
make
if errorlevel 1 goto done
cd ..
cd pfs
make
if errorlevel 1 goto done
cd ..
cd playseq
make
if errorlevel 1 goto done
cd ..
cd polyline
make
if errorlevel 1 goto done
cd ..
cd simple
make
if errorlevel 1 goto done
cd ..
cd soundmonkey
make
if errorlevel 1 goto done
cd ..
cd spgame
make
if errorlevel 1 goto done
cd ..
cd spritemonkey
make
if errorlevel 1 goto done
cd ..
cd sramtest
make
if errorlevel 1 goto done
cd ..
cd terrain
make
if errorlevel 1 goto done
cd ..
cd texlight
make
if errorlevel 1 goto done
cd ..
cd tile_rect2d
make
if errorlevel 1 goto done
cd ..
cd tile_rect3d
make
if errorlevel 1 goto done
cd ..
cd topgun
make
if errorlevel 1 goto done
cd ..
cd tron
make
if errorlevel 1 goto done
cd ..
cd turbomonkey
make
if errorlevel 1 goto done
cd ..
cd hostio
make
if "%BCC%"=="" goto skip_bcc
cd borland
make
cd ..
:skip_bcc
if "%MSVC%"=="" goto skip_msvc
cd microsoft
make
cd ..
:skip_msvc
cd ..
cd host_test
make
if "%BCC%"=="" goto 1skip_bcc
cd borland
make
if errorlevel 1 goto done
cd ..
:1skip_bcc
if "%MSVC%"=="" goto 1skip_msvc
cd microsoft
make
if errorlevel 1 goto done
cd ..
:1skip_msvc
cd ..
:done
