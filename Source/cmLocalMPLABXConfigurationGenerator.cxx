#include "cmLocalMPLABXConfigurationGenerator.h"

#include "cmXMLWriter.h"
#include "cmMPLABXXMLElement.h"

cmLocalMPLABXConfigurationGenerator::cmLocalMPLABXConfigurationGenerator()
{
  rootFolder.displayName = "root";
  rootFolder.projectFiles = true;

  rootFolder.subFolders["HeaderFiles"].displayName = "Header Files";
  rootFolder.subFolders["HeaderFiles"].projectFiles = true;

  rootFolder.subFolders["LinkerScript"].displayName = "Linker Files";
  rootFolder.subFolders["LinkerScript"].projectFiles = true;

  rootFolder.subFolders["SourceFiles"].displayName = "Source Files";
  rootFolder.subFolders["SourceFiles"].projectFiles = true;

  rootFolder.subFolders["ExternalFiles"].displayName = "Important Files";
  rootFolder.subFolders["ExternalFiles"].projectFiles = false;
  rootFolder.subFolders["ExternalFiles"].files.push_back("Makefile");

  AddC32Configuration();
  AddC32_ARConfiguration();
  AddC32_LDConfiguration();
  AddC32_CPPConfiguration();
  AddC32GlobalConfiguration();
}

void cmLocalMPLABXConfigurationGenerator::WriteToStream(
  std::ostream& output) const
{
  cmXMLWriter xmlFile{output};
  xml = &xmlFile;

  xml->StartDocument();

  XMLElement configurationDescriptorElement(xml, "configurationDescriptor");
  xml->Attribute("version", "62");

  WriteLogicalFolders("root", rootFolder);
  WriteSourceRoots();
  WriteProjectMakeFile();
  WriteConfigurations();
}

void cmLocalMPLABXConfigurationGenerator::WriteElementAndContent(
  const char *elementName, const char *content) const
{
  XMLElement element{ xml, elementName };
  if (content)
  {
    xml->Content(content);
  }
}

void cmLocalMPLABXConfigurationGenerator::WriteLogicalFolders(
  char const * name, MLPABXConfiguration::logicalFolder const& folder) const
{
  XMLElement folderElement{ xml, "logicalFolder" };
  xml->Attribute("name", name);
  xml->Attribute("displayName", folder.displayName);
  xml->Attribute("projectFiles", folder.projectFiles ? "true" : "false");

  for(auto& subFolder : folder.subFolders)
  {
    WriteLogicalFolders(subFolder.first.c_str(), subFolder.second);
  }

  for(auto& file : folder.files)
  {
    XMLElement fileElement{ xml, "itemPath" };
    xml->Content(file.c_str());
  }
}

void cmLocalMPLABXConfigurationGenerator::WriteSourceRoots() const
{
  XMLElement sourceRootListElement{ xml, "sourceRootList" };

  for(auto& root : sourceRoots)
  {
    XMLElement rootElement{ xml, "Elem" };
    xml->Content(root.c_str());
  }
}

void cmLocalMPLABXConfigurationGenerator::WriteProjectMakeFile() const
{
  XMLElement projectmakefileElement{ xml, "projectmakefile" };
  xml->Content("Makefile");
}


void cmLocalMPLABXConfigurationGenerator::WriteConfigurations() const
{
  XMLElement confsElement{ xml, "confs" };

  XMLElement confElement{ xml, "conf" };
  xml->Attribute("name", "default");

  switch(targetType)
  {
  case cmStateEnums::TargetType::STATIC_LIBRARY:
    xml->Attribute("type", "3");
    break;

  case cmStateEnums::TargetType::EXECUTABLE:
  default:
    xml->Attribute("type", "2");
    break;
  }

  WriteToolsSet();
  WriteCompileType();
  WriteMakeCustomizationType();
  WriteToolsConfigurations();
}

