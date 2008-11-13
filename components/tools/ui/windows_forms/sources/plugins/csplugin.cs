using System.Drawing;

namespace tools
{

namespace ui
{

namespace windows_forms
{

public class MyPlugin : IPlugin, IPropertyListener
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени плагина
///////////////////////////////////////////////////////////////////////////////////////////////////
  public MyPlugin ()
  {
    System.Console.WriteLine ("MyPlugin");
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение имени плагина
///////////////////////////////////////////////////////////////////////////////////////////////////
  public System.String GetName ()
  {
    return "MyPlugin";
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание формы
///////////////////////////////////////////////////////////////////////////////////////////////////
  public System.Windows.Forms.Control CreateControl (System.String init_string)
  {
    System.Windows.Forms.Control new_control = new System.Windows.Forms.RichTextBox ();

    new_control.BackColor = Color.Red;

    return new_control;
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка xml конфигурации
///////////////////////////////////////////////////////////////////////////////////////////////////
  public void LoadConfiguration (System.Xml.XmlNode xml_configuration)
  {
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка событий переменных
///////////////////////////////////////////////////////////////////////////////////////////////////
  public void OnAddProperty (System.String name)
  {
    System.Console.WriteLine ("On add property {0}", name);
  }

  public void OnChangeProperty (System.String name)
  {
    System.Console.WriteLine ("On change property {0}", name);
  }

  public void OnRemoveProperty (System.String name)
  {
    System.Console.WriteLine ("On remove property {0}", name);
  }
};

public class Plugin
{
  public static void InitPlugin (tools.ui.windows_forms.IApplicationServer application_server)
  {
    MyPlugin new_plugin = new MyPlugin ();

    application_server.AddPlugin (new_plugin.GetName (), new_plugin);
    application_server.RegisterPropertyListener ("*", new_plugin);
  }
}

}

}

}
