using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Reflection;

namespace tools.ui.windows_forms
{
  namespace CodeSnippets
  {
    public static class Attributes
    {
      public static bool Text(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Text"] != null)
        {
          writer.Write("      {0}.Text = \"{1}\";\n", varname, node.Attributes["Text"].InnerText);
          return true;
        }
        return false;
      }
      public static bool Icon(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Icon"] != null)
        {
          writer.Write("      {0}.SmallImage = Image.FromFile(\"{1}\");\n", varname, node.Attributes["Icon"].InnerText.Trim());
          return true;
        }
        return false;
      }
      public static bool Image(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Image"] != null)
        {
          writer.Write("      {0}.Image = Image.FromFile(\"{1}\");\n", varname, node.Attributes["Image"].InnerText.Trim());
          return true;
        }
        return false;
      }
      public static bool SizeMode(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Size"] != null)
        {
          switch (node.Attributes["Size"].InnerText.Trim())
          {
            case "Icon":
              writer.Write("      {0}.MinSizeMode = {0}.MaxSizeMode = RibbonElementSizeMode.Compact;\n", varname);
              break;
            case "Small":
              writer.Write("      {0}.MinSizeMode = {0}.MaxSizeMode = RibbonElementSizeMode.Medium;\n", varname);
              break;
            case "Large":
              writer.Write("      {0}.MinSizeMode = {0}.MaxSizeMode = RibbonElementSizeMode.Large;\n", varname);
              break;
          }
          return true;
        }
        writer.Write("      {0}.MinSizeMode = {0}.MaxSizeMode = RibbonElementSizeMode.Medium;\n", varname);
        return false;
      }
      public static bool Flow(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Flow"] != null)
        {
          switch (node.Attributes["Flow"].InnerText.Trim())
          {
            case "Horizontal":
              writer.Write("      {0}.FlowsTo = RibbonPanelFlowDirection.Right;\n", varname);
              break;
            case "Vertical":
              writer.Write("      {0}.FlowsTo = RibbonPanelFlowDirection.Bottom;\n", varname);
              break;
          }
          return true;
        }
        return false;
      }
    }
    public static class Events
    {
      public static bool Click(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["OnClick"] != null)
        {
          writer.Write("      {0}.Click += EventCommandProxy.Create(this.server, \"{1}\");\n", varname, node.Attributes["OnClick"].InnerText.Trim());
          return true;
        }
        return false;
      }
    }    
  }
}
