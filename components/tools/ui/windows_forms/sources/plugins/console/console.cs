using System.Drawing;
using System.Windows.Forms;

namespace tools
{

namespace ui
{

namespace windows_forms
{

namespace ui_console_plugin
{

public class ConsoleControl: Control
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
  public ConsoleControl (IApplicationServer in_application_server)
  {
    application_server = in_application_server;

    command_line = new ComboBox ();
    output       = new TextBox ();

    Name = "Console";

    command_line.BackColor = Color.Black;
    command_line.ForeColor = Color.White;
    command_line.Dock      = DockStyle.Bottom;        

    command_line.KeyDown += new KeyEventHandler (this.OnCommandLineKeyDown);

    command_line.Items.Insert (0, "");
    command_line.Items.Insert (1, "");
    command_line.Items.Insert (2, "");
    command_line.Items.Insert (3, "");

    output.BackColor = Color.Black;
    output.ForeColor = Color.White;
    output.Multiline = true;
    output.ReadOnly  = true;
    output.Dock      = DockStyle.Fill;

    Controls.Add (command_line);
    Controls.Add (output);
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка событий
///////////////////////////////////////////////////////////////////////////////////////////////////
  private void OnCommandLineKeyDown (System.Object sender, KeyEventArgs event_args)
  {
    switch (event_args.KeyCode)
    {
      case Keys.Return:
      {
        if (command_line.Text == "")
          break;

        System.String command_line_text = command_line.Text;

        command_line.Items.Remove (command_line.Text);

        command_line.Items.Insert (2, command_line_text);

        try
        {
          application_server.ExecuteCommand (command_line.Text);
        }
        catch
        {
        }

        command_line.Text = "";

        break;
      }
      case Keys.Up:
        if (command_line.SelectedIndex < 1)
        {
          command_line.SelectedIndex = 3;
          return;                    
        }

        if (command_line.SelectedIndex < (command_line.Items.Count - 1))
          command_line.SelectedIndex += 2;
          
        break;
      case Keys.Down:
        if (command_line.SelectedIndex > 0)
          command_line.SelectedIndex -= 2;
  
        break;
    }
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Члены класса
///////////////////////////////////////////////////////////////////////////////////////////////////
  private IApplicationServer application_server;
  private ComboBox           command_line;
  private TextBox            output;
};

}

public class UIConsolePlugin: IPlugin
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
  public UIConsolePlugin (IApplicationServer in_application_server)
  {
    application_server = in_application_server;
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание формы
///////////////////////////////////////////////////////////////////////////////////////////////////
  public System.Windows.Forms.Control CreateControl (System.String init_string)
  {
    return new ui_console_plugin.ConsoleControl (application_server);
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка xml конфигурации
///////////////////////////////////////////////////////////////////////////////////////////////////
  public void LoadConfiguration (System.Xml.XmlNode xml_configuration)
  {
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Члены класса
///////////////////////////////////////////////////////////////////////////////////////////////////
  private IApplicationServer application_server;
};

public class Plugin
{
  public static void InitPlugin (tools.ui.windows_forms.IApplicationServer application_server)
  {
    application_server.AddPlugin ("UIConsolePlugin", new UIConsolePlugin (application_server));
  }
}

}

}

}
