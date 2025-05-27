Launcher
--------

The Launcher module adds an icon to the current virtual desktop in your
amiwm config file.  The given command / command line is run when the
icon is double clicked.

Example
-------

Screen "Screen 1"
Module "Launcher" "(xterm) (def_tool.info) (xterm -bg black -fg white)"

Screen "Screen 2"
Module "Launcher" "(xterm) (def_tool.info) (xterm -bg black -fg white)"
Module "Launcher" "(kicad) (def_tool.info) (kicad)"

Each invocation of Launcher controls a single icon/application.
