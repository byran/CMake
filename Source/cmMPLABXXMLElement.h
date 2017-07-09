#ifndef cmLocalMPLABXXMLElement_h
#define cmLocalMPLABXXMLElement_h

#include "cmXMLWriter.h"

class XMLElement
{
private:
  cmXMLWriter& xml;
  bool endCalled{ false };
public:
  XMLElement(cmXMLWriter& xml, char const* name)
    : xml{ xml }
  {
    xml.StartElement(name);
  }

  XMLElement(cmXMLWriter* xml_ptr, char const* name)
    : xml{ *xml_ptr }
  {
    xml.StartElement(name);
  }

  XMLElement(XMLElement const &) = delete;
  XMLElement& operator=(XMLElement const &) = delete;

  ~XMLElement()
  {
    if (!endCalled)
    {
      EndElement();
    }
  }

  void EndElement()
  {
    endCalled = true;
    xml.EndElement();
  }

  static void WriteWithContent(cmXMLWriter& xml, const char *elementName,
    const char *content)
  {
    XMLElement element{ xml, elementName };
    if (content)
    {
      xml.Content(content);
    }
  }
};

#endif
