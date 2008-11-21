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

    XmlNodeList property_maps_node_list = xml_configuration.SelectNodes ("PropertyMaps");

    if (property_maps_node_list.Count != 1)
      throw new System.Exception ("Invalid plugin 'PropertyGrid' configuration - must contain one 'PropertyMaps' sub-tag");

    StringWriter generated_code = GenerateCode (property_maps_node_list[0]);

//    System.Console.WriteLine (generated_code.ToString ());

    System.CodeDom.Compiler.CompilerParameters compiler_parameters = new System.CodeDom.Compiler.CompilerParameters ();

    compiler_parameters.GenerateExecutable = false;
    compiler_parameters.GenerateInMemory   = true;
    compiler_parameters.WarningLevel       = 3;
    compiler_parameters.CompilerOptions    = "/target:library /optimize /nologo";

    compiler_parameters.ReferencedAssemblies.Add ("System.dll");
    compiler_parameters.ReferencedAssemblies.Add (Application.StartupPath + "\\" + tools.ui.windows_forms.ui_property_grid_plugin.Constants.PluginApiDllName ());

    Microsoft.CSharp.CSharpCodeProvider     cs_code_provider = new Microsoft.CSharp.CSharpCodeProvider ();    
    System.CodeDom.Compiler.CompilerResults results          = cs_code_provider.CompileAssemblyFromSource (compiler_parameters, generated_code.ToString ());

    System.Collections.Specialized.StringCollection sc = results.Output;
    
    for (int i = 0; i < sc.Count; i++)
      System.Console.WriteLine (sc [i]);  

    assembly = results.CompiledAssembly;
  }

  private void ParseProperty (XmlNode property, StringWriter writer)
  {
    XmlNode name_node     = property.Attributes ["Name"];
    XmlNode var_name_node = property.Attributes ["VarName"];
    XmlNode category_node = property.Attributes ["Category"];
    XmlNode tip_node      = property.Attributes ["Tip"];
    XmlNode title_node    = property.Attributes ["Title"];
    XmlNode type_node     = property.Attributes ["Type"];
    
    if (name_node == null)
      throw new System.Exception ("Invalid 'Property' node, must contain 'Name' attribute");

    if (var_name_node == null)
      throw new System.Exception ("Invalid 'Property' node, must contain 'VarName' attribute");

    writer.Write ("\tprivate System.String {0}_var_name;\n\n", name_node.InnerText);

    writer.Write ("\t[System.ComponentModel.Description (\"{0}\"),\n", (tip_node != null) ? tip_node.InnerText : "");
    writer.Write ("\t System.ComponentModel.DisplayName (\"{0}\"),\n", (title_node != null) ? title_node.InnerText : name_node.InnerText);
    writer.Write ("\t System.ComponentModel.Category    (\"{0}\")]\n", (category_node != null) ? category_node.InnerText : "Common");

    writer.Write ("\tpublic {0} {1}\n\t{{\n", (type_node != null) ? type_node.InnerText : "string", name_node.InnerText);
    
    writer.Write ("\t\tget\n\t\t{\n");
    writer.Write ("\t\t\tif (!application_server.IsPropertyPresent ({0}_var_name))\n", var_name_node.InnerText);
    writer.Write ("\t\t\t\tthrow new System.Exception (\"Variable '\" + {0}_var_name + \"' not found!\");\n", var_name_node.InnerText);
    writer.Write ("System.String return_value = null;\n");
    writer.Write ("\t\t\tapplication_server.GetProperty ({0}_var_name, ref return_value);\n", name_node.InnerText);
    writer.Write ("\t\t\treturn return_value;\n\t\t}\n");

    writer.Write ("\t\tset\n\t\t{{\n\t\t\tapplication_server.SetProperty ({0}_var_name, value);\n\t\t}}\n", name_node.InnerText);
    writer.Write ("\t}\n\n");
  }

  private void ParsePropertyMap (XmlNode property_map, StringWriter writer)
  {
    XmlNode name_node    = property_map.Attributes ["Name"];
    XmlNode inherit_node = property_map.Attributes ["Inherit"];

    if (name_node == null)
      throw new System.Exception ("Invalid 'PropertyMap' node, must contain 'Name' attribute");

    if (inherit_node != null) writer.Write ("public class {0}: {1}, IPropertyListener\n{{\n", name_node.InnerText, inherit_node.InnerText);
    else                      writer.Write ("public class {0}: IPropertyListener\n{{\n", name_node.InnerText);  

    writer.Write ("\tprivate IApplicationServer application_server;\n\n");
    writer.Write ("\tpublic {0} (IApplicationServer in_application_server, System.String var_name_prefix)\n", name_node.InnerText);

    if (inherit_node != null)
      writer.Write ("\t: base (in_application_server, var_name_prefix)\n");    
    
    writer.Write ("\t{\n\t\tapplication_server = in_application_server;\n\t\tInitVars (var_name_prefix);\n\t}\n\n");

    XmlNodeList list = property_map.SelectNodes ("Property");
    
    for (int i = 0; i < list.Count; i++)
      ParseProperty (list [i], writer);

    writer.Write ("\tprivate void InitVars (System.String var_name_prefix)\n\t{\n");
    writer.Write ("\t\tif (var_name_prefix.Length > 0)\n\t\t\tvar_name_prefix += \".\";\n");

    for (int i = 0; i < list.Count; i++)
      writer.Write ("\t\t{0}_var_name = var_name_prefix + \"{0}\";\n", list[i].Attributes["VarName"].InnerText);
    
    writer.Write ("\t}\n\n");

    writer.Write ("\tpublic new void OnAddProperty (System.String property_name)\n\t{\n\t}\n\n");
    writer.Write ("\tpublic new void OnChangeProperty (System.String property_name)\n\t{\n\t}\n\n");
    writer.Write ("\tpublic new void OnRemoveProperty (System.String property_name)\n\t{\n\t}\n\n");

    writer.Write ("};\n\n");
  }

  private StringWriter GenerateCode (XmlNode property_maps)
  {
    StringWriter string_writer = new StringWriter ();

    XmlNodeList list = property_maps.SelectNodes ("PropertyMap");
    
    string_writer.Write ("namespace tools\n{\n\nnamespace ui\n{\n\nnamespace windows_forms\n{\n\nnamespace ui_property_grid_plugin\n{\n\n");
    
    for (int i = 0; i < list.Count; i++)
      ParsePropertyMap (list [i], string_writer);
      
    string_writer.Write ("}\n\n}\n\n}\n\n}\n\n");

    return string_writer;
  }
  
///Члены класса
  private IApplicationServer                                       application_server;
  private System.Reflection.Assembly                               assembly;
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
