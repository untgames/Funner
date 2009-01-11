using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.Xml;

namespace tools
{

namespace ui
{

namespace windows_forms
{

namespace ui_property_grid_plugin
{

class Constants
{
  public static System.String PluginApiDllName ()
  {
    return "funner.tools.ui.windows_forms.PluginApi.dll";
  }

  public static System.String TypeConverterDllName ()
  {
    return "funner.tools.ui.windows_forms.Helpers.dll";
//    return "funner.tools.ui.windows_forms.Helpers.dll";
  }
};

public class PropertyGridControl: PropertyGrid, IPropertyListener
{
///Конструктор
  public PropertyGridControl (IApplicationServer in_application_server, System.Reflection.Assembly assembly,
                              System.String property_map, System.String var_registry_branch)
  {
    application_server = in_application_server;

    Name = property_map;

    System.Object [] arguments = {application_server, var_registry_branch};

    System.Object map = assembly.CreateInstance ("tools.ui.windows_forms.ui_property_grid_plugin." + property_map, false,
                                                 System.Reflection.BindingFlags.Default, null, arguments, null, null);

    if (map == null)
    {
      System.String exception = "Can't create PropertyGrid for property map " + property_map;

      throw new System.Exception (exception);
    }

    SelectedObject = map;

    var_mask = var_registry_branch;

    if (var_mask.Length == 0)
      var_mask = "*";
    else
      var_mask = var_mask + ".*";

    application_server.RegisterPropertyListener (var_mask, this);
  }

  ~PropertyGridControl ()
  {
    application_server.UnregisterPropertyListener (var_mask, this);
  }

///События
  public void OnAddProperty (System.String name)
  {
    Refresh ();
  }

  public void OnChangeProperty (System.String name)
  {
    Refresh ();
  }

  public void OnRemoveProperty (System.String name)
  {
  }

///Члены класса
  private IApplicationServer application_server;
  private System.String      var_mask;
};

}

public class UIPropertyGridPlugin: IPlugin
{
///Конструктор
  public UIPropertyGridPlugin (IApplicationServer in_application_server)
  {
    application_server = in_application_server;
    assembly           = null;

    type_conversion_match = new System.Collections.Hashtable ();

    type_conversion_match.Add ("string",               "ToString");
    type_conversion_match.Add ("list",                 "ToString");
    type_conversion_match.Add ("bool",                 "ToBool");
    type_conversion_match.Add ("int",                  "ToInt");
    type_conversion_match.Add ("float",                "ToFloat");
    type_conversion_match.Add ("double",               "ToDouble");
    type_conversion_match.Add ("System.DateTime",      "ToDateTime");
    type_conversion_match.Add ("System.Drawing.Color", "ToColor");
  }

///Создание формы
  public System.Windows.Forms.Control CreateControl (System.String init_string)
  {
    if (assembly == null)
      throw new System.Exception ("Can't create control, configuration not loaded");

    Regex regular_expression = new Regex ("[A-Za-z]+[A-Za-z0-9_\\.]*");

    MatchCollection matches = regular_expression.Matches (init_string);

    if ((matches.Count == 0) || (matches.Count > 2))
      throw new System.Exception ("Invalid PropertyGrid init_string, format: property_map_name [var_registry_branch]");

    if (matches.Count == 1)
      return new ui_property_grid_plugin.PropertyGridControl (application_server, assembly, matches[0].ToString (), "");
    else // (matches.Count == 2)
      return new ui_property_grid_plugin.PropertyGridControl (application_server, assembly, matches[0].ToString (), matches[1].ToString ());
  }

///Загрузка xml конфигурации
  public void LoadConfiguration (XmlNode xml_configuration)
  {
    assembly = null;

    StringWriter generated_code = GenerateCode (xml_configuration);

//    System.Console.WriteLine (generated_code.ToString ());

    System.CodeDom.Compiler.CompilerParameters compiler_parameters = new System.CodeDom.Compiler.CompilerParameters ();

    compiler_parameters.GenerateExecutable = false;
    compiler_parameters.GenerateInMemory   = true;
    compiler_parameters.WarningLevel       = 3;
    compiler_parameters.CompilerOptions    = "/target:library /optimize /nologo";

    compiler_parameters.ReferencedAssemblies.Add ("System.dll");
    compiler_parameters.ReferencedAssemblies.Add ("System.Drawing.dll");
    compiler_parameters.ReferencedAssemblies.Add (Application.StartupPath + "\\" + tools.ui.windows_forms.ui_property_grid_plugin.Constants.TypeConverterDllName ());
    compiler_parameters.ReferencedAssemblies.Add (Application.StartupPath + "\\" + tools.ui.windows_forms.ui_property_grid_plugin.Constants.PluginApiDllName ());

    Microsoft.CSharp.CSharpCodeProvider     cs_code_provider = new Microsoft.CSharp.CSharpCodeProvider ();
    System.CodeDom.Compiler.CompilerResults results          = cs_code_provider.CompileAssemblyFromSource (compiler_parameters, generated_code.ToString ());

    System.Collections.Specialized.StringCollection sc = results.Output;

    for (int i = 0; i < sc.Count; i++)
      System.Console.WriteLine (sc [i]);

    assembly = results.CompiledAssembly;
  }

