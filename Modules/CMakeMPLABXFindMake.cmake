
find_program(CMAKE_MAKE_PROGRAM
  NAMES make.exe
  PATHS "C:/Program Files (x86)/Microchip/MPLABX/v3.26/gnuBins/GnuWin32/bin"
  NO_DEFAULT_PATH)

mark_as_advanced(CMAKE_MAKE_PROGRAM)

find_program(MPLABX_PROJECT_MAKEFILES_GENERATOR
  NAMES prjMakefilesGenerator.bat
  PATHS "C:/Program Files (x86)/Microchip/MPLABX/v3.26/mplab_ide/bin"
  NO_DEFAULT_PATH)
