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
      foreach (XmlNode child in node.ChildNodes)
        try
        {
          writer.Write(
        "      this.Controls.Add({0});\n", ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      CodeSnippets.Layout.Resume(writer, "this");
      writer.Write(
        "    } // end-of-ctor CustomPanel(IApplicationServer)\n" +
        "  } // end-of-class CustomPanel\n"
      );
      writer.Write(
        "} // end-of-namespace tools.ui.windows_forms\n"
      );
    }
    private void ParseContainerAttributes(XmlNode node, StringWriter writer, string varname)
    {
      CodeSnippets.Attributes.EnableDoubleBuffered(node, writer, varname);
      if (!CodeSnippets.Attributes.Name(node, writer, varname))
        writer.Write(
        "      {0}.Name = \"{0}\";\n", varname
        );
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
        case "Panel": return ParsePanel(node, writer);
        case "Grid":  return ParseTablePanel(node, writer);
        case "Tabs":  return ParseTabPanel(node, writer);
        case "Group": return ParseGroupBox(node, writer);
        case "Split": return ParseSplitter(node, writer);
        case "Flow":  return ParseFlowPanel(node, writer);
        // other controls
      }
      return "null";
    }
    private string ParsePanel(XmlNode node, StringWriter writer)
    {
      string name = "panel" + ControlIndex.ToString("D");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      foreach (XmlNode child in node.ChildNodes)
        try
        {
          writer.Write(
        "      {0}.Controls.Add({1});\n", name, ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseTablePanel(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    private string ParseTabPanel(XmlNode node, StringWriter writer)
    {
      string name = "tabpanel" + ControlIndex.ToString("D");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      foreach (XmlNode child in node.SelectNodes("Tab"))
      {
        ++ControlIndex;
        try
        {
          writer.Write(
        "      {0}.Controls.Add({1});\n", name, ParseTabPage(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      }
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseTabPage(XmlNode node, StringWriter writer)
    {
      string name = "tabpage" + ControlIndex.ToString("D");
      CodeSnippets.Layout.Suspend(writer, name);
      if (!CodeSnippets.Attributes.Name(node, writer, name))
        writer.Write(
        "      {0}.Name = \"{0}\";\n", name
        );
      if (!CodeSnippets.Attributes.Text(node, writer, name))
        writer.Write(
        "      {0}.Text = \"{0}\";\n", name
        );
      writer.Write(
        "      {0}.UseVisualStyleBackColor = true;\n", name
      );
      foreach (XmlNode child in node.ChildNodes)
        try
        {
          writer.Write(
        "      {0}.Controls.Add({1});\n", name, ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseGroupBox(XmlNode node, StringWriter writer)
    {
      string name = "grouppanel" + ControlIndex.ToString("D");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      foreach (XmlNode child in node.ChildNodes)
        try
        {
          writer.Write(
        "      {0}.Controls.Add({1});\n", name, ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseSplitter(XmlNode node, StringWriter writer)
    {
      string name = "splitterpanel" + ControlIndex.ToString("D");
      XmlNodeList panels = node.SelectNodes("Panel");
      if (panels == null || panels.Count != 2)
        throw new Exception("Invalid Splitter Panel layout, must contain exactly two 'Panel' nodes");
      CodeSnippets.Layout.Suspend(writer, name);
      CodeSnippets.Layout.Suspend(writer, name + ".Panel1");
      CodeSnippets.Layout.Suspend(writer, name + ".Panel2");
      ParseContainerAttributes(node, writer, name);
      CodeSnippets.Attributes.Orientation(node, writer, name);
      foreach (XmlNode child in panels[0].ChildNodes)
        try
        {
          writer.Write(
        "      {0}.Panel1.Controls.Add({1});\n", name, ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      foreach (XmlNode child in panels[1].ChildNodes)
        try
        {
          writer.Write(
        "      {0}.Panel2.Controls.Add({1});\n", name, ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      CodeSnippets.Layout.Resume(writer, name + ".Panel1");
      CodeSnippets.Layout.Resume(writer, name + ".Panel2");
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseFlowPanel(XmlNode node, StringWriter writer)
    {
      string name = "flowpanel" + ControlIndex.ToString("D");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseContainerAttributes(node, writer, name);
      //FlowDirection
      //WrapContents
      foreach (XmlNode child in node.ChildNodes)
        try
        {
          writer.Write(
        "      {0}.Controls.Add({1});\n", name, ParseControl(child, writer)
          );
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
  }
}