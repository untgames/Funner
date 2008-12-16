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
      public static bool TextBoxWidth(XmlNode node, StringWriter writer, string varname)
      {
        try
        {
          if (node.Attributes["Width"] != null)
          {
            if (Converter.ToInt(node.Attributes["Width"].InnerText) <= 0)
              throw new Exception("Invalid 'Width' attribute value");
            writer.Write("      {0}.TextBoxWidth = {1};\n", varname, node.Attributes["Width"].InnerText);
            return true;
          }
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
        writer.Write("      {0}.TextBoxWidth = 50;\n", varname);
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
    public static class Properties
    {
      public static string TemplateName(string container, string property)
      {
        return container + "_" + property + "_PropertyListener";
      }
      public static string Template(string container, string property, string type, string converter)
      {
        StringWriter tmpl = new StringWriter();
        tmpl.Write("  public class {0}: IPropertyListener\n", TemplateName(container, property));
        tmpl.Write("  {{\n");
        tmpl.Write("    private IApplicationServer server;\n");
        tmpl.Write("    private {0} container;\n", container);
        tmpl.Write("    private string variable;\n");
        tmpl.Write("    \n");
        tmpl.Write("    public {0} ({1} container, IApplicationServer server, string variable)\n", TemplateName(container, property), container);
        tmpl.Write("    {{\n");
        tmpl.Write("      this.server = server;\n");
        tmpl.Write("      this.variable = variable;\n");
        tmpl.Write("      this.container = container;\n");
        tmpl.Write("      this.server.RegisterPropertyListener(this.variable, this);\n");
        tmpl.Write("    }}\n");
        tmpl.Write("    public ~{0}PropertyListener()\n", property);
        tmpl.Write("    {{\n");
        tmpl.Write("      this.server.UnregisterPropertyListener(this.variable, this);\n");
        tmpl.Write("    }}\n");
        tmpl.Write("    public void OnAddProperty(string name)\n");
        tmpl.Write("    {{\n");
        tmpl.Write("      OnChangeProperty(this.variable);\n");
        tmpl.Write("    }}\n");
        tmpl.Write("    public void OnChangeProperty(string name)\n");
        tmpl.Write("    {{\n");
        tmpl.Write("      try\n");
        tmpl.Write("      {{\n");
        tmpl.Write("        string buffer;\n");
        tmpl.Write("        server.GetProperty(this.variable, buffer);\n");
        tmpl.Write("        container.{0} = Converter.{1}(buffer);\n", property, converter);
        tmpl.Write("      }}\n");
        tmpl.Write("      catch (Exception e)\n");
        tmpl.Write("      {{\n");
        tmpl.Write("        System.Console.WriteLine(e.ToString());\n");
        tmpl.Write("      }}\n");
        tmpl.Write("    }}\n");
        tmpl.Write("    public void OnUpdateProperty(object sender, EventArgs e)\n");
        tmpl.Write("    {{\n");
        tmpl.Write("      try\n");
        tmpl.Write("      {{\n");
        tmpl.Write("        server.SetProperty(this.variable, Converter.ToString(container.{0}));\n", property);
        tmpl.Write("      }}\n");
        tmpl.Write("      catch (Exception e)\n");
        tmpl.Write("      {{\n");
        tmpl.Write("        System.Console.WriteLine(e.ToString());\n");
        tmpl.Write("      }}\n");
        tmpl.Write("    }}\n");
        tmpl.Write("    public void OnRemoveProperty(string name)\n");
        tmpl.Write("    {{\n");
        tmpl.Write("      try\n");
        tmpl.Write("      {{\n");
        tmpl.Write("        container.{0} = default({1});\n", property, type);
        tmpl.Write("      }}\n");
        tmpl.Write("      catch (Exception e)\n");
        tmpl.Write("      {{\n");
        tmpl.Write("        System.Console.WriteLine(e.ToString());\n");
        tmpl.Write("      }}\n");
        tmpl.Write("    }}\n");
        tmpl.Write("  }}\n");
        return tmpl.ToString();
      }
    }
  }
}
