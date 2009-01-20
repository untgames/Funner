using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Reflection;

namespace tools.ui.windows_forms
{
  public class FormBuilder: IPlugin
  {
    public Control CreateControl(string init_string)
    {
      throw new NotImplementedException();
    }
    public void LoadConfiguration(XmlNode node)
    {
      foreach (XmlNode child in node.SelectNodes("Form"))
      {
        try
        {
          if (child.Attributes["Name"] == null)
            throw new ArgumentNullException("Form tag must contain 'Name' attribute");
          if (assemblies.ContainsKey(child.Attributes["Name"].InnerText))
            throw new Exception("Form with name '" + child.Attributes["Name"].InnerText + "' already exists");
          assemblies.Add(child.Attributes["Name"].InnerText, CodeBuilder.Build(modules, ParseForm, child));
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      }
    }
    int ControlIndex = 0;
    protected Dictionary<string, Assembly> assemblies = new Dictionary<string, Assembly>();
    private static string[] modules =
    {
      "System.Windows.Forms.dll",
      Application.StartupPath + "\\funner.tools.ui.windows_forms.PluginApi.dll",
      Application.StartupPath + "\\funner.tools.ui.windows_forms.Helpers.dll",
    };
    private void ParseForm(XmlNode node, StringWriter writer)
    {
      string name = node.Attributes["Name"].InnerText.Trim();
      writer.Write(
        "using System;\n" +
        "using System.Drawing;\n" +
        "using System.Windows.Forms;\n" +
        "using System.Collections.Generic;\n" +
        "\n" +
        "namespace tools.ui.windows_forms {\n" +
        "  public class CustomPanel: Panel\n" +
        "  {\n" +
        "    protected IApplicationServer server;\n" +
        "    public CustomPanel(IApplicationServer server)\n" +
        "    {\n" +
        "      this.server = server;\n"
      );
      CodeSnippets.Layout.Suspend(writer, "this");
      ParseContainerAttributes(node, writer, "this");
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, "this", ParseControl);
      CodeSnippets.Layout.Resume(writer, "this");
      writer.Write(
        "    } // end-of-ctor CustomPanel(IApplicationServer)\n" +
        "  } // end-of-class CustomPanel\n"
      );
      writer.Write(
        "{0}\n",
        CodeSnippets.Properties.Template("CheckBox", "Checked", "bool", "ToBool")
      );
      writer.Write(
        "} // end-of-namespace tools.ui.windows_forms\n"
      );
    }
    private void ParseContainerAttributes(XmlNode node, StringWriter writer, string varname)
    {
      CodeSnippets.Attributes.EnableDoubleBuffered(node, writer, varname);
      CodeSnippets.Attributes.Name(node, writer, varname, varname);
      CodeSnippets.Attributes.Text(node, writer, varname);
      CodeSnippets.Attributes.BackgroundImage(node, writer, varname);
      CodeSnippets.Attributes.Size(node, writer, varname);
      CodeSnippets.Attributes.Location(node, writer, varname);
      CodeSnippets.Attributes.Padding(node, writer, varname);
      CodeSnippets.Attributes.Margin(node, writer, varname);
      CodeSnippets.Attributes.ForeColor(node, writer, varname);
      CodeSnippets.Attributes.BackColor(node, writer, varname);
      CodeSnippets.Attributes.AnchorDock(node, writer, varname);
      CodeSnippets.Attributes.Visible(node, writer, varname);
      CodeSnippets.Attributes.Enabled(node, writer, varname);
      CodeSnippets.Attributes.TabStop(node, writer, varname);
    }
    private string ParseControl(XmlNode node, StringWriter writer)
    {
      ++ControlIndex;
      switch (node.Name)
      {
        // container controls
        case "Panel":   return ParsePanel(node, writer);
        case "Grid":    return ParseTablePanel(node, writer);
        case "Tabs":    return ParseTabPanel(node, writer);
        case "Group":   return ParseGroupBox(node, writer);
        case "Split":   return ParseSplitter(node, writer);
        case "Flow":    return ParseFlowPanel(node, writer);
        // other controls
        case "Label":   return ParseLabel(node, writer);
        case "Input":   return ParseInput(node, writer);
        case "Button":  return ParseButton(node, writer);
        case "Flag":    return ParseCheckBox(node, writer);
        case "Switch":  return ParseRadioGroup(node, writer);
        case "List":    return ParseList(node, writer);
      }
      return "null";
    }
    private string ParsePanel(XmlNode node, StringWriter writer)
    {
      string name = "panel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "Panel");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, name, ParseControl);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseTablePanel(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
      string name = "tablepanel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "TableLayoutPanel");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseTabPanel(XmlNode node, StringWriter writer)
    {
      string name = "tabpanel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "TabControl");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Common.AddControls(node.SelectNodes("Tab"), writer, name, ParseTabPage);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseTabPage(XmlNode node, StringWriter writer)
    {
      ++ControlIndex;
      string name = "tabpage" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "TabPage");
      CodeSnippets.Layout.Suspend(writer, name);
      CodeSnippets.Attributes.Name(node, writer, name, name);
      CodeSnippets.Attributes.Text(node, writer, name, name);
      writer.Write(
        "      {0}.UseVisualStyleBackColor = true;\n", name
      );
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, name, ParseControl);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseGroupBox(XmlNode node, StringWriter writer)
    {
      string name = "groupbox" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "GroupBox");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, name, ParseControl);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseSplitter(XmlNode node, StringWriter writer)
    {
      string name = "splitterpanel" + ControlIndex.ToString("D");
      XmlNodeList panels = node.SelectNodes("Panel");
      if (panels == null || panels.Count != 2)
        throw new Exception("Invalid Splitter Panel layout, must contain exactly two 'Panel' nodes");
      CodeSnippets.Common.Instantiate(writer, name, "SplitContainer");
      CodeSnippets.Layout.Suspend(writer, name);
      CodeSnippets.Layout.Suspend(writer, name + ".Panel1");
      CodeSnippets.Layout.Suspend(writer, name + ".Panel2");
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Attributes.Orientation(node, writer, name);
      CodeSnippets.Common.AddControls(panels[0].ChildNodes, writer, name + ".Panel1", ParseControl);
      CodeSnippets.Common.AddControls(panels[1].ChildNodes, writer, name + ".Panel2", ParseControl);
      CodeSnippets.Layout.Resume(writer, name + ".Panel1");
      CodeSnippets.Layout.Resume(writer, name + ".Panel2");
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseFlowPanel(XmlNode node, StringWriter writer)
    {
      string name = "flowpanel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "FlowLayoutPanel");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Attributes.FlowDirection(node, writer, name);
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, name, ParseControl);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // label item
    private string ParseLabel(XmlNode node, StringWriter writer)
    {
      string name = "label" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "Label");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // button item
    private string ParseButton(XmlNode node, StringWriter writer)
    {
      string name = "button" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "Button");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Events.Click(node, writer, name);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // input item
    private string ParseInput(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
      // place for datetime, text, spin, masked text
    }
    private string ParseDateTimeField(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    private string ParseNumericSpin(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    private string ParseTextBox(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    } 
    private string ParseMaskedTextBox(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    } 
    // flag item
    private string ParseCheckBox(XmlNode node, StringWriter writer)
    {
      string name = "checkbox" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "CheckBox");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Common.Bind(node, writer, name, "Variable", "CheckedChanged", "CheckBox", "Checked");
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // list item
    private string ParseList(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    private string ParseListBox(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    private string ParseComboBox(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    // switch item
    private string ParseRadioGroup(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
  }
}