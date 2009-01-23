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
    public static class Common
    {
      public delegate string Parser<T>(T node, StringWriter writer);
      public static void Instantiate(StringWriter writer, string name, string type)
      {
        writer.Write("      {1} {0} = new {1}();\n", name, type);
      }
      public static void AddControls(XmlNodeList nodes, StringWriter writer, string container, Parser<XmlNode> parser)
      {
        foreach (XmlNode node in nodes)
        {
          try
          {
            writer.Write(
          "      {0}.Controls.Add({1});\n", container, parser(node, writer)
            );
          }
          catch (Exception e)
          {
            Console.WriteLine(e.ToString());
          }
        }
      }
      public static bool Bind(XmlNode node, StringWriter writer, string name, string attrib, string eventname, string type, string property)
      {
        return Bind(node, writer, name, attrib, eventname, type, property, null);
      }
      public static bool Bind(XmlNode node, StringWriter writer, string name, string attrib, string eventname, string type, string property, string defaultvalue)
      {
        string value = null;
        if (node.Attributes[attrib] != null)
          value = node.Attributes[attrib].InnerText;
        else if (defaultvalue != null)
          value = defaultvalue;
        if (value != null)
        {
          if (!string.IsNullOrEmpty(eventname))
            writer.Write(
              "      {0}.{2} += new EventHandler(new {3}({0}, this.server, \"{1}\").OnUpdateProperty);\n",
              name,
              value,
              eventname,
              Properties.TemplateName(type, property)
            );
          else
            writer.Write(
              "      new {2}({0}, this.server, \"{1}\");\n",
              name,
              value,
              Properties.TemplateName(type, property)
            );
        }
        else
          return false;
        return true;
      }
    }
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
      public static bool MinimumMaximum(XmlNode node, StringWriter writer, string varname)
      {
        return MinimumMaximum(node, writer, varname, null);
      }
      public static bool MinimumMaximum(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        try
        {
          string value = null;
          if (node.Attributes["Bounds"] != null)
            value = node.Attributes["Bounds"].InnerText;
          else if (defaultvalue != null)
            value = defaultvalue;
          if (value != null)
          {
            string[] parts = value.Split(" ,;".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
            if (parts.Length != 2)
              throw new ArgumentException("Invalid 'Bounds' format");
            writer.Write("      {0}.Minimum = {1};\n      {0}.Maximum = {2};\n",
              varname, Converter.ToDecimal(parts[0]).ToString(), Converter.ToDecimal(parts[0]).ToString());
            return true;
          }
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
        return false;
      }
      public static bool Increment(XmlNode node, StringWriter writer, string varname)
      {
        return Increment(node, writer, varname, null);
      }
      public static bool Increment(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        try
        {
          string value = null;
          if (node.Attributes["Step"] != null)
            value = node.Attributes["Step"].InnerText;
          else if (defaultvalue != null)
            value = defaultvalue;
          if (value != null)
          {
            writer.Write("      {0}.Increment = {1};\n", varname, Converter.ToDecimal(value).ToString());
            return true;
          }
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
        return false;
      }
      public static bool Mask(XmlNode node, StringWriter writer, string varname)
      {
        return Mask(node, writer, varname, null);
      }
      public static bool Mask(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;
        if (node.Attributes["Mask"] != null)
          value = node.Attributes["Mask"].InnerText;
        else if (defaultvalue != null)
          value = defaultvalue;
        if (value != null)
        {
          writer.Write("      {0}.Mask = \"{1}\";\n", varname, value);
          return true;
        }
        return false;
      }
      public static bool Text(XmlNode node, StringWriter writer, string varname)
      {
        return Text(node, writer, varname, null);
      }
      public static bool Text(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;
        if (node.Attributes["Text"] != null)
          value = node.Attributes["Text"].InnerText;
        else if (defaultvalue != null)
          value = defaultvalue;
        if (value != null)
        {
          writer.Write("      {0}.Text = \"{1}\";\n", varname, value);
          return true;
        }
        return false;
      }
      public static bool SmallImage(XmlNode node, StringWriter writer, string varname)
      {
        return SmallImage(node, writer, varname, null);
      }
      public static bool SmallImage(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;
        if (node.Attributes["Icon"] != null)
          value = node.Attributes["Icon"].InnerText;
        else if (defaultvalue != null)
          value = defaultvalue;
        if (value != null)                          
        {
          writer.Write("      {0}.SmallImage = Image.FromFile(\"{1}\");\n", varname, value.Trim());
          return true;
        }
        return false;
      }
      public static bool Image(XmlNode node, StringWriter writer, string varname)
      {
        return Image(node, writer, varname, null);
      }
      public static bool Image(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Image"] != null)        
          value = node.Attributes["Image"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          writer.Write("      {0}.Image = Image.FromFile(\"{1}\");\n", varname, value.Trim());
          return true;
        }
        return false;
      }
      public static bool BackgroundImage(XmlNode node, StringWriter writer, string varname)
      {
        return BackgroundImage(node, writer, varname, null);
      }
      public static bool BackgroundImage(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["BackImage"] != null)        
          value = node.Attributes["BackImage"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          writer.Write("      {0}.BackgroundImage = Image.FromFile(\"{1}\");\n", varname, value.Trim());
          return true;
        }
        return false;
      }
      public static bool SizeMode(XmlNode node, StringWriter writer, string varname)
      {
        return SizeMode(node, writer, varname, null);
      }
      public static bool SizeMode(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Size"] != null)        
          value = node.Attributes["Size"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          switch (value.Trim())
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
        writer.Write("      {0}.MinSizeMode = {0}.MaxSizeMode = RibbonElementSizeMode.Compact;\n", varname);
        return false;
      }
      public static bool FlowsTo(XmlNode node, StringWriter writer, string varname)
      {
        return FlowsTo(node, writer, varname, null);
      }
      public static bool FlowsTo(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Flow"] != null)
          value = node.Attributes["Flow"].InnerText;
        else if (defaultvalue != null)
          value = defaultvalue;
        if (value != null)
        {
          switch (value.Trim())
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
      public static bool FlowDirection(XmlNode node, StringWriter writer, string varname)
      {
        return FlowDirection(node, writer, varname, null);
      }
      public static bool FlowDirection(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Flow"] != null)        
          value = node.Attributes["Flow"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          switch (value.Trim())
          {
            case "LeftToRight":
              writer.Write("      {0}.FlowDirection = FlowDirection.LeftToRight;\n", varname);
              break;
            case "RightToLeft":
              writer.Write("      {0}.FlowDirection = FlowDirection.RightToLeft;\n", varname);
              break;
            case "TopDown":
              writer.Write("      {0}.FlowDirection = FlowDirection.TopDown;\n", varname);
              break;
            case "BottomUp":
              writer.Write("      {0}.FlowDirection = FlowDirection.BottomUp;\n", varname);
              break;
          }
          return true;
        }
        return false;
      }
      public static bool TextBoxWidth(XmlNode node, StringWriter writer, string varname)
      {
        return TextBoxWidth(node, writer, varname, null);
      }
      public static bool TextBoxWidth(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        try
        {
          string value = null;                        
          if (node.Attributes["Width"] != null)        
            value = node.Attributes["Width"].InnerText;
          else if (defaultvalue != null)              
            value = defaultvalue;                     
          if (value != null)                          
          {
            if (Converter.ToInt(value) <= 0)
              throw new Exception("Invalid 'Width' attribute value");
            writer.Write("      {0}.TextBoxWidth = {1};\n", varname, value);
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
        return Name(node, writer, varname, null);
      }
      public static bool Name(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Name"] != null)        
          value = node.Attributes["Name"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          writer.Write("      {0}.Name = \"{1}\";\n", varname, value);
          return true;
        }
        return false;
      }
      public static bool ForeColor(XmlNode node, StringWriter writer, string varname)
      {
        return ForeColor(node, writer, varname, null);
      }
      public static bool ForeColor(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["ForeColor"] != null)        
          value = node.Attributes["ForeColor"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            writer.Write("      {0}.ForeColor = Color.FromArgb({1:D});\n", varname, Converter.ToColor(value).ToArgb());
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
        return BackColor(node, writer, varname, null);
      }
      public static bool BackColor(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["BackColor"] != null)        
          value = node.Attributes["BackColor"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            writer.Write("      {0}.BackColor = Color.FromArgb({1:D});\n", varname, Converter.ToColor(value).ToArgb());
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
        return AnchorDock(node, writer, varname, null);
      }
      public static bool AnchorDock(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        try
        {
          string value = null;
          if (node.Attributes["Anchor"] != null && node.Attributes["Dock"] == null)
            value = "Anchor|" + node.Attributes["Anchor"].InnerText.Trim();
          else if (node.Attributes["Anchor"] == null && node.Attributes["Dock"] != null)
            value = "Dock|" + node.Attributes["Dock"].InnerText.Trim();
          else if (defaultvalue != null)
            value = defaultvalue;
          if (value != null)
          {
            string[] tokens = value.Split("|".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
            if (tokens.Length != 2)
              throw new Exception("Invalid Anchor or Dock default value specified");
            switch (tokens[0].Trim())
            {
              case "Anchor":
                string[] items = Converter.ToAnchorStyles(tokens[1]).ToString().Split(" ,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries);
                string buffer = "";
                foreach (string item in items)
                  buffer += (string.IsNullOrEmpty(buffer) ? "AnchorStyles." : " | AnchorStyles.") + item;
                writer.Write("      {0}.Anchor = {1};\n", varname, buffer);
                  return true;
              case "Dock":
                writer.Write("      {0}.Dock = DockStyle.{1};\n", varname, Converter.ToDockStyle(tokens[1]).ToString());
                return true;
            }
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
        return Size(node, writer, varname, null);
      }
      public static bool Size(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Size"] != null)        
          value = node.Attributes["Size"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            System.Drawing.Size sz = Converter.ToSize(value);
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
        return Location(node, writer, varname, null);
      }
      public static bool Location(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Location"] != null)        
          value = node.Attributes["Location"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            System.Drawing.Point pt = Converter.ToPoint(value);
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
        return Padding(node, writer, varname, null);
      }
      public static bool Padding(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Padding"] != null)        
          value = node.Attributes["Padding"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            Padding pd = Converter.ToPadding(value);
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
        return Margin(node, writer, varname, null);
      }
      public static bool Margin(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Margin"] != null)        
          value = node.Attributes["Margin"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            Padding pd = Converter.ToPadding(value);
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
        return Visible(node, writer, varname, null);
      }
      public static bool Visible(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        return FlagAttributeTemplate(node, writer, varname, defaultvalue, "Visible", "Visible");
      }
      public static bool Enabled(XmlNode node, StringWriter writer, string varname)
      {
        return Enabled(node, writer, varname, null);
      }
      public static bool Enabled(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        return FlagAttributeTemplate(node, writer, varname, defaultvalue, "Enabled", "Enabled");
      }
      public static bool TabStop(XmlNode node, StringWriter writer, string varname)
      {
        return TabStop(node, writer, varname, null);
      }
      public static bool TabStop(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        return FlagAttributeTemplate(node, writer, varname, defaultvalue, "TabStop", "TabStop");
      }
      public static bool IsSplitterFixed(XmlNode node, StringWriter writer, string varname)
      {
        return IsSplitterFixed(node, writer, varname, null);
      }
      public static bool IsSplitterFixed(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        return FlagAttributeTemplate(node, writer, varname, defaultvalue, "Fixed", "IsSplitterFixed");
      }
      public static bool WrapContents(XmlNode node, StringWriter writer, string varname)
      {
        return WrapContents(node, writer, varname, null);
      }
      public static bool WrapContents(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        return FlagAttributeTemplate(node, writer, varname, defaultvalue, "Wrap", "WrapContents");
      }
      public static bool EnableDoubleBuffered(XmlNode node, StringWriter writer, string varname)
      {
        return EnableDoubleBuffered(node, writer, varname, null);
      }
      public static bool EnableDoubleBuffered(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        writer.Write("      {0}.DoubleBuffered = true;\n", varname);
        return true;
      }
      public static bool FixedPanel(XmlNode node, StringWriter writer, string varname)
      {
        return FixedPanel(node, writer, varname, null);
      }
      public static bool FixedPanel(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["FixedPanel"] != null)        
          value = node.Attributes["FixedPanel"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          switch (value.Trim())
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
        return PanelMinSizes(node, writer, varname, null);
      }
      public static bool PanelMinSizes(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["PanelMinSizes"] != null)        
          value = node.Attributes["PanelMinSizes"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            System.Drawing.Size sz = Converter.ToSize(value);
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
        return Orientation(node, writer, varname, null);
      }
      public static bool Orientation(XmlNode node, StringWriter writer, string varname, string defaultvalue)
      {
        string value = null;                        
        if (node.Attributes["Orientation"] != null)        
          value = node.Attributes["Orientation"].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          switch (value.Trim())
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
      private static bool FlagAttributeTemplate(XmlNode node, StringWriter writer, string varname, string defaultvalue, string attribname, string propname)
      {
        string value = null;                        
        if (node.Attributes[attribname] != null)        
          value = node.Attributes[attribname].InnerText;
        else if (defaultvalue != null)              
          value = defaultvalue;                     
        if (value != null)                          
        {
          try
          {
            writer.Write("      {0}.{1} = {2};\n", varname, propname, Converter.ToBool(value) ? "true" : "false");
            return true;
          }
          catch (Exception e)
          {
            Console.WriteLine(e.ToString());
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
          "      if (this.locked)" +
          "        return;" +
          "      this.locked = true;" +
          "      try\n" +
          "      {\n" +
          "        string buffer = \"\";\n" +
          "        server.GetProperty(this.variable, ref buffer);\n"
        );
        tmpl.Write(
          "        container.{0} = Converter.{1}(buffer);\n", property, converter
        );
        tmpl.Write(
          "      }\n" +
          "      catch (Exception e)\n" +
          "      {\n" +
          "        System.Console.WriteLine(e.ToString());\n" +
          "      }\n" +
          "      this.locked = false;\n" +
          "    }\n" +
          "    public void OnUpdateProperty(object sender, EventArgs args)\n" +
          "    {\n" +
          "      if (this.locked)" +
          "        return;" +
          "      this.locked = true;" +
          "      try\n" +
          "      {\n"
        );
        tmpl.Write(
          "        server.SetProperty(this.variable, Converter.ToString(container.{0}));\n", property
        );
        tmpl.Write(
          "      }\n" +
          "      catch (Exception e)\n" +
          "      {\n" +
          "        System.Console.WriteLine(e.ToString());\n" +
          "      }\n" +
          "      this.locked = false;\n" +
          "    }\n" +
          "    public void OnRemoveProperty(string name)\n" +
          "    {\n" +
          /*"      if (this.locked)" +*/
          "        return;" +
          "      this.locked = true;" +
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
          "      this.locked = false;\n" +
          "    }\n" +
          "  }\n"
        );
        return tmpl.ToString();
      }
    }
  }
}
