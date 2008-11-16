using System.Drawing;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.Xml;

namespace tools
{

namespace ui
{

namespace windows_forms
{

namespace ui_console_plugin
{

public struct ConsoleControlConfiguration
{
  public Color         background_color;
  public bool          has_background_color;
  public Color         foreground_color;
  public bool          has_foreground_color;
  public int           output_cut_threshold;
  public int           output_cut_to;
  public int           history_capacity;
  public System.String output_var_name;
};

public class ConsoleControl: Control, IPropertyListener
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
  public ConsoleControl (IApplicationServer in_application_server, ConsoleControlConfiguration configuration)
  {
    application_server = in_application_server;

    output_cut_threshold = configuration.output_cut_threshold;
    output_cut_to        = configuration.output_cut_to;

    output_var_name = configuration.output_var_name;

    if (output_var_name.Length > 0)
      application_server.RegisterPropertyListener (output_var_name, this);

    history_capacity   = configuration.history_capacity;

    command_line    = new TextBox ();
    output          = new TextBox ();
    command_history = new System.Collections.ArrayList ();

    Name = "Console";

    if (configuration.has_background_color)
    {
      command_line.BackColor = configuration.background_color;
      output.BackColor       = configuration.background_color;
    }
    
    if (configuration.has_foreground_color)
    {
      command_line.ForeColor = configuration.foreground_color;
      output.ForeColor       = configuration.foreground_color;
    }

    command_line.Dock = DockStyle.Bottom;        

    command_line.KeyDown += new KeyEventHandler (this.OnCommandLineKeyDown);

    output.Multiline  = true;
    output.ScrollBars = ScrollBars.Vertical;
    output.ReadOnly   = true;
    output.Dock       = DockStyle.Fill;

    Controls.Add (output);
    Controls.Add (command_line);
  }

  ~ConsoleControl ()
  {
    if (output_var_name.Length > 0)
      application_server.UnregisterPropertyListener (output_var_name, this);
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

        if (command_history.Count > history_capacity)
          command_history.RemoveAt (command_history.Count - 1);

        break;
      }
      case Keys.Up:
        if (command_history.Count == 0)
          break;

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

  public void OnAddProperty (System.String name)
  {
    UpdateOutput ();
  }
  
  public void OnChangeProperty (System.String name)
  {
    UpdateOutput ();
  }

  public void OnRemoveProperty (System.String name)
  {
  }