void cmLocalMPLABXConfigurationGenerator::WriteToolsSet() const
{
  XMLElement toolsetElement{ xml, "toolsSet" };

  WriteElementAndContent("developmentServer", "localhost");
  WriteElementAndContent("targetDevice" ,targetDevice.c_str());
  WriteElementAndContent("targetHeader", nullptr);
  WriteElementAndContent("targetPluginBoard" ,nullptr);
  WriteElementAndContent("platformTool" ,"Simulator");
  WriteElementAndContent("languageToolchain", languageToolchain.c_str());
  WriteElementAndContent("languageToolchainVersion",
    languageToolchainVersion.c_str());
  WriteElementAndContent("platform" ,"3");
}

void cmLocalMPLABXConfigurationGenerator::WriteCompileType() const
{
  XMLElement compileTypeElement{ xml, "compileType" };

  {
    XMLElement linkerToolElement{ xml, "linkerTool" };
    XMLElement linkerLibItemsElement{ xml, "linkerLibItems" };
    for(auto& library : libraries)
    {
      WriteLinkerLibrary(library);
    }
  }

  WriteElementAndContent("archiverTool", nullptr);

  {
    XMLElement loadingElement{ xml, "loading" };

    WriteElementAndContent("useAlternateLoadableFile", "false");
    WriteElementAndContent("parseOnProdLoad", "false");
    WriteElementAndContent("alternateLoadableFile", nullptr);
  }
}

void cmLocalMPLABXConfigurationGenerator::WriteLinkerLibrary(
  MLPABXConfiguration::library const& library) const
{
  if(!library.targetLibrary)
  {
    XMLElement linkerLibProjectItemElement{xml, "linkerLibFileItem"};
    xml->Content(library.path);
  }
  else
  {
    XMLElement linkerLibProjectItemElement{xml, "linkerLibProjectItem"};

    XMLElement makeArtifactElement{xml, "makeArtifact"};

    std::string libraryPath = library.path + "/" + library.name + ".X";

    xml->Attribute("PL", libraryPath.c_str());
    xml->Attribute("CT", "3");
    xml->Attribute("CN", "default");
    xml->Attribute("AC", "true");
    xml->Attribute("BL", "true");
    xml->Attribute("WD", libraryPath.c_str());
    xml->Attribute("BC", "${MAKE}  -f Makefile CONF=default");
    xml->Attribute("DBC",
                   "${MAKE}  -f Makefile CONF=default TYPE_IMAGE=DEBUG_RUN");
    xml->Attribute("CC",
                   "rm -rf \"build/default\" \"dist/default\"");

    std::string value = "dist/default/production/";
    value += library.name + ".X.a";
    xml->Attribute("OP", value.c_str());

    value = "dist/default/debug/";
    value += library.name + ".X.a";
    xml->Attribute("DOP", value.c_str());

    value = "dist/default/production/";
    value += library.name + ".X.a";
    xml->Attribute("FL", value.c_str());

    value = "dist/default/production/";
    value += library.name + ".X.";
    xml->Attribute("PD", value.c_str());

    value = "dist/default/debug/";
    value += library.name + ".X.";
    xml->Attribute("DD", value.c_str());
  }
}

void cmLocalMPLABXConfigurationGenerator::WriteMakeCustomizationType() const
{
  XMLElement makeCustomizationTypeElement{ xml, "makeCustomizationType" };
  WriteElementAndContent("makeCustomizationPreStepEnabled", "false");
  WriteElementAndContent("makeCustomizationPreStep", nullptr);
  WriteElementAndContent("makeCustomizationPostStepEnabled", "false");
  WriteElementAndContent("makeCustomizationPostStep", nullptr);
  WriteElementAndContent("makeCustomizationPutChecksumInUserID", "false");
  WriteElementAndContent("makeCustomizationEnableLongLines", "false");
  WriteElementAndContent("makeCustomizationNormalizeHexFile", "false");
}

