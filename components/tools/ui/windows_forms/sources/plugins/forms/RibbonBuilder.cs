using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Reflection;

namespace tools.ui.windows_forms
{
  public class RibbonBuilderPlugin: IPlugin
  {
    protected static int ControlIndex = 0;
    protected static string[] RibbonModules = new string[] {
      "System.Windows.Forms.dll",
      Application.StartupPath + "\\funner.tools.ui.windows_forms.PluginApi.dll",
      Application.StartupPath + "\\funner.tools.ui.windows_forms.Helpers.dll",
      Application.StartupPath + "\\System.Windows.Forms.Ribbon.dll",
    };
    protected Dictionary<string, Assembly> assemblies = new Dictionary<string, Assembly>();
    protected IApplicationServer server = null;
    public RibbonBuilderPlugin(IApplicationServer server)
    {
      if (server == null)
        throw new ArgumentNullException("IApplicationServer reference must not be NULL");
      this.server = server;
    }
    public Control CreateControl(string initstring)
    {
      if (!assemblies.ContainsKey(initstring.Trim()))
        throw new KeyNotFoundException("Invalid initialization string, must specify existing ribbon control configuration name");
      return (Control)CodeBuilder.Instantiate(assemblies[initstring.Trim()], "tools.ui.windows_forms.CustomRibbon", this.server);
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

      writer.Write("using System;\n");
      writer.Write("using System.Drawing;\n");
      writer.Write("using System.Windows.Forms;\n");
      writer.Write("using System.Collections.Generic;\n\n");
      writer.Write("namespace tools.ui.windows_forms {\n");
      writer.Write("{0}", CodeSnippets.Properties.Template("RibbonTextBox", "TextBoxText", "string", "ToString"));
      foreach (XmlNode dropdown in ribbon.SelectNodes("Menu"))
        ParseDropDownMenu(dropdown, writer);
      foreach (XmlNode list in ribbon.SelectNodes("List"))
        ParseList(list, writer);
      writer.Write("  public class CustomRibbon: Ribbon\n");
      writer.Write("  {\n");
      writer.Write("    protected IApplicationServer server;\n");
      writer.Write("    public CustomRibbon(IApplicationServer server): base()\n");
      writer.Write("    {\n");
      writer.Write("      this.server = server;\n");
      writer.Write("      this.SuspendLayout();\n");
      writer.Write("      this.TabSpacing = 6;\n");
      writer.Write("      this.TabIndex = 0;\n");
      writer.Write("      this.Name = \"{0}\";\n", classname);
      writer.Write("      this.Minimized = false;\n");
      writer.Write("      \n");
      foreach (XmlNode tab in ribbon.SelectNodes("Tab"))
        writer.Write("      this.Tabs.Add({0});\n", ParseRibbonTab(tab, writer));
      writer.Write("      \n");
      writer.Write("      this.ResumeLayout();\n");
      writer.Write("    }\n");
      writer.Write("  }\n");
      writer.Write("}\n");
    }
    private void ParseList(XmlNode list, StringWriter writer)
    {
      if (list.Attributes["Name"] == null)
        throw new Exception("'Name' attribute for ribbon list not specified");
      string varname = list.Attributes["Name"].InnerText.Trim();
      writer.Write("  public class RibbonList_{0}: RibbonComboBox\n", varname);
      writer.Write("  {\n");
      writer.Write("    public RibbonList_{0}(): base()\n", varname);
      writer.Write("    {\n");
      foreach (XmlNode item in list.ChildNodes)
      {
        try
        {
          switch (item.Name)
          {
            case "Item":
              writer.Write("      this.DropDownItems.Add({0});\n", ParseListItem(item, writer));
              break;
            case "Separator":
              writer.Write("      this.DropDownItems.Add({0});\n", ParseSeparator(item, writer));
              break;
            default:
              throw new Exception("Unknown node tag '" + item.Name + "' when parsing ribbon combobox list");
          }
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
      }      
      writer.Write("    }\n");
      writer.Write("  }\n");
    }
    private string ParseListItem(XmlNode item, StringWriter writer)
    {
      ++ControlIndex;
      string varname = "ribbonlistitem" + ControlIndex.ToString();
      writer.Write("      RibbonButton {0} = new RibbonButton();\n", varname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      return varname;
    }
    private void ParseDropDownMenu(XmlNode menu, StringWriter writer)
    {
      if (menu.Attributes["Name"] == null)
        throw new Exception("'Name' attribute for ribbon drop-down menu not specified");
      string varname = menu.Attributes["Name"].InnerText.Trim();
      if (menu.Attributes["Source"] != null)
      {
        writer.Write("  public class RibbonMenu_{0}: RibbonMenu_{1}\n", varname, menu.Attributes["Source"].InnerText.Trim());
        writer.Write("  {\n");
        writer.Write("    public RibbonMenu_{0}(IApplicationServer server): base(server)\n", varname);
        writer.Write("    {\n");
      }
      else
      {
        writer.Write("  public class RibbonMenu_{0}: RibbonButton\n", varname);
        writer.Write("  {\n");
        writer.Write("    protected IApplicationServer server;\n");
        writer.Write("    public RibbonMenu_{0}(IApplicationServer server): base()\n", varname);
        writer.Write("    {\n");
        writer.Write("      this.server = server;\n");
      }
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
              writer.Write("      this.DropDownItems.Add({0});\n", ParseDropDownItem(item, writer));
              break;
            case "Separator":
              writer.Write("      this.DropDownItems.Add({0});\n", ParseSeparator(item, writer));
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
      writer.Write("    }\n");
      writer.Write("  }\n");
    }
    private string ParseDropDownItem(XmlNode item, StringWriter writer)
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
              writer.Write("      {0}.DropDownItems.Add({1});\n", varname, ParseDropDownItem(sub, writer));
              break;
            case "Separator":
              writer.Write("      {0}.DropDownItems.Add({1});\n", varname, ParseSeparator(sub, writer));
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
      writer.Write("      {\n");
      writer.Write("        {0}.Style = RibbonButtonStyle.Normal;\n", varname);
      CodeSnippets.Events.Click(item, writer, varname);
      writer.Write("      }\n");
      return varname;
    }
    private string ParseRibbonTab(XmlNode tab, StringWriter writer)
    {
      ++ControlIndex;
      string tabvarname = "ribbontab" + ControlIndex.ToString();
      writer.Write("      RibbonTab {0} = new RibbonTab();\n", tabvarname);
      if (!CodeSnippets.Attributes.Text(tab, writer, tabvarname))
        writer.Write("      {0}.Text = \"Tab {1}\";\n", tabvarname, ControlIndex.ToString());
      writer.Write("      \n");
      foreach (XmlNode panel in tab.SelectNodes("Panel"))
        writer.Write("      {0}.Panels.Add({1});\n", tabvarname,  ParseRibbonPanel(panel, writer));
      writer.Write("      \n");
      return tabvarname;
    }
    private string ParseRibbonPanel(XmlNode panel, StringWriter writer)
    {
      ++ControlIndex;
      string panelvarname = "ribbonpanel" + ControlIndex.ToString();
      // Create panel
      writer.Write("      RibbonPanel {0} = new RibbonPanel();\n", panelvarname);
      if (!CodeSnippets.Attributes.Text(panel, writer, panelvarname))
        writer.Write("      {0}.Text = \"Panel {1}\";\n", panelvarname, ControlIndex.ToString());
      CodeSnippets.Attributes.Flow(panel, writer, panelvarname);
      writer.Write("      \n");
      // process childs
      foreach (XmlNode item in panel.ChildNodes)
        try
        {
          if (item.Name == "Group")
            writer.Write("      {0}.Items.Add({1});\n", panelvarname, ParseRibbonItemGroup(item, writer));
          else
            writer.Write("      {0}.Items.Add({1});\n", panelvarname, ParseRibbonItem(item, writer));
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      writer.Write("      \n");
      return panelvarname;
    }
    private string ParseRibbonItemGroup(XmlNode group, StringWriter writer)
    {
      ++ControlIndex;
      string groupvarname = "ribbonitemgroup" + ControlIndex.ToString();
      writer.Write("      RibbonItemGroup {0} = new RibbonItemGroup();\n", groupvarname);
      writer.Write("      \n");
      foreach (XmlNode item in group.ChildNodes)
        try
        {
          writer.Write("      {0}.Items.Add({1});\n", groupvarname, ParseRibbonItem(item, writer));
        }
        catch (Exception e)
        {
          Console.WriteLine("{0}", e.ToString());
        }
      writer.Write("      \n");
      return groupvarname;
    }
    private string ParseRibbonItem(XmlNode item, StringWriter writer)
    {
      switch (item.Name)
      {
        case "Separator":
          return ParseSeparator(item, writer);
        case "Button":
          return ParseButton(item, writer);
        case "Menu":
          return ParseMenu(item, writer);
        case "Editor":
          return ParseEditBox(item, writer);
        case "List":
          return ParseComboBox(item, writer);
      }
      throw new Exception("Unknown ribbon element node '" + item.Name + "'");
    }
    private string ParseEditBox(XmlNode item, StringWriter writer)
    {
      ++ControlIndex;
      string varname = "ribboneditbox" + ControlIndex.ToString();
      if (item.Attributes["Variable"] == null)
        throw new Exception("Content variable not specified");
      string binding = item.Attributes["Variable"].InnerText;
      writer.Write("      RibbonTextBox {0} = new RibbonTextBox();\n", varname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      CodeSnippets.Attributes.Image(item, writer, varname);
      CodeSnippets.Attributes.TextBoxWidth(item, writer, varname);
      // register link between textbox contents and scripting variable
      writer.Write("      {0}.TextBoxTextChanged += new EventHandler(new {1}({0}, this.server, \"{2}\").OnUpdateProperty);\n",
        varname, CodeSnippets.Properties.TemplateName("RibbonTextBox", "TextBoxText"), binding);
      return varname;
    }
    private string ParseComboBox(XmlNode item, StringWriter writer)
    {
      ++ControlIndex;
      string varname = "ribboncombobox" + ControlIndex.ToString();
      if (item.Attributes["Source"] == null)
        throw new Exception("'Source' attribute must be specified and contain name of existing list");
      string classname = item.Attributes["Source"].InnerText.Trim();
      if (item.Attributes["Variable"] == null)
        throw new Exception("'Variable' attribute must be specified and contain name of core variable");
      string binding = item.Attributes["Variable"].InnerText;
      writer.Write("      RibbonComboBox {0} = new RibbonList_{1}();\n", varname, classname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      CodeSnippets.Attributes.Image(item, writer, varname);
      CodeSnippets.Attributes.TextBoxWidth(item, writer, varname);
      // register link between textbox contents and scripting variable
      writer.Write("      {0}.TextBoxTextChanged += new EventHandler(new {1}({0}, this.server, \"{2}\").OnUpdateProperty);\n",
        varname, CodeSnippets.Properties.TemplateName("RibbonTextBox", "TextBoxText"), binding);
      return varname;
    }
    private string ParseButton(XmlNode item, StringWriter writer)
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
    private string ParseMenu(XmlNode item, StringWriter writer)
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
    private string ParseSeparator(XmlNode item, StringWriter writer)
    {
      ++ControlIndex;
      string varname = "ribbonseparator" + ControlIndex.ToString();
      writer.Write("      RibbonSeparator {0} = new RibbonSeparator();\n", varname);
      CodeSnippets.Attributes.Text(item, writer, varname);
      return varname;
    }
  }
}
