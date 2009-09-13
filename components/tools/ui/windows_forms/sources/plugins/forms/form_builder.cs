using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Xml;
using System.IO;
using System.Reflection;
using Draw = System.Drawing;

namespace tools.ui.windows_forms
{
  public class FormBuilderPlugin: IPlugin
  {
    protected int ControlIndex = 0;
    protected Dictionary<string, Assembly> assemblies = new Dictionary<string, Assembly>();
    protected IApplicationServer server = null;
    public FormBuilderPlugin(IApplicationServer server)
    {
      if (server == null)
        throw new ArgumentNullException("IApplicationServer reference must not be NULL");
      this.server = server;
    }
    public Control CreateControl(string initstring)
    {
      if (!assemblies.ContainsKey(initstring.Trim()))
        throw new KeyNotFoundException("Invalid initialization string, must specify existing form control configuration name");
      return (Control)CodeBuilder.Instantiate(assemblies[initstring.Trim()], "tools.ui.windows_forms.CustomPanel", this.server);
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
      ParseAttributes(node, writer, "this");
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, "this", ParseControl);
      CodeSnippets.Layout.Resume(writer, "this");
      writer.Write(
        "    } // end-of-ctor CustomPanel(IApplicationServer)\n" +
        "  } // end-of-class CustomPanel\n"
      );
      // here we add templates for property listener classes
      writer.Write("{0}\n", CodeSnippets.Properties.Template("CheckBox", "Checked", "bool", "ToBool"));
      writer.Write("{0}\n", CodeSnippets.Properties.Template("Control", "Text", "string", "ToString"));
      writer.Write("{0}\n", CodeSnippets.Properties.Template("NumericUpDown", "Value", "decimal", "ToDecimal"));
      writer.Write("{0}\n", CodeSnippets.Properties.Template("Control", "Visible", "bool", "ToBool"));
      writer.Write("{0}\n", CodeSnippets.Properties.Template("Control", "Enabled", "bool", "ToBool"));
      writer.Write(
        "} // end-of-namespace tools.ui.windows_forms\n"
      );
    }
    private void ParseAttributes(XmlNode node, StringWriter writer, string varname)
    {
      CodeSnippets.Attributes.EnableDoubleBuffered(node, writer, varname);
      CodeSnippets.Attributes.Name(node, writer, varname, varname);
      CodeSnippets.Attributes.Text(node, writer, varname);
      CodeSnippets.Attributes.BackgroundImage(node, writer, varname);
      CodeSnippets.Attributes.Size(node, writer, varname);
      CodeSnippets.Attributes.Location(node, writer, varname);
      //CodeSnippets.Attributes.Padding(node, writer, varname);
      //CodeSnippets.Attributes.Margin(node, writer, varname);
      CodeSnippets.Attributes.ForeColor(node, writer, varname);
      CodeSnippets.Attributes.BackColor(node, writer, varname);
      CodeSnippets.Attributes.AnchorDock(node, writer, varname);
      CodeSnippets.Attributes.Visible(node, writer, varname);
      CodeSnippets.Attributes.Enabled(node, writer, varname);
      CodeSnippets.Attributes.TabStop(node, writer, varname);
      // bind properties to specific watch variables
      CodeSnippets.Common.Bind(node, writer, varname, "Watch.Visible", null, "Control", "Visible");
      CodeSnippets.Common.Bind(node, writer, varname, "Watch.Enabled", null, "Control", "Enabled");
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
        case "Spin":    return ParseNumericSpin(node, writer);
        case "Button":  return ParseButton(node, writer);
        case "Flag":    return ParseCheckBox(node, writer);
        case "Radio":   return ParseRadioButton(node, writer);
        case "List":    return ParseListBox(node, writer);
        case "Combo":   return ParseComboBox(node, writer);
      }
      throw new Exception("Unknown control type");
    }
    private string ParsePanel(XmlNode node, StringWriter writer)
    {
      string name = "panel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "Panel");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      CodeSnippets.Common.AddControls(node.ChildNodes, writer, name, ParseControl);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private string ParseTablePanel(XmlNode node, StringWriter writer)
    {
      string name = "tablepanel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "TableLayoutPanel");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      Draw.Size tablesize = new Draw.Size(1, 1);
      Draw.Point insertpoint = new Draw.Point(0, 0);
      foreach (XmlNode child in node.ChildNodes)
      {
        try
        {
          Draw.Point pt = TablePosAttribute(child);
          Draw.Size maxspan = new Draw.Size(1, 1);
          bool autoinspoint = false;
          switch (child.Name)
          {
            case "Row":
              if (pt.Y < 0)
                autoinspoint = true;
              pt.X = pt.X < 0 ? insertpoint.X : pt.X;
              pt.Y = pt.Y < 0 ? insertpoint.Y : pt.Y;
              foreach (XmlNode item in child.ChildNodes)
                try
                {
                  Draw.Size span = ParseTableElement(item, writer, name, pt);
                  pt.X += span.Width;
                  maxspan.Height = Math.Max(maxspan.Height, span.Height);
                }
                catch (Exception e)
                {
                  Console.WriteLine(e.ToString());
                }
              pt.Y += maxspan.Height;
              if (autoinspoint)
                insertpoint.Y += maxspan.Height;
              break;
            case "Column":
              if (pt.X < 0)
                autoinspoint = true;
              pt.X = pt.X < 0 ? insertpoint.X : pt.X;
              pt.Y = pt.Y < 0 ? insertpoint.Y : pt.Y;
              foreach (XmlNode item in child.ChildNodes)
                try
                {
                  Draw.Size span = ParseTableElement(item, writer, name, pt);
                  pt.Y += span.Height;
                  maxspan.Width = Math.Max(maxspan.Width, span.Width);
                }
                catch (Exception e)
                {
                  Console.WriteLine(e.ToString());
                }
              pt.X += maxspan.Width;
              if (autoinspoint)
                insertpoint.X += maxspan.Width;
              break;
            default:
              maxspan = ParseTableElement(child, writer, name, pt);
              pt = Draw.Point.Add(pt, maxspan);
              break;
          }
          tablesize = new Draw.Size(Math.Max(tablesize.Width, pt.X), Math.Max(tablesize.Height, pt.Y));
        }
        catch (Exception e)
        {
          Console.WriteLine(e.ToString());
        }
      }
      writer.Write(
        "      {0}.ColumnCount = {1:D};\n" +
        "      {0}.RowCount = {2:D};\n" +
        "      foreach (ColumnStyle style in {0}.ColumnStyles)\n" +
        "        style.SizeType = SizeType.AutoSize;\n" +
        "      foreach (RowStyle style in {0}.RowStyles)\n" +
        "        style.SizeType = SizeType.AutoSize;\n" +
        "      for ({0}.ColumnStyles.Count < {0}.ColumnCount)\n" +
        "        {0}.ColumnStyles.Add(new ColumnStyle(SizeType.AutoSize));\n" +
        "      for ({0}.RowStyles.Count < {0}.RowCount)\n" +
        "        {0}.RowStyles.Add(new RowStyle(SizeType.AutoSize));\n",
        name, tablesize.Width, tablesize.Height
      );
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    private Draw.Point TablePosAttribute(XmlNode node)
    {
      Draw.Point tablepos = new Draw.Point(-1, -1);
      try
      {
        if (node.Attributes["Column"] != null)
          tablepos.X = Converter.ToInt(node.Attributes["Column"].InnerText);
      }
      catch (Exception e)
      {
        Console.WriteLine(e.ToString());
      }
      try
      {
        if (node.Attributes["Row"] != null)
          tablepos.Y = Converter.ToInt(node.Attributes["Row"].InnerText);
      }
      catch (Exception e)
      {
        Console.WriteLine(e.ToString());
      }
      return tablepos;
    }
    private Draw.Size ParseTableElement(XmlNode node, StringWriter writer, string owner, Draw.Point location)
    {
      string name = ParseControl(node, writer);
      writer.Write(
        "      {0}.Controls.Add({1}, {2:D}, {3:D});\n",
        owner,
        name,
        location.X,
        location.Y
      );
      Draw.Size sz = new Draw.Size(1, 1);
      if (node.Attributes["Span"] != null)
      {
        sz = Converter.ToSize(node.Attributes["Span"].InnerText);
        writer.Write(
          "      {0}.SetColumnSpan({1}, {2:D});\n      {0}.SetRowSpan({1}, {3:D});\n",
          owner, name, sz.Width, sz.Height
        );
      }
      return sz;
    }
    private string ParseTabPanel(XmlNode node, StringWriter writer)
    {
      string name = "tabpanel" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "TabControl");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
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
      ParseAttributes(node, writer, name);
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
      ParseAttributes(node, writer, name);
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
      ParseAttributes(node, writer, name);
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
      ParseAttributes(node, writer, name);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // button item
    private string ParseButton(XmlNode node, StringWriter writer)
    {
      string name = "button" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "Button");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      CodeSnippets.Events.Click(node, writer, name);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // spin item
    private string ParseNumericSpin(XmlNode node, StringWriter writer)
    {
      string name = "spinbox" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "NumericUpDown");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      CodeSnippets.Attributes.MinimumMaximum(node, writer, name);
      CodeSnippets.Attributes.Increment(node, writer, name);
      CodeSnippets.Common.Bind(node, writer, name, "Variable", "ValueChanged", "NumericUpDown", "Value");
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // input item
    private string ParseInput(XmlNode node, StringWriter writer)
    {
      StringWriter tmpwriter = new StringWriter();
      string name = "input" + ControlIndex.ToString("D");
      bool masked = CodeSnippets.Attributes.Mask(node, tmpwriter, name);
      if (masked)
      {
        CodeSnippets.Common.Instantiate(writer, name, "MaskedTextBox");
        writer.Write(tmpwriter.ToString());
      }
      else
        CodeSnippets.Common.Instantiate(writer, name, "TextBox");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      CodeSnippets.Common.Bind(node, writer, name, "Variable", "TextChanged", "Control", "Text");
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // flag item
    private string ParseCheckBox(XmlNode node, StringWriter writer)
    {
      string name = "checkbox" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "CheckBox");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      CodeSnippets.Common.Bind(node, writer, name, "Variable", "CheckedChanged", "CheckBox", "Checked");
      CodeSnippets.Events.Click(node, writer, name);
      CodeSnippets.Layout.Resume(writer, name);
      return name;
    }
    // listbox item
    private string ParseListBox(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
    // combobox item
    private string ParseComboBox(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
/*      string name = "combobox" + ControlIndex.ToString("D");
      CodeSnippets.Common.Instantiate(writer, name, "ComboBox");
      CodeSnippets.Layout.Suspend(writer, name);
      ParseAttributes(node, writer, name);
      CodeSnippets.Common.Bind(node, writer, name, "Variable", "CheckedChanged", "CheckBox", "Checked");
      CodeSnippets.Layout.Resume(writer, name);
      return name;*/
    }
    // radiobutton item
    private string ParseRadioButton(XmlNode node, StringWriter writer)
    {
      throw new NotImplementedException();
    }
  }
}