  private void UpdateOutput ()
  {
    System.String new_message = null;

    application_server.GetProperty (output_var_name, ref new_message);

    output.Text += System.Environment.NewLine + new_message;

    if (output.Text.Length > output_cut_threshold)
    {
      int cut_end = output.Text.IndexOf (System.Environment.NewLine, output.Text.Length - output_cut_to);

      if (cut_end == -1)
        cut_end = output.Text.Length - output_cut_to;

      output.Text = output.Text.Remove (0, cut_end);
    }

    output.SelectionStart = output.Text.Length;
    output.ScrollToCaret ();
  }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Члены класса
///////////////////////////////////////////////////////////////////////////////////////////////////
  private IApplicationServer           application_server;
  private TextBox                      output;
  private TextBox                      command_line;
  private int                          selected_item;
  private int                          output_cut_threshold;
  private int                          output_cut_to;
  private System.Collections.ArrayList command_history;
  private int                          history_capacity;
  private System.String                output_var_name;
};

}

public class UIConsolePlugin: IPlugin
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
  public UIConsolePlugin (IApplicationServer in_application_server)
  {
    DEFAULT_HISTORY_CAPACITY = 64;

    application_server = in_application_server;

    configuration = new ui_console_plugin.ConsoleControlConfiguration ();

    configuration.has_background_color = false;
    configuration.has_foreground_color = false;
    configuration.output_cut_threshold = 1024*64;
    configuration.output_cut_to        = 1024*63;
    configuration.history_capacity     = DEFAULT_HISTORY_CAPACITY;
    configuration.output_var_name      = "";
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание формы
///////////////////////////////////////////////////////////////////////////////////////////////////
  public System.Windows.Forms.Control CreateControl (System.String init_string)
  {
    return new ui_console_plugin.ConsoleControl (application_server, configuration);
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка xml конфигурации
///////////////////////////////////////////////////////////////////////////////////////////////////
  public void LoadConfiguration (XmlNode xml_configuration)
  {
    XmlNode history_capacity     = xml_configuration.Attributes["HistoryCapacity"];
    XmlNode background_color     = xml_configuration.Attributes["BackgroundColor"];
    XmlNode foreground_color     = xml_configuration.Attributes["ForegroundColor"];
    XmlNode output_cut_threshold = xml_configuration.Attributes["OutputCutThreshold"];
    XmlNode output_cut_to        = xml_configuration.Attributes["OutputCutTo"];
    XmlNode output_var_name      = xml_configuration.Attributes["OutputVarName"];

    ui_console_plugin.ConsoleControlConfiguration new_configuration = new ui_console_plugin.ConsoleControlConfiguration ();

    if (output_var_name == null)
      throw new System.Exception ("No 'OutputVarName' attribute in 'Console' plugin configuration");

    new_configuration.output_var_name = output_var_name.InnerText;

    if (history_capacity == null)
      new_configuration.history_capacity = DEFAULT_HISTORY_CAPACITY;
    else
    {
      int new_history_capacity;

      if (!int.TryParse (history_capacity.InnerText, out new_history_capacity))
        throw new System.Exception ("Invalid 'HistoryCapacity' attribute in 'Console' plugin configuration, must be integer");

      if (new_history_capacity < 0)
        throw new System.Exception ("Invalid 'HistoryCapacity' attribute in 'Console' plugin configuration, must not be less then 0");

      new_configuration.history_capacity = new_history_capacity;
    }

    if (background_color == null)
      new_configuration.has_background_color = false;
    else
    {
      new_configuration.has_background_color = true;
      new_configuration.background_color     = StringToColor (background_color.InnerText);
    }

    if (foreground_color == null)
      new_configuration.has_foreground_color = false;
    else
    {
      new_configuration.has_foreground_color = true;
      new_configuration.foreground_color     = StringToColor (foreground_color.InnerText);
    }

    if ((output_cut_threshold != null) && (output_cut_to != null))
    {
      int dummy;

      if (!int.TryParse (output_cut_threshold.InnerText, out dummy))
        throw new System.Exception ("Invalid 'OutputCutThreshold' attribute in 'Console' plugin configuration, must be integer");

      if (dummy < 0)
        throw new System.Exception ("Invalid 'OutputCutThreshold' attribute in 'Console' plugin configuration, must not be less then 0");
      
      new_configuration.output_cut_threshold = dummy;

      if (!int.TryParse (output_cut_to.InnerText, out dummy))
        throw new System.Exception ("Invalid 'OutputCutTo' attribute in 'Console' plugin configuration, must be integer");

      if (dummy < 0)
        throw new System.Exception ("Invalid 'OutputCutTo' attribute in 'Console' plugin configuration, must not be less then 0");
      
      new_configuration.output_cut_to = dummy;

      if (new_configuration.output_cut_threshold < new_configuration.output_cut_to)
        throw new System.Exception ("Invalid 'OutputCutTo' attribute in 'Console' plugin configuration, must not be greater then 'OutputCutThreshold'");
    }
    else 
    {
      new_configuration.output_cut_threshold = configuration.output_cut_threshold;
      new_configuration.output_cut_to        = configuration.output_cut_to;
    }

    configuration = new_configuration;
  }

  Color StringToColor (System.String color_string)
  {
    Regex regular_expression = new Regex ("\\d\\d*");

    MatchCollection matches = regular_expression.Matches (color_string);

    if (matches.Count == 4)
      return Color.FromArgb (System.Convert.ToByte (matches[3].ToString ()), System.Convert.ToByte (matches[0].ToString ()), 
                             System.Convert.ToByte (matches[1].ToString ()), System.Convert.ToByte (matches[2].ToString ()));

    if (matches.Count == 3)
      return Color.FromArgb (255, System.Convert.ToByte (matches[0].ToString ()), 
                             System.Convert.ToByte (matches[1].ToString ()), System.Convert.ToByte (matches[2].ToString ()));
    
    Color return_value = Color.FromName (color_string);

    if (return_value.A == 0 && return_value.R == 0 && return_value.G == 0 && return_value.B == 0)
      throw new System.Exception ("Invalid color attribute format in 'Console' plugin configuration");

    return return_value;
  }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Члены класса
///////////////////////////////////////////////////////////////////////////////////////////////////
  private IApplicationServer                            application_server;
  private ui_console_plugin.ConsoleControlConfiguration configuration;
  private static int                                    DEFAULT_HISTORY_CAPACITY;
};

public class Plugin
{
  public static void InitPlugin (tools.ui.windows_forms.IApplicationServer application_server)
  {
    application_server.AddPlugin ("Console", new UIConsolePlugin (application_server));
  }
}

}

}

}