  public void ParseProperty (XmlNode property, int property_index, System.String map_name, StringWriter writer)
  {
    XmlNode name_node      = property.Attributes ["Name"];
    XmlNode var_name_node  = property.Attributes ["VarName"];
    XmlNode category_node  = property.Attributes ["Category"];
    XmlNode tip_node       = property.Attributes ["Tip"];
    XmlNode type_node      = property.Attributes ["Type"];
    XmlNode read_only_node = property.Attributes ["ReadOnly"];
    XmlNode list_box_node  = property.Attributes ["ListBox"];

    if (name_node == null)
      throw new System.Exception ("Invalid 'Property' node, must contain 'Name' attribute");

    if (var_name_node == null)
      throw new System.Exception ("Invalid 'Property' node, must contain 'VarName' attribute");

    System.String type = "string";

    if (type_node != null)
    {
      if (type_conversion_match.Contains (type_node.InnerText))
        type = type_node.InnerText;
      else
      {
        string exception_string = "Invalid 'Property' node, unknown attribute 'Type' value: " + type_node.InnerText;

        throw new System.Exception (exception_string);
      }
    }

    if (type == "list" && list_box_node == null)
      throw new System.Exception ("Property with type 'list' must contain 'ListBox' attribute");

    writer.Write ("\tprivate System.String {0}_property_{1}_var_name;\n\n", map_name, property_index);

    writer.Write ("\t[System.ComponentModel.Description (\"{0}\"),\n", (tip_node != null) ? tip_node.InnerText : "");

    if (type == "list")
      writer.Write ("\tSystem.ComponentModel.TypeConverter (typeof ({0}Converter)),\n", list_box_node.InnerText);

    writer.Write ("\t System.ComponentModel.DisplayName (\"{0}\"),\n", name_node.InnerText);
    writer.Write ("\t System.ComponentModel.Category    (\"{0}\")]\n", (category_node != null) ? category_node.InnerText : "Common");

    if (type == "list")
      writer.Write ("\tpublic string {0}Property{1}\n\t{{\n", map_name, property_index);
    else
      writer.Write ("\tpublic {0} {1}Property{2}\n\t{{\n", type, map_name, property_index);

    writer.Write ("\t\tget\n\t\t{\n");
    writer.Write ("\t\t\tif (!application_server.IsPropertyPresent ({0}_property_{1}_var_name))\n", map_name, property_index);
    writer.Write ("\t\t\t\tthrow new System.Exception (\"Variable '\" + {0}_property_{1}_var_name + \"' not found!\");\n", map_name, property_index);
    writer.Write ("\t\t\t\tSystem.String return_value = null;\n");
    writer.Write ("\t\t\tapplication_server.GetProperty ({0}_property_{1}_var_name, ref return_value);\n", map_name, property_index);

    if (type == "list")
      writer.Write ("\t\t\treturn List{0}Table.GetItemByValue (return_value);\n\t\t}}\n", list_box_node.InnerText);
    else
      writer.Write ("\t\t\treturn Converter.{0} (return_value);\n\t\t}}\n", type_conversion_match[type]);

    if ((read_only_node == null) || (read_only_node.InnerText == "0"))
    {
      if (type == "list")
        writer.Write ("\t\tset\n\t\t{{\n\t\t\tapplication_server.SetProperty ({0}_property_{1}_var_name, List{2}Table.GetValueByItem (value));\n\t\t}}\n",
                      map_name, property_index, list_box_node.InnerText);
      else
        writer.Write ("\t\tset\n\t\t{{\n\t\t\tapplication_server.SetProperty ({0}_property_{1}_var_name, Converter.ToString (value));\n\t\t}}\n",
                      map_name, property_index);
    }

    writer.Write ("\t}\n\n");
  }

