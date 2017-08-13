
find_program(CMAKE_MAKE_PROGRAM
  NAMES
    make.exe
    make
  PATHS
    "C:/Program Files (x86)/Microchip/MPLABX/v3.26/gnuBins/GnuWin32/bin"
    "/usr/bin"
  NO_DEFAULT_PATH)

mark_as_advanced(CMAKE_MAKE_PROGRAM)

find_program(MPLABX_PROJECT_MAKEFILES_GENERATOR
  NAMES
    prjMakefilesGenerator.bat
    prjMakefilesGenerator.sh
  PATHS
    "C:/Program Files (x86)/Microchip/MPLABX/v3.26/mplab_ide/bin"
    "/opt/microchip/mplabx/v3.65/mplab_ide/bin"
  NO_DEFAULT_PATH)
