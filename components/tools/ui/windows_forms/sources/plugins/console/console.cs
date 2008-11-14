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
    HISTORY_CAPACITY = 65;

    application_server = in_application_server;

    command_line    = new TextBox ();
    output          = new TextBox ();
    command_history = new System.Collections.ArrayList ();

    Name = "Console";

    command_line.BackColor = Color.Black;
    command_line.ForeColor = Color.White;
    command_line.Dock      = DockStyle.Bottom;        

    command_line.KeyDown += new KeyEventHandler (this.OnCommandLineKeyDown);

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

        command_history.Remove (command_line_text);
        command_history.Insert (0, command_line_text);

        try
        {
          application_server.ExecuteCommand (command_line.Text);
        }
        catch
        {
        }

        command_line.Text = "";

        selected_item = -1;

        if (command_history.Count > HISTORY_CAPACITY)
          command_history.RemoveAt (command_history.Count - 1);

        break;
      }
      case Keys.Up:
        if (selected_item > command_history.Count - 2)
          selected_item = command_history.Count - 1;
        else
          selected_item += 1;

        command_line.Text = command_history[selected_item].ToString ();

        break;
      case Keys.Down:
        if (selected_item < 1)
        {
          selected_item = -1;
          command_line.Text = "";
        }
        else
        {
          selected_item -= 1;
          command_line.Text = command_history[selected_item].ToString ();
        }

        break;
    }
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Члены класса
///////////////////////////////////////////////////////////////////////////////////////////////////
  private IApplicationServer           application_server;
  private TextBox                      output;
  private TextBox                      command_line;
  private int                          selected_item;
  private System.Collections.ArrayList command_history;
  private int                          HISTORY_CAPACITY;
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