  private void ParsePropertyMap (XmlNode property_map, StringWriter writer)
  {
    XmlNode name_node    = property_map.Attributes ["Name"];
    XmlNode inherit_node = property_map.Attributes ["Inherit"];

    if (name_node == null)
      throw new System.Exception ("Invalid 'PropertyMap' node, must contain 'Name' attribute");

    if (inherit_node != null) writer.Write ("public class {0}: {1}\n{{\n", name_node.InnerText, inherit_node.InnerText);
    else                      writer.Write ("public class {0}\n{{\n", name_node.InnerText);

    writer.Write ("\tprivate IApplicationServer application_server;\n\n");
    writer.Write ("\tpublic {0} (IApplicationServer in_application_server, System.String var_name_prefix)\n", name_node.InnerText);

    if (inherit_node != null)
      writer.Write ("\t: base (in_application_server, var_name_prefix)\n");

    writer.Write ("\t{\n\t\tapplication_server = in_application_server;\n\t\tInitVars (var_name_prefix);\n\t}\n\n");

    XmlNodeList list = property_map.SelectNodes ("Property");

    for (int i = 0; i < list.Count; i++)
      ParseProperty (list [i], i, name_node.InnerText, writer);

    writer.Write ("\tprivate void InitVars (System.String var_name_prefix)\n\t{\n");
    writer.Write ("\t\tif (var_name_prefix.Length > 0)\n\t\t\tvar_name_prefix += \".\";\n");

    for (int i = 0; i < list.Count; i++)
      writer.Write ("\t\t{0}_property_{1}_var_name = var_name_prefix + \"{2}\";\n", name_node.InnerText, i, list[i].Attributes["VarName"].InnerText);

    writer.Write ("\t}\n\n");

    writer.Write ("};\n\n");
  }

