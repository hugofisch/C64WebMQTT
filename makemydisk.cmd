@echo off
echo Contiki Entwicklungsumgebung gesetzt %PATHCONTIKI%
copy .\emptyD64\contiki.d64 .
copy .\webserver-example.c64 contiki

%C1541% -attach contiki.d64 -delete contiki -write contiki contiki,p
%C1541% -attach contiki.d64 -delete cs8900a.eth -write cs8900a.eth cs8900a.eth,u
%C1541% -attach contiki.d64 -delete lan91c96.eth -write lan91c96.eth lan91c96.eth,u
%C1541% -attach contiki.d64 -delete contiki.cfg -write contiki.cfg contiki.cfg,u
pause