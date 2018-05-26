
find_program(CMAKE_C_COMPILER
  NAMES xc32-gcc
  PATHS
  "C:/Program Files (x86)/Microchip/xc32/v${XC32_VERSION}/bin"
  "/opt/microchip/xc32/v${XC32_VERSION}/bin"
  NO_DEFAULT_PATH)
mark_as_advanced(CMAKE_C_COMPILER)

find_program(CMAKE_CXX_COMPILER
  NAMES xc32-g++
  PATHS
  "C:/Program Files (x86)/Microchip/xc32/v${XC32_VERSION}/bin"
  "/opt/microchip/xc32/v${XC32_VERSION}/bin"
  NO_DEFAULT_PATH)
mark_as_advanced(CMAKE_CXX_COMPILER)

find_program(CMAKE_AR
        NAMES xc32-ar
        PATHS
        "C:/Program Files (x86)/Microchip/xc32/v${XC32_VERSION}/bin"
        "/opt/microchip/xc32/v${XC32_VERSION}/bin"
        NO_DEFAULT_PATH)
mark_as_advanced(XC32_AR)

find_program(XC32_BIN2HEX
        NAMES xc32-bin2hex
        PATHS
        "C:/Program Files (x86)/Microchip/xc32/v${XC32_VERSION}/bin"
        "/opt/microchip/xc32/v${XC32_VERSION}/bin"
        NO_DEFAULT_PATH)
mark_as_advanced(XC32_BIN2HEX)