void cmLocalMPLABXConfigurationGenerator::WriteToolsConfigurations() const
{
  for(auto& toolConfiguration : toolsConfiguration)
  {
    XMLElement toolElement{ xml, toolConfiguration.first.c_str() };

    for(auto& property : toolConfiguration.second)
    {
      XMLElement propertyElement{ xml, "property" };
      xml->Attribute("key", property.first.c_str());
      xml->Attribute("value", property.second.c_str());
    }
  }
}

void cmLocalMPLABXConfigurationGenerator::AddC32Configuration()
{
  auto& config = toolsConfiguration["C32"];

  config["additional-warnings"] = "false";
  config["enable-app-io"] = "false";
  config["enable-omit-frame-pointer"] = "false";
  config["enable-symbols"] = "true";
  config["enable-unroll-loops"] = "false";
  config["exclude-floating-point"] = "false";
  config["extra-include-directories"] = "";
  config["generate-16-bit-code"] = "false";
  config["generate-micro-compressed-code"] = "false";
  config["isolate-each-function"] = "false";
  config["make-warnings-into-errors"] = "false";
  config["optimization-level"] = "";
  config["place-data-into-section"] = "false";
  config["post-instruction-scheduling"] = "default";
  config["pre-instruction-scheduling"] = "default";
  config["preprocessor-macros"] = "";
  config["strict-ansi"] = "false";
  config["support-ansi"] = "false";
  config["use-cci"] = "false";
  config["use-iar"] = "false";
  config["use-indirect-calls"] = "false";
}

void cmLocalMPLABXConfigurationGenerator::AddC32_ARConfiguration()
{
  auto& config = toolsConfiguration["C32-AR"];

  config["additional-options-chop-files"] = "false";
}

void cmLocalMPLABXConfigurationGenerator::AddC32_LDConfiguration()
{
  auto& config = toolsConfiguration["C32-LD"];

  config["additional-options-use-response-files"] = "false";
  config["enable-check-sections"] = "false";
  config["exclude-floating-point-library"] = "false";
  config["exclude-standard-libraries"] = "false";
  config["extra-lib-directories"] = "";
  config["fill-flash-options-addr"] = "";
  config["fill-flash-options-const"] = "";
  config["fill-flash-options-how"] = "0";
  config["fill-flash-options-inc-const"] = "1";
  config["fill-flash-options-increment"] = "";
  config["fill-flash-options-seq"] = "";
  config["fill-flash-options-what"] = "0";
  config["generate-16-bit-code"] = "false";
  config["generate-cross-reference-file"] = "false";
  config["generate-micro-compressed-code"] = "false";
  config["heap-size"] = "";
  config["input-libraries"] = "";
  config["linker-symbols"] = "";
  config["map-file"] = "${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map";
  config["no-startup-files"] = "false";
  config["oXC32ld-extra-opts"] = "";
  config["optimization-level"] = "";
  config["preprocessor-macros"] = "";
  config["remove-unused-sections"] = "false";
  config["report-memory-usage"] = "false";
  config["stack-size"] = "";
  config["symbol-stripping"] = "";
  config["trace-symbols"] = "";
  config["warn-section-align"] = "false";
}

void cmLocalMPLABXConfigurationGenerator::AddC32_CPPConfiguration()
{
  auto& config = toolsConfiguration["C32CPP"];

  config["additional-warnings"] = "false";
  config["check-new"] = "false";
  config["eh-specs"] = "false";
  config["enable-app-io"] = "false";
  config["enable-omit-frame-pointer"] = "false";
  config["enable-symbols"] = "true";
  config["enable-unroll-loops"] = "false";
  config["exceptions"] = "false";
  config["exclude-floating-point"] = "false";
  config["extra-include-directories"] = "";
  config["generate-16-bit-code"] = "false";
  config["generate-micro-compressed-code"] = "false";
  config["isolate-each-function"] = "false";
  config["make-warnings-into-errors"] = "false";
  config["optimization-level"] = "";
  config["place-data-into-section"] = "false";
  config["post-instruction-scheduling"] = "default";
  config["pre-instruction-scheduling"] = "default";
  config["preprocessor-macros"] = "";
  config["rtti"] = "false";
  config["strict-ansi"] = "false";
  config["use-cci"] = "false";
  config["use-iar"] = "false";
  config["use-indirect-calls"] = "false";
}

