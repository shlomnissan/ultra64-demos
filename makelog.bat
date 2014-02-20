cd autofill
make > %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd blockmonkey
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd bumpmap
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd chrome
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd ci8fb
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd detail
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd eeptest
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd fault
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd fogworld
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd ginv
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd ground
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd gs2dex
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd gtdemo
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd lights
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd mipmap
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd morphcube
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd morphfaces
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd motortest
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd nnsample1
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd nnsample2
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd nosPak
call makenos >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd onetri
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd pfs
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd playseq
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd polyline
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd simple
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd soundmonkey
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd spgame
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd spritemonkey
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd sramtest
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd terrain
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd texlight
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd tile_rect2d
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd tile_rect3d
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd topgun
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd tron
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd turbomonkey
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
cd hostio
make >> %ROOT%\usr\src\pr\demos\appslog
if "%BCC%"=="" goto skip_bcc
cd borland
make >> %ROOT%\usr\src\pr\demos\appslog
cd ..
:skip_bcc
if "%MSVC%"=="" goto skip_msvc
cd microsoft
make >> %ROOT%\usr\src\pr\demos\appslog
cd ..
:skip_msvc
cd ..
cd host_test
make >> %ROOT%\usr\src\pr\demos\appslog
if "%BCC%"=="" goto 1skip_bcc
cd borland
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
:1skip_bcc
if "%MSVC%"=="" goto 1skip_msvc
cd microsoft
make >> %ROOT%\usr\src\pr\demos\appslog
if errorlevel 1 goto done
cd ..
:1skip_msvc
cd ..
:done