  private void ParseListBox (XmlNode list_box, StringWriter writer)
  {
    XmlNode name_node = list_box.Attributes ["Name"];

    if (name_node == null)
      throw new System.Exception ("Invalid 'ListBox' node, must contain 'Name' attribute");

    writer.Write ("public class {0}Converter: System.ComponentModel.StringConverter\n", name_node.InnerText);
    writer.Write ("{\n\tpublic override bool GetStandardValuesSupported (System.ComponentModel.ITypeDescriptorContext dummy)\n\t{\n\t\treturn true;\n\t}\n\n");
    writer.Write ("\tpublic override System.ComponentModel.TypeConverter.StandardValuesCollection GetStandardValues (System.ComponentModel.ITypeDescriptorContext dummy)\n\t{\n");

    XmlNodeList list = list_box.SelectNodes ("Item");

    writer.Write ("\t\tstring [] list_items = new string [{0}];\n", list.Count);

    for (int i = 0; i < list.Count; i++)
    {
      XmlNode item_name_node  = list[i].Attributes ["Name"];
      XmlNode item_value_node = list[i].Attributes ["Value"];

      if (item_name_node == null || item_value_node == null)
        throw new System.Exception ("Invalid 'ListBox' 'Item' sub-tag, must contain 'Name' and 'Value' attributes");

      writer.Write ("\t\tlist_items[{0}] = \"{1}\";\n", i, item_name_node.InnerText);
    }

    writer.Write ("\t\treturn new System.ComponentModel.TypeConverter.StandardValuesCollection (list_items);\n\t}\n");

    writer.Write ("};\n\n");

    writer.Write ("class List{0}Table\n", name_node.InnerText);

    writer.Write ("{{\n\tstatic private ListBoxItem [] items = new ListBoxItem [{0}] {{", list.Count);
    for (int i = 0; i < list.Count; i++)
    {
      writer.Write (" new ListBoxItem (\"{0}\", \"{1}\")", list[i].Attributes["Name"].InnerText, list[i].Attributes["Value"].InnerText);
      if (i < (list.Count - 1))
        writer.Write (",");
    }
    writer.Write (" };\n\n");

    writer.Write ("\tstatic public string GetItemByValue (string value)\n\t{\n");
    writer.Write ("\t\tfor (int i = 0; i < items.Length; i++)\n");
    writer.Write ("\t\t{\n\t\t\tif (value == items[i].item_value) return items[i].item_name;\n\t\t}\n");
    writer.Write ("\t\tthrow new System.Exception (\"ListBox '{0}' doesn't contain item with value '\" + value + \"'\");\n", name_node.InnerText);
    writer.Write ("\t}\n\n");

    writer.Write ("\tstatic public string GetValueByItem (string item)\n\t{\n");
    writer.Write ("\t\tfor (int i = 0; i < items.Length; i++)\n");
    writer.Write ("\t\t{\n\t\t\tif (item == items[i].item_name) return items[i].item_value;\n\t\t}\n");
    writer.Write ("\t\tthrow new System.Exception (\"ListBox '{0}' doesn't contain item with name '\" + item + \"'\");\n", name_node.InnerText);
    writer.Write ("\t}\n\n");

    writer.Write ("};\n\n");
  }

  private StringWriter GenerateCode (XmlNode property_maps)
  {
    StringWriter string_writer = new StringWriter ();

    string_writer.Write ("namespace tools\n{\n\nnamespace ui\n{\n\nnamespace windows_forms\n{\n\nnamespace ui_property_grid_plugin\n{\n\n");

    string_writer.Write ("struct ListBoxItem\n{\n\tpublic string item_name;\n\tpublic string item_value;\n\n");
    string_writer.Write ("\tpublic ListBoxItem (string in_item_name, string in_item_value)\n\t{\n\t\titem_name = in_item_name;\n\t\titem_value = in_item_value;\n\t}\n");
    string_writer.Write ("};\n\n");

    XmlNodeList list = property_maps.SelectNodes ("ListBox");

    for (int i = 0; i < list.Count; i++)
      ParseListBox (list [i], string_writer);

    list = property_maps.SelectNodes ("PropertyMap");

    for (int i = 0; i < list.Count; i++)
      ParsePropertyMap (list [i], string_writer);

    string_writer.Write ("}\n\n}\n\n}\n\n}\n\n");

    return string_writer;
  }

///Члены класса
  private IApplicationServer           application_server;
  private System.Reflection.Assembly   assembly;
  private System.Collections.Hashtable type_conversion_match;
};

public class Plugin
{
  public static void InitPlugin (tools.ui.windows_forms.IApplicationServer application_server)
  {
    application_server.AddPlugin ("PropertyGrid", new UIPropertyGridPlugin (application_server));
  }
}

}

}

}
