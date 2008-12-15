using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Reflection;

namespace tools.ui.windows_forms
{
  public class RibbonBuilder: IPlugin
  {
    protected static int ControlIndex = 0;
    protected static string[] RibbonModules = new string[] {
      "System.Windows.Forms.dll",
      Application.StartupPath + "\\tools.ui.windows_forms.Helpers.dll",
      Application.StartupPath + "\\System.Windows.Forms.Ribbon.dll",
    };
    protected Dictionary<string, Assembly> assemblies = new Dictionary<string, Assembly>();
    public Control CreateControl(string initstring)
    {
      throw new NotImplementedException("CreateControl method not implemented yet");
    }
    public void LoadConfiguration(XmlNode root)
    {
      foreach (XmlNode node in root.SelectNodes("Ribbon"))
      {
        try
        {
          if (node.Attributes["Name"] == null)
            throw new Exception("'Name' attribute not specified for Ribbon control");
          if (assemblies.ContainsKey(node.Attributes["Name"].InnerText))
            throw new Exception("Ribbon control with specified name already exists");
          assemblies.Add(node.Attributes["Name"].InnerText, CodeBuilder.Build(RibbonModules, ParseRibbon, node));
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      }
    }
    private void ParseRibbon(XmlNode ribbon, StringWriter writer)
    {
      string classname = ribbon.Attributes["Name"].InnerText;
      StringWriter tailwriter = new StringWriter();
      Dictionary<string, int> propclasses = new Dictionary<string, int>();

      writer.Write("using System;\n");
      writer.Write("using System.Drawing;\n");
      writer.Write("using System.Windows.Forms;\n");
      writer.Write("using System.Collections.Generic;\n\n");
      writer.Write("namespace tools.ui.windows_forms {{\n");
      foreach (XmlNode dropdown in ribbon.SelectNodes("Menu"))
        ParseDropDownMenu(dropdown, writer, tailwriter, propclasses);
      writer.Write("  public class {0}Ribbon: Ribbon {{\n", classname);
      writer.Write("    protected IApplicationServer server;\n");
      writer.Write("    public {0}Ribbon(IApplicationServer server): base()\n", classname);
      writer.Write("    {{\n");
      writer.Write("      this.server = server;\n");
      writer.Write("      this.SuspendLayout();\n");
      writer.Write("      this.TabSpacing = 6;\n");
      writer.Write("      this.TabIndex = 0;\n");
      writer.Write("      this.Name = \"{0}\";\n", classname);
      writer.Write("      this.Minimized = false;\n");
      writer.Write("      \n");
      foreach (XmlNode tab in ribbon.SelectNodes("Tab"))
        writer.Write("      this.Tabs.Add({0});\n", ParseRibbonTab(tab, writer, tailwriter, propclasses));
      writer.Write("      \n");
      writer.Write("      this.RestoreLayout();\n");
      writer.Write("    }}\n");
      writer.Write("  }}\n");
      writer.Write("{0}", tailwriter.ToString());
      writer.Write("}}\n");
    }
    private void ParseDropDownMenu(XmlNode menu, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      if (menu.Attributes["Name"] == null)
        throw new Exception("'Name' attribute for ribbon drop-down menu not specified");
      string varname = menu.Attributes["Name"].InnerText.Trim();
      if (menu.Attributes["Source"] != null)
      {
        writer.Write("  public class RibbonMenu_{0}: RibbonMenu_{1}\n", varname, menu.Attributes["Source"].InnerText.Trim());
        writer.Write("  {{\n");
        writer.Write("    public RibbonMenu_{0}(IApplicationServer server): base(server)\n", varname);
        writer.Write("    {{\n");
      }
      else
      {
        writer.Write("  {{\n");
        writer.Write("  public class RibbonMenu_{0}: RibbonButton\n", varname);
        writer.Write("    protected IApplicationServer server;\n");
        writer.Write("    public RibbonMenu_{0}(IApplicationServer server): base()\n", varname);
        writer.Write("    {{\n");
        writer.Write("      this.server = server;\n");
      }
      writer.Write("      this.SuspendLayout();\n");
      CodeSnippets.Attributes.Icon(menu, writer, "this");
      CodeSnippets.Attributes.Image(menu, writer, "this");
      CodeSnippets.Attributes.Text(menu, writer, "this");
      CodeSnippets.Attributes.SizeMode(menu, writer, "this");
      if (CodeSnippets.Events.Click(menu, writer, "this"))
        writer.Write("      this.Style = RibbonButtonStyle.SplitDropDown;\n");
      else
        writer.Write("      this.Style = RibbonButtonStyle.DropDown;\n");
      writer.Write("      \n");
      foreach (XmlNode item in menu.ChildNodes)
      {
        try
        {
          switch (item.Name)
          {
            case "Item":
              writer.Write("      this.DropDownItems.Add({0});\n", ParseDropDownItem(item, writer, tailwriter, propclasses));
              break;
            case "Separator":
              writer.Write("      this.DropDownItems.Add({0});\n", ParseSeparator(item, writer, tailwriter, propclasses));
              break;
            default:
              throw new Exception("Unknown node tag '" + item.Name + "' when parsing ribbon dropdown menu");
          }
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
      }
      writer.Write("      \n");
      writer.Write("      this.RestoreLayout();\n");
      writer.Write("    }}\n");
      writer.Write("  }}\n");
    }
    private string ParseDropDownItem(XmlNode item, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string varname = "ribbondropdownitem" + ControlIndex.ToString();
      if (item.Attributes["Source"] != null)
        writer.Write("      RibbonButton {0} = new RibbonMenu_{1}(this.server);\n", varname, item.Attributes["Source"].InnerText.Trim());
      else
        writer.Write("      RibbonButton {0} = new RibbonButton();\n", varname);
      CodeSnippets.Attributes.Icon(item, writer, varname);
      CodeSnippets.Attributes.Image(item, writer, varname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      foreach (XmlNode sub in item.ChildNodes)
      {
        try
        {
          switch (sub.Name)
          {
            case "Item":
              writer.Write("      {0}.DropDownItems.Add({1});\n", varname, ParseDropDownItem(sub, writer, tailwriter, propclasses));
              break;
            case "Separator":
              writer.Write("      {0}.DropDownItems.Add({1});\n", varname, ParseSeparator(sub, writer, tailwriter, propclasses));
              break;
            default:
              throw new Exception("Unknown node tag '" + sub.Name + "' when parsing ribbon dropdown menu subitem");
          }
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
      }
      writer.Write("      if ({0}.DropDownItems.Count > 0)\n", varname);
      writer.Write("        {0}.Style = RibbonButtonStyle.DropDown;\n", varname);
      writer.Write("      else\n");
      writer.Write("      {{\n");
      writer.Write("        {0}.Style = RibbonButtonStyle.Normal;\n", varname);
      CodeSnippets.Events.Click(item, writer, varname);
      writer.Write("      }}\n");
      return varname;
    }
    private string ParseRibbonTab(XmlNode tab, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string tabvarname = "ribbontab" + ControlIndex.ToString();
      string tabtext = tab.Attributes["Title"] != null ? tab.Attributes["Title"].InnerText : ("Tab " + ControlIndex.ToString());
      writer.Write("      RibbonTab {0} = new RibbonTab();\n", tabvarname);
      writer.Write("      {0}.Text = \"{1}\";\n", tabvarname, tabtext);
      writer.Write("      \n");
      foreach (XmlNode panel in tab.SelectNodes("Panel"))
        writer.Write("      {0}.Panels.Add({1});\n", tabvarname,  ParseRibbonPanel(panel, writer, tailwriter, propclasses));
      writer.Write("      \n");
      return tabvarname;
    }
    private string ParseRibbonPanel(XmlNode panel, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string panelvarname = "ribbonpanel" + ControlIndex.ToString();
      string paneltext = panel.Attributes["Title"] != null ? panel.Attributes["Title"].InnerText : ("Panel " + ControlIndex.ToString());
      // Create panel
      writer.Write("      RibbonPanel {0} = new RibbonPanel();\n", panelvarname);
      writer.Write("      {0}.Text = \"{1}\";\n", panelvarname, paneltext);
      CodeSnippets.Attributes.Flow(panel, writer, panelvarname);
      writer.Write("      \n");
      // process childs
      foreach (XmlNode group in panel.SelectNodes("Group"))
        writer.Write("      {0}.Items.Add({1});\n", panelvarname, ParseRibbonItemGroup(group, writer, tailwriter, propclasses));
      foreach (XmlNode item in panel.ChildNodes)
        writer.Write("      {0}.Items.Add({1});\n", panelvarname, ParseRibbonItem(item, writer, tailwriter, propclasses));
      writer.Write("      \n");
      return panelvarname;
    }
    private string ParseRibbonItemGroup(XmlNode group, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string groupvarname = "ribbonitemgroup" + ControlIndex.ToString();
      writer.Write("      RibbonItemGroup {0} = new RibbonItemGroup();\n", groupvarname);
      writer.Write("      \n");
      foreach (XmlNode item in group.ChildNodes)
        try
        {
          writer.Write("      {0}.Items.Add({1});\n", groupvarname, ParseRibbonItem(item, writer, tailwriter, propclasses));
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
      writer.Write("      \n");
      return groupvarname;
    }
    private string ParseRibbonItem(XmlNode item, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      switch (item.Name)
      {
        case "Separator":
          return ParseSeparator(item, writer, tailwriter, propclasses);
        case "Button":
          return ParseButton(item, writer, tailwriter, propclasses);
        case "Menu":
          return ParseMenu(item, writer, tailwriter, propclasses);
        case "Editor":
          return ParseEditBox(item, writer, tailwriter, propclasses);
      }
      return "null";
    }
    private string ParseEditBox(XmlNode item, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string varname = "ribboneditbox" + ControlIndex.ToString();
      if (item.Attributes["Variable"] == null)
        throw new Exception("Content variable not specified");
      string binding = item.Attributes["Variable"].InnerText;
      writer.Write("      RibbonTextBox {0} = new RibbonTextBox();\n", varname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      CodeSnippets.Attributes.Image(item, writer, varname);
      // register link between textbox contents and scripting variable
      if (!propclasses.ContainsKey("TextBoxText"))
      {
        propclasses.Add("TextBoxText", 0);
        tailwriter.Write("{0}", CodeBuilder.PropertyListenerTemplate("TextBoxText", "string", "RibbonTextBox", "ToString"));
      }
      ++ControlIndex;
      writer.Write("      {0}.TextBoxTextChanged += new EventHandler(new TextBoxTextPropertyListener({0}, this.server, \"{1}\").OnUpdateProperty);\n",
        varname, binding);
      return varname;
    }
    private string ParseButton(XmlNode item, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string buttonvarname = "ribbonbutton" + ControlIndex.ToString();
      writer.Write("      RibbonButton {0} = new RibbonButton();\n", buttonvarname);
      writer.Write("      {0}.Style = RibbonButtonStyle.Normal;\n", buttonvarname);
      CodeSnippets.Attributes.Text(item, writer, buttonvarname);
      CodeSnippets.Attributes.Image(item, writer, buttonvarname);
      CodeSnippets.Attributes.Icon(item, writer, buttonvarname);
      CodeSnippets.Attributes.SizeMode(item, writer, buttonvarname);
      CodeSnippets.Events.Click(item, writer, buttonvarname);
      return buttonvarname;
    }
    private string ParseMenu(XmlNode item, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string varname = "ribbonmenu" + ControlIndex.ToString();
      if (item.Attributes["Source"] == null)
        throw new Exception("Content not specified for Menu control");
      writer.Write("      RibbonButton {0} = new RibbonMenu_{1}(this.server);\n", varname, item.Attributes["Source"].InnerText.Trim());
      CodeSnippets.Attributes.Text(item, writer, varname);
      CodeSnippets.Attributes.Image(item, writer, varname);
      CodeSnippets.Attributes.Icon(item, writer, varname);
      CodeSnippets.Attributes.SizeMode(item, writer, varname);
      if (CodeSnippets.Events.Click(item, writer, varname))
        writer.Write("      {0}.Style = RibbonButtonStyle.SplitDropDown;\n", varname);
      return varname;
    }
    private string ParseSeparator(XmlNode item, StringWriter writer, StringWriter tailwriter, Dictionary<string, int> propclasses)
    {
      ++ControlIndex;
      string varname = "ribbonseparator" + ControlIndex.ToString();
      writer.Write("      RibbonSeparator {0} = new RibbonSeparator();\n", varname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      return varname;
    }
  }
}