void cmLocalMPLABXConfigurationGenerator::AddC32GlobalConfiguration()
{
  auto& config = toolsConfiguration["C32Global"];

  config["common-include-directories"] = "";
  config["gp-relative-option"] = "";
  config["legacy-libc"] = "false";
  config["relaxed-math"] = "false";
  config["save-temps"] = "false";
  config["wpo-lto"] = "false";
}

void cmLocalMPLABXConfigurationGenerator::AddRealICEConfiguration()
{
  auto& config = toolsConfiguration["RealICEPlatformTool"];

  config["ADC 1"] = "true";
  config["AutoSelectMemRanges"] = "auto";
  config["CAN1"] = "true";
  config["CHANGE NOTICE"] = "true";
  config["COMPARATOR"] = "true";
  config["DMA"] = "true";
  config["Freeze All Other Peripherals"] = "true";
  config["I2C1"] = "true";
  config["I2C3"] = "true";
  config["I2C4"] = "true";
  config["I2C5"] = "true";
  config["INPUT CAPTURE 1"] = "true";
  config["INPUT CAPTURE 2"] = "true";
  config["INPUT CAPTURE 3"] = "true";
  config["INPUT CAPTURE 4"] = "true";
  config["INPUT CAPTURE 5"] = "true";
  config["INTERRUPT CONTROL"] = "true";
  config["OUTPUT COMPARE 1"] = "true";
  config["OUTPUT COMPARE 2"] = "true";
  config["OUTPUT COMPARE 3"] = "true";
  config["OUTPUT COMPARE 4"] = "true";
  config["OUTPUT COMPARE 5"] = "true";
  config["PARALLEL MASTER/SLAVE PORT"] = "true";
  config["REAL TIME CLOCK"] = "true";
  config["RIExTrigs.Five"] = "OFF";
  config["RIExTrigs.Four"] = "OFF";
  config["RIExTrigs.One"] = "OFF";
  config["RIExTrigs.Seven"] = "OFF";
  config["RIExTrigs.Six"] = "OFF";
  config["RIExTrigs.Three"] = "OFF";
  config["RIExTrigs.Two"] = "OFF";
  config["RIExTrigs.Zero"] = "OFF";
  config["SPI 2"] = "true";
  config["SPI 3"] = "true";
  config["SPI 4"] = "true";
  config["SecureSegment.SegmentProgramming"] = "FullChipProgramming";
  config["TIMER1"] = "true";
  config["TIMER2"] = "true";
  config["TIMER3"] = "true";
  config["TIMER4"] = "true";
  config["TIMER5"] = "true";
  config["ToolFirmwareFilePath"] =
    "Press to browse for a specific firmware version";
  config["ToolFirmwareOption.UseLatestFirmware"] = "true";
  config["UART1"] = "true";
  config["UART2"] = "true";
  config["UART3"] = "true";
  config["UART4"] = "true";
  config["UART5"] = "true";
  config["UART6"] = "true";
  config["USB"] = "true";
  config["debugoptions.useswbreakpoints"] = "false";
  config["hwtoolclock.frcindebug"] = "false";
  config["hwtoolclock.instructionspeed"] = "4";
  config["hwtoolclock.units"] = "mips";
  config["memories.aux"] = "false";
  config["memories.bootflash"] = "true";
  config["memories.configurationmemory"] = "true";
  config["memories.configurationmemory2"] = "true";
  config["memories.dataflash"] = "true";
  config["memories.eeprom"] = "true";
  config["memories.flashdata"] = "true";
  config["memories.id"] = "true";
  config["memories.programmemory"] = "true";
  config["memories.programmemory.end"] = "0x1d07ffff";
  config["memories.programmemory.partition2"] = "true";
  config["memories.programmemory.partition2.end"] =
    "${memories.programmemory.partition2.end.value}";
  config["memories.programmemory.partition2.start"] =
    "${memories.programmemory.partition2.start.value}";
  config["memories.programmemory.start"] = "0x1d000000";
  config["poweroptions.powerenable"] = "false";
  config["programoptions.donoteraseauxmem"] = "false";
  config["programoptions.eraseb4program"] = "true";
  config["programoptions.preservedataflash"] = "false";
  config["programoptions.preserveeeprom"] = "false";
  config["programoptions.preserveprogramrange"] = "false";
  config["programoptions.preserveprogramrange.end"] = "0x1d07ffff";
  config["programoptions.preserveprogramrange.start"] = "0x1d000000";
  config["programoptions.preserveuserid"] = "false";
  config["programoptions.programcalmem"] = "false";
  config["programoptions.programuserotp"] = "false";
  config["programoptions.usehighvoltageonmclr"] = "false";
  config["programoptions.uselvpprogramming"] = "false";
  config["tracecontrol.include.timestamp"] = "summarydataenabled";
  config["tracecontrol.select"] = "0";
  config["tracecontrol.stallontracebufferfull"] = "false";
  config["tracecontrol.timestamp"] = "0";
  config["tracecontrol.tracebufmax"] = "546000";
  config["tracecontrol.tracefile"] = "defmplabxtrace.log";
  config["tracecontrol.traceresetonrun"] = "false";
  config["voltagevalue"] = "3.25";
}

