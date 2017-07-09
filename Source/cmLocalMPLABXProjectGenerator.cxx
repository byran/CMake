#include "cmLocalMPLABXProjectGenerator.h"

#include "cmXMLWriter.h"

#include "cmMPLABXXMLElement.h"

void cmLocalMPLABXProjectGenerator::WriteToStream(std::ostream& output) const
{
  cmXMLWriter xml{ output };
  xml.StartDocument();

  XMLElement projectElement{ xml, "project" };
  xml.Attribute("xmlns", "http://www.netbeans.org/ns/project/1");

  XMLElement::WriteWithContent(xml, "type",
    "com.microchip.mplab.nbide.embedded.makeproject");

  XMLElement configurationElement{ xml, "configuration" };
  XMLElement dataElement{ xml, "data" };
  xml.Attribute("xmlns", "http://www.netbeans.org/ns/make-project/1");

  XMLElement::WriteWithContent(xml, "name", name.c_str());
  XMLElement::WriteWithContent(xml, "make-project-type", "0");
  XMLElement::WriteWithContent(xml, "c-extensions", "c");
  XMLElement::WriteWithContent(xml, "cpp-extensions", "cpp");
  XMLElement::WriteWithContent(xml, "header-extensions", "h");
  XMLElement::WriteWithContent(xml, "asminc-extensions", nullptr);
  XMLElement::WriteWithContent(xml, "sourceEncoding", "ISO-8859-1");

  XMLElement dependentProjects{ xml, "make-dep-projects" };
  for(auto& dependency : dependencies)
  {
    XMLElement::WriteWithContent(xml, "make-dep-project", dependency.c_str());
  }
}
