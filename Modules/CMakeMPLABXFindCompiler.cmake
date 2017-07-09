
find_program(CMAKE_C_COMPILER
  NAMES xc32-gcc
  PATHS "C:/Program Files (x86)/Microchip/xc32/v${XC32_VERSION}/bin"
  NO_DEFAULT_PATH)
mark_as_advanced(CMAKE_C_COMPILER)

find_program(CMAKE_CXX_COMPILER
  NAMES xc32-g++
  PATHS "C:/Program Files (x86)/Microchip/xc32/v${XC32_VERSION}/bin"
  NO_DEFAULT_PATH)
mark_as_advanced(CMAKE_CXX_COMPILER)