void cmLocalMPLABXConfigurationGenerator::AddSimulatorConfiguration()
{
  auto& config = toolsConfiguration["Simulator"];

  config["codecoverage.enabled"] = "Disable";
  config["codecoverage.enableoutputtofile"] = "false";
  config["codecoverage.outputfile"] = "";
  config["oscillator.auxfrequency"] = "120";
  config["oscillator.auxfrequencyunit"] = "Mega";
  config["oscillator.frequency"] = "1";
  config["oscillator.frequencyunit"] = "Mega";
  config["oscillator.rcfrequency"] = "250";
  config["oscillator.rcfrequencyunit"] = "Kilo";
  config["performancedata.show"] = "false";
  config["periphADC1.altscl"] = "false";
  config["periphADC1.minTacq"] = "132";
  config["periphADC1.tacqunits"] = "microseconds";
  config["periphADC2.altscl"] = "false";
  config["periphADC2.minTacq"] = "";
  config["periphADC2.tacqunits"] = "microseconds";
  config["periphComp1.gte"] = "gt";
  config["periphComp2.gte"] = "gt";
  config["periphComp3.gte"] = "gt";
  config["periphComp4.gte"] = "gt";
  config["periphComp5.gte"] = "gt";
  config["periphComp6.gte"] = "gt";
  config["reset.scl"] = "false";
  config["reset.type"] = "MCLR";
  config["tracecontrol.include.timestamp"] = "summarydataenabled";
  config["tracecontrol.select"] = "0";
  config["tracecontrol.stallontracebufferfull"] = "false";
  config["tracecontrol.timestamp"] = "0";
  config["tracecontrol.tracebufmax"] = "546000";
  config["tracecontrol.tracefile"] = "defmplabxtrace.log";
  config["tracecontrol.traceresetonrun"] = "false";
  config["uart10io.output"] = "window";
  config["uart10io.outputfile"] = "";
  config["uart10io.uartioenabled"] = "false";
  config["uart1io.output"] = "window";
  config["uart1io.outputfile"] = "";
  config["uart1io.uartioenabled"] = "false";
  config["uart2io.output"] = "window";
  config["uart2io.outputfile"] = "";
  config["uart2io.uartioenabled"] = "false";
  config["uart3io.output"] = "window";
  config["uart3io.outputfile"] = "";
  config["uart3io.uartioenabled"] = "false";
  config["uart4io.output"] = "window";
  config["uart4io.outputfile"] = "";
  config["uart4io.uartioenabled"] = "false";
  config["uart5io.output"] = "window";
  config["uart5io.outputfile"] = "";
  config["uart5io.uartioenabled"] = "false";
  config["uart6io.output"] = "window";
  config["uart6io.outputfile"] = "";
  config["uart6io.uartioenabled"] = "false";
  config["uart7io.output"] = "window";
  config["uart7io.outputfile"] = "";
  config["uart7io.uartioenabled"] = "false";
  config["uart8io.output"] = "window";
  config["uart8io.outputfile"] = "";
  config["uart8io.uartioenabled"] = "false";
  config["uart9io.output"] = "window";
  config["uart9io.outputfile"] = "";
  config["uart9io.uartioenabled"] = "false";
  config["warningmessagebreakoptions.W0001_CORE_BITREV_MODULO_EN"] = "report";
  config["warningmessagebreakoptions.W0002_CORE_SECURE_MEMORYACCESS"] =
    "report";
  config["warningmessagebreakoptions.W0003_CORE_SW_RESET"] = "report";
  config["warningmessagebreakoptions.W0004_CORE_WDT_RESET"] = "report";
  config["warningmessagebreakoptions.W0005_CORE_IOPUW_RESET"] = "report";
  config["warningmessagebreakoptions.W0006_CORE_CODE_GUARD_PFC_RESET"] =
    "report";
  config["warningmessagebreakoptions.W0007_CORE_DO_LOOP_STACK_UNDERFLOW"] =
    "report";
  config["warningmessagebreakoptions.W0008_CORE_DO_LOOP_STACK_OVERFLOW"] =
    "report";
  config["warningmessagebreakoptions.W0009_CORE_NESTED_DO_LOOP_RANGE"] =
    "report";
  config["warningmessagebreakoptions.W0010_CORE_SIM32_ODD_WORDACCESS"] =
    "report";
  config["warningmessagebreakoptions."
    "W0011_CORE_SIM32_UNIMPLEMENTED_RAMACCESS"] = "report";
  config["warningmessagebreakoptions.W0012_CORE_STACK_OVERFLOW_RESET"] =
    "report";
  config["warningmessagebreakoptions.W0013_CORE_STACK_UNDERFLOW_RESET"] =
    "report";
  config["warningmessagebreakoptions.W0014_CORE_INVALID_OPCODE"] = "report";
  config["warningmessagebreakoptions.W0015_CORE_INVALID_ALT_WREG_SET"] =
    "report";
  config["warningmessagebreakoptions.W0101_SIM_UPDATE_FAILED"] = "report";
  config["warningmessagebreakoptions.W0102_SIM_PERIPH_MISSING"] = "report";
  config["warningmessagebreakoptions.W0103_SIM_PERIPH_FAILED"] = "report";
  config["warningmessagebreakoptions.W0104_SIM_FAILED_TO_INIT_TOOL"] =
    "report";
  config["warningmessagebreakoptions.W0105_SIM_INVALID_FIELD"] = "report";
  config["warningmessagebreakoptions.W0106_SIM_PERIPH_PARTIAL_SUPPORT"] =
    "report";
  config["warningmessagebreakoptions.W0201_ADC_NO_STIMULUS_FILE"] = "report";
  config["warningmessagebreakoptions.W0202_ADC_GO_DONE_BIT"] = "report";
  config["warningmessagebreakoptions.W0203_ADC_MINIMUM_2_TAD"] = "report";
  config["warningmessagebreakoptions.W0204_ADC_TAD_TOO_SMALL"] = "report";
  config["warningmessagebreakoptions.W0205_ADC_UNEXPECTED_TRANSITION"] =
    "report";
  config["warningmessagebreakoptions.W0206_ADC_SAMP_TIME_TOO_SHORT"] =
    "report";
  config["warningmessagebreakoptions.W0207_ADC_NO_PINS_SCANNED"] = "report";
  config["warningmessagebreakoptions.W0208_ADC_UNSUPPORTED_CLOCK_SOURCE"] =
    "report";
  config["warningmessagebreakoptions.W0209_ADC_ANALOG_CHANNEL_DIGITAL"] =
    "report";
  config["warningmessagebreakoptions.W0210_ADC_ANALOG_CHANNEL_OUTPUT"] =
    "report";
  config["warningmessagebreakoptions.W0211_ADC_PIN_INVALID_CHANNEL"] =
    "report";
  config["warningmessagebreakoptions.W0212_ADC_BAND_GAP_NOT_SUPPORTED"] =
    "report";
  config["warningmessagebreakoptions.W0213_ADC_RESERVED_SSRC"] = "report";
  config["warningmessagebreakoptions.W0214_ADC_POSITIVE_INPUT_DIGITAL"] =
    "report";
  config["warningmessagebreakoptions.W0215_ADC_POSITIVE_INPUT_OUTPUT"] =
    "report";
  config["warningmessagebreakoptions.W0216_ADC_NEGATIVE_INPUT_DIGITAL"] =
    "report";
  config["warningmessagebreakoptions.W0217_ADC_NEGATIVE_INPUT_OUTPUT"] =
    "report";
  config["warningmessagebreakoptions.W0218_ADC_REFERENCE_HIGH_DIGITAL"] =
    "report";
  config["warningmessagebreakoptions.W0219_ADC_REFERENCE_HIGH_OUTPUT"] =
    "report";
  config["warningmessagebreakoptions.W0220_ADC_REFERENCE_LOW_DIGITAL"] =
    "report";
  config["warningmessagebreakoptions.W0221_ADC_REFERENCE_LOW_OUTPUT"] =
    "report";
  config["warningmessagebreakoptions.W0222_ADC_OVERFLOW"] = "report";
  config["warningmessagebreakoptions.W0223_ADC_UNDERFLOW"] = "report";
  config["warningmessagebreakoptions.W0224_ADC_CTMU_NOT_SUPPORTED"] =
    "report";
  config["warningmessagebreakoptions.W0225_ADC_INVALID_CH0S"] = "report";
  config["warningmessagebreakoptions.W0226_ADC_VBAT_NOT_SUPPORTED"] = "report";
  config["warningmessagebreakoptions.W0227_ADC_INVALID_ADCS"] = "report";
  config["warningmessagebreakoptions.W0228_ADC_INVALID_ADCS"] = "report";
  config["warningmessagebreakoptions.W0229_ADC_INVALID_ADCS"] = "report";
  config["warningmessagebreakoptions.W0230_ADC_TRIGSEL_NOT_SUPPORTED"] =
    "report";
  config["warningmessagebreakoptions.W0400_PWM_PWM_FASTER_THAN_FOSC"] =
    "report";
  config["warningmessagebreakoptions.W0700_CLC_GENERAL_WARNING"] = "report";
  config["warningmessagebreakoptions.W0701_CLC_CLCOUT_AS_INPUT"] = "report";
  config["warningmessagebreakoptions.W0702_CLC_CIRCULAR_LOOP"] = "report";
  config["warningmessagebreakoptions.W1201_DATAFLASH_MEM_OUTSIDE_RANGE"] =
    "report";
  config["warningmessagebreakoptions.W1202_DATAFLASH_ERASE_WHILE_LOCKED"] =
    "report";
  config["warningmessagebreakoptions.W1203_DATAFLASH_WRITE_WHILE_LOCKED"] =
    "report";
  config["warningmessagebreakoptions.W1401_DMA_PERIPH_NOT_AVAIL"] = "report";
  config["warningmessagebreakoptions.W1402_DMA_INVALID_IRQ"] = "report";
  config["warningmessagebreakoptions.W1403_DMA_INVALID_SFR"] = "report";
  config["warningmessagebreakoptions.W1404_DMA_INVALID_DMA_ADDR"] = "report";
  config["warningmessagebreakoptions.W1405_DMA_IRQ_DIR_MISMATCH"] = "report";
  config["warningmessagebreakoptions.W1600_PPS_INVALID_MAP"] = "report";
  config["warningmessagebreakoptions.W2001_INPUTCAPTURE_TMR3_UNAVAILABLE"] =
    "report";
  config["warningmessagebreakoptions.W2002_INPUTCAPTURE_CAPTURE_EMPTY"] =
    "report";
  config["warningmessagebreakoptions."
    "W2003_INPUTCAPTURE_SYNCSEL_NOT_AVIALABLE"] = "report";
  config["warningmessagebreakoptions.W2004_INPUTCAPTURE_BAD_SYNC_SOURCE"] =
    "report";
  config["warningmessagebreakoptions."
    "W2501_OUTPUTCOMPARE_SYNCSEL_NOT_AVIALABLE"] = "report";
  config["warningmessagebreakoptions.W2502_OUTPUTCOMPARE_BAD_SYNC_SOURCE"] =
    "report";
  config["warningmessagebreakoptions.W2503_OUTPUTCOMPARE_BAD_TRIGGER_SOURCE"] =
    "report";
  config["warningmessagebreakoptions.W9001_TMR_GATE_AND_EXTCLOCK_ENABLED"] =
    "report";
  config["warningmessagebreakoptions.W9002_TMR_NO_PIN_AVAILABLE"] = "report";
  config["warningmessagebreakoptions.W9003_TMR_INVALID_CLOCK_SOURCE"] =
    "report";
  config["warningmessagebreakoptions.W9201_UART_TX_OVERFLOW"] = "report";
  config["warningmessagebreakoptions.W9202_UART_TX_CAPTUREFILE"] = "report";
  config["warningmessagebreakoptions.W9203_UART_TX_INVALIDINTERRUPTMODE"] =
    "report";
  config["warningmessagebreakoptions.W9204_UART_RX_EMPTY_QUEUE"] = "report";
  config["warningmessagebreakoptions.W9205_UART_TX_BADFILE"] = "report";
  config["warningmessagebreakoptions.W9401_CVREF_INVALIDSOURCESELECTION"] =
    "report";
  config["warningmessagebreakoptions.W9402_CVREF_INPUT_OUTPUTPINCONFLICT"] =
    "report";
  config["warningmessagebreakoptions.W9601_COMP_FVR_SOURCE_UNAVAILABLE"] =
    "report";
  config["warningmessagebreakoptions.W9602_COMP_DAC_SOURCE_UNAVAILABLE"] =
    "report";
  config["warningmessagebreakoptions.W9603_COMP_CVREF_SOURCE_UNAVAILABLE"] =
    "report";
  config["warningmessagebreakoptions.W9801_FVR_INVALID_MODE_SELECTION"] =
    "report";
  config["warningmessagebreakoptions.W9801_SCL_BAD_SUBTYPE_INDICATION"] =
    "report";
  config["warningmessagebreakoptions.W9802_SCL_FILE_NOT_FOUND"] = "report";
  config["warningmessagebreakoptions.W9803_SCL_FAILED_TO_READ_FILE"] =
    "report";
  config["warningmessagebreakoptions.W9804_SCL_UNRECOGNIZED_LABEL"] = "report";
  config["warningmessagebreakoptions.W9805_SCL_UNRECOGNIZED_VAR"] = "report";
  config["warningmessagebreakoptions.displaywarningmessagesoption"] = "";
  config["warningmessagebreakoptions.warningmessages"] = "holdstate";
}
