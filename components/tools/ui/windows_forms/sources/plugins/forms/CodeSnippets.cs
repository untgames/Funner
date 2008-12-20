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
    public static class Layout
    {
      public static void Suspend(StringWriter writer, string varname)
      {
        writer.Write("      {0}.SuspendLayout();\n", varname);
      }
      public static void Resume(StringWriter writer, string varname)
      {
        writer.Write("      {0}.ResumeLayout(true);\n", varname);
      }
    }
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
      public static bool SmallImage(XmlNode node, StringWriter writer, string varname)
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
      public static bool BackgroundImage(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["BackImage"] != null)
        {
          writer.Write("      {0}.BackgroundImage = Image.FromFile(\"{1}\");\n", varname, node.Attributes["BackImage"].InnerText.Trim());
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
      public static bool FlowsTo(XmlNode node, StringWriter writer, string varname)
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
      public static bool Name(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Name"] != null)
        {
          writer.Write("      {0}.Name = \"{1}\";\n", varname, node.Attributes["Name"].InnerText.Trim());
          return true;
        }
        writer.Write("      {0}.Name = \"{0}\";\n", varname);
        return false;
      }
      public static bool ForeColor(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["ForeColor"] != null)
        {
          try
          {
            writer.Write("      {0}.ForeColor = Color.FromArgb({1:D});\n", varname, Converter.ToColor(node.Attributes["ForeColor"].InnerText).ToArgb());
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
      public static bool BackColor(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["BackColor"] != null)
        {
          try
          {
            writer.Write("      {0}.BackColor = Color.FromArgb({1:D});\n", varname, Converter.ToColor(node.Attributes["BackColor"].InnerText).ToArgb());
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
      public static bool AnchorDock(XmlNode node, StringWriter writer, string varname)
      {
        try
        {
          if (node.Attributes["Anchor"] != null && node.Attributes["Dock"] == null)
          {
            string[] items = Converter.ToAnchorStyles(node.Attributes["Anchor"].InnerText).ToString().Split(" ,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
            string buffer = "";
            foreach (string item in items)
              buffer += (string.IsNullOrEmpty(buffer) ? "AnchorStyles." : " | AnchorStyles.") + item;
            writer.Write("      {0}.Anchor = {1};\n", varname, buffer);
            return true;
          }
          if (node.Attributes["Anchor"] == null && node.Attributes["Dock"] != null)
          {
            writer.Write("      {0}.Dock = DockStyle.{1});\n", varname, Converter.ToDockStyle(node.Attributes["Dock"].InnerText).ToString());
            return true;
          }
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
        return false;
      }
      public static bool Size(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Size"] != null)
        {
          try
          {
            System.Drawing.Size sz = Converter.ToSize(node.Attributes["Size"].InnerText);
            writer.Write("      {0}.Size = new Size({1:D}, {2:D});\n", varname, sz.Width, sz.Height);
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
      public static bool Location(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Location"] != null)
        {
          try
          {
            System.Drawing.Point pt = Converter.ToPoint(node.Attributes["Location"].InnerText);
            writer.Write("      {0}.Location = new Point({1:D}, {2:D});\n", varname, pt.X, pt.Y);
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
      public static bool Padding(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Padding"] != null)
        {
          try
          {
            Padding pd = Converter.ToPadding(node.Attributes["Padding"].InnerText);
            writer.Write("      {0}.Padding = new Padding({1:D}, {2:D}, {3:D}, {4:D});\n", varname, pd.Left, pd.Top, pd.Right, pd.Bottom);
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
      public static bool Margin(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Margin"] != null)
        {
          try
          {
            Padding pd = Converter.ToPadding(node.Attributes["Margin"].InnerText);
            writer.Write("      {0}.Margin = new Padding({1:D}, {2:D}, {3:D}, {4:D});\n", varname, pd.Left, pd.Top, pd.Right, pd.Bottom);
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
      public static bool Visible(XmlNode node, StringWriter writer, string varname)
      {
        return FlagAttributeTemplate(node, writer, varname, "Visible", "Visible");
      }
      public static bool Enabled(XmlNode node, StringWriter writer, string varname)
      {
        return FlagAttributeTemplate(node, writer, varname, "Enabled", "Enabled");
      }
      public static bool TabStop(XmlNode node, StringWriter writer, string varname)
      {
        return FlagAttributeTemplate(node, writer, varname, "TabStop", "TabStop");
      }
      public static bool IsSplitterFixed(XmlNode node, StringWriter writer, string varname)
      {
        return FlagAttributeTemplate(node, writer, varname, "Fixed", "IsSplitterFixed");
      }
      public static bool EnableDoubleBuffered(XmlNode node, StringWriter writer, string varname)
      {
        writer.Write("      {0}.DoubleBuffered = true;\n", varname);
        return true;
      }
      public static bool FixedPanel(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["FixedPanel"] != null)
        {
          switch (node.Attributes["FixedPanel"].InnerText.Trim())
          {
            case "First":
              writer.Write("      {0}.FixedPanel = FixedPanel.Panel1;\n", varname);
              break;
            case "Second":
              writer.Write("      {0}.FixedPanel = FixedPanel.Panel2;\n", varname);
              break;
            case "None":
              writer.Write("      {0}.FixedPanel = FixedPanel.None;\n", varname);
              break;            
          }
          return true;
        }
        return false;
      }
      public static bool PanelMinSizes(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["PanelMinSizes"] != null)
        {
          try
          {
            System.Drawing.Size sz = Converter.ToSize(node.Attributes["PanelMinSizes"].InnerText);
            writer.Write(
              "      {0}.Panel1MinSize = {1:D};\n" +
              "      {0}.Panel2MinSize = {2:D};\n",
              varname, sz.Width, sz.Height
            );
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine(e.ToString());
          }
        }
        return false;
      }
      public static bool Orientation(XmlNode node, StringWriter writer, string varname)
      {
        if (node.Attributes["Orientation"] != null)
        {
          switch (node.Attributes["Orientation"].InnerText.Trim())
          {
            case "Horizontal":
              writer.Write("      {0}.Orientation = Orientation.Horizontal;\n", varname);
              break;
            case "Vertical":
              writer.Write("      {0}.Orientation = Orientation.Vertical;\n", varname);
              break;
          }
          return true;
        }
        return false;
      }
      private static bool FlagAttributeTemplate(XmlNode node, StringWriter writer, string varname, string attribname, string propname)
      {
        if (node.Attributes[attribname] != null)
        {
          try
          {
            writer.Write("      {0}.{1} = {2};\n", varname, propname, Converter.ToBool(node.Attributes[attribname].InnerText) ? "true" : "false");
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine("{0}", e.ToString());
          }
        }
        return false;
      }
    }
    public static class Events
    {
      private static bool EventTemplate(XmlNode node, StringWriter writer, string varname, string attribname, string eventname)
      {
        if (node.Attributes[attribname] != null)
        {
          writer.Write("      {0}.{1} += EventCommandProxy.Create(this.server, \"{2}\");\n", varname, eventname, node.Attributes[attribname].InnerText);
          return true;
        }
        return false;
      }
      public static bool Click(XmlNode node, StringWriter writer, string varname)
      {
        return EventTemplate(node, writer, varname, "OnClick", "Click");
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
        tmpl.Write(
          "  public class {0}: IPropertyListener\n", TemplateName(container, property)
        );
        tmpl.Write(
          "  {\n" +
          "    private IApplicationServer server;\n"
        );
        tmpl.Write(
          "    private {0} container;\n", container
        );
        tmpl.Write(
          "    private string variable;\n" +
          "    private bool locked = false; // prevents from endless recursion on this listener instance\n"
        );
        tmpl.Write(
          "    public {0} ({1} container, IApplicationServer server, string variable)\n", TemplateName(container, property), container
        );
        tmpl.Write(
          "    {\n" +
          "      this.server = server;\n" +
          "      this.variable = variable;\n" +
          "      this.container = container;\n" +
          "      this.server.RegisterPropertyListener(this.variable, this);\n" +
          "    }\n"
        );
        tmpl.Write(
          "    ~{0}()\n", TemplateName(container, property)
        );
        tmpl.Write("    {\n" +
          "      this.server.UnregisterPropertyListener(this.variable, this);\n" +
          "    }\n" +
          "    public void OnAddProperty(string name)\n" +
          "    {\n" +
          "      OnChangeProperty(this.variable);\n" +
          "    }\n" +
          "    public void OnChangeProperty(string name)\n" +
          "    {\n" +
          "      bool prelocked = true; // checks if this method call actually locked listener\n" +
          "      try\n" +
          "      {\n" +
          "        if (!this.locked)\n" +
          "        {\n" +
          "          prelocked = false;\n" +
          "          this.locked = true;\n" +
          "          string buffer = \"\";\n" +
          "          server.GetProperty(this.variable, ref buffer);\n"
        );
        tmpl.Write(
          "          container.{0} = Converter.{1}(buffer);\n", property, converter
        );
        tmpl.Write(
          "        }\n" +
          "      }\n" +
          "      catch (Exception e)\n" +
          "      {\n" +
          "        System.Console.WriteLine(e.ToString());\n" +
          "      }\n" +
          "      if (!prelocked)\n" +
          "        this.locked = false;\n" +
          "    }\n" +
          "    public void OnUpdateProperty(object sender, EventArgs args)\n" +
          "    {\n" +
          "      bool prelocked = true; // checks if this method call actually locked listener\n" +
          "      try\n" +
          "      {\n" +
          "        if (!this.locked)\n" +
          "        {\n" +
          "          prelocked = false;\n" +
          "          this.locked = true;\n"
        );
        tmpl.Write(
          "          server.SetProperty(this.variable, Converter.ToString(container.{0}));\n", property
        );
        tmpl.Write(
          "        }\n" +
          "      }\n" +
          "      catch (Exception e)\n" +
          "      {\n" +
          "        System.Console.WriteLine(e.ToString());\n" +
          "      }\n" +
          "      if (!prelocked)\n" +
          "        this.locked = false;\n" +
          "    }\n" +
          "    public void OnRemoveProperty(string name)\n" +
          "    {\n" +
          "      try\n" +
          "      {\n"
        );
        tmpl.Write(
          "        container.{0} = default({1});\n", property, type
        );
        tmpl.Write(
          "      }\n" +
          "      catch (Exception e)\n" +
          "      {\n" +
          "        System.Console.WriteLine(e.ToString());\n" +
          "      }\n" +
          "    }\n" +
          "  }\n"
        );
        return tmpl.ToString();
      }
    }
  }
}
