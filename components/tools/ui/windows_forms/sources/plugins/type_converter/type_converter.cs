namespace tools.ui.windows_forms
{

public static class Converter
{
  static Converter ()
  {
    number_format_provider = new System.Globalization.NumberFormatInfo ();
    
    number_format_provider.NumberDecimalSeparator = ".";
  }

  public static string ToString (string value)
  {
    return value;
  }

  public static string ToString (bool value)
  {
    if (value)
      return "1";
    return "0";
  }

  public static string ToString (int value)
  {
    return System.Convert.ToString (value);
  }

  public static string ToString (float value)
  {
    return System.Convert.ToString (value, number_format_provider);
  }

  public static string ToString (double value)
  {
    return System.Convert.ToString (value, number_format_provider);
  }

  public static string ToString (System.DateTime value)
  {
    return System.Convert.ToString (value);
  }

  public static string ToString (System.Drawing.Color value)
  {
    return System.Convert.ToString (value.R / 255.0, number_format_provider) + "; " + 
           System.Convert.ToString (value.G / 255.0, number_format_provider) + "; " +
           System.Convert.ToString (value.B / 255.0, number_format_provider);
  }

  public static bool ToBool (string value)
  {
    if (value == "0" || value == "false")
      return false;
    return true;
  }

  public static int ToInt (string value)
  {
    return System.Convert.ToInt32 (value);
  }

  public static float ToFloat (string value)
  {
    return System.Convert.ToSingle (value, number_format_provider);
  }

  public static double ToDouble (string value)
  {
    return System.Convert.ToDouble (value, number_format_provider);
  }

  public static System.DateTime ToDateTime (string value)
  {
    return System.Convert.ToDateTime (value);
  }

  public static System.Drawing.Color ToColor (string value)
  {
    string [] color_components = value.Split (color_separators, System.StringSplitOptions.RemoveEmptyEntries);

    if (color_components.Length == 3)
      return System.Drawing.Color.FromArgb (255, (int)(System.Convert.ToSingle (color_components[0], number_format_provider) * 255.0), 
                                            (int)(System.Convert.ToSingle (color_components[1], number_format_provider) * 255.0), 
                                            (int)(System.Convert.ToSingle (color_components[2], number_format_provider) * 255.0));

    if (color_components.Length == 4)
      return System.Drawing.Color.FromArgb ((int)(System.Convert.ToSingle (color_components[3], number_format_provider) * 255.0), 
                                            (int)(System.Convert.ToSingle (color_components[0], number_format_provider) * 255.0), 
                                            (int)(System.Convert.ToSingle (color_components[1], number_format_provider) * 255.0), 
                                            (int)(System.Convert.ToSingle (color_components[2], number_format_provider) * 255.0));

    throw new System.Exception ("Invalid color format");
  }

  private static System.Char []                        color_separators = {';', ' '};
  private static System.Globalization.NumberFormatInfo number_format_provider;
};

}
