/* Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
   file Copyright.txt or https://cmake.org/licensing for details.  */
#ifndef cmExtraMPLABXGenerator_h
#define cmExtraMPLABXGenerator_h

#include <cmConfigure.h>

#include "cmExternalMakefileProjectGenerator.h"

#include <string>
#include <vector>

class cmGeneratorTarget;
class cmLocalGenerator;
class cmMakefile;
class cmXMLWriter;

/** \class cmExtraMPLABXGenerator
 * \brief Write MPLAB project files for Makefile based projects
 */
class cmExtraMPLABXGenerator : public cmExternalMakefileProjectGenerator
{
public:
  cmExtraMPLABXGenerator();

  static cmExternalMakefileProjectGeneratorFactory* GetFactory();

  void Generate() CM_OVERRIDE;

private:
  struct CbpUnit
  {
    std::vector<const cmGeneratorTarget*> Targets;
  };

  void CreateProjectFile(const std::vector<cmLocalGenerator*>& lgs);

  void CreateNewProjectFile(const std::vector<cmLocalGenerator*>& lgs,
                            const std::string& filename);
};

#endif
