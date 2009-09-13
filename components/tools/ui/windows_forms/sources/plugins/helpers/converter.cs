using System;
using System.Drawing;
using System.Collections.Generic;
using System.Windows.Forms;
using System.Globalization;

namespace tools.ui.windows_forms
{
  public static class Converter
  {
    private static NumberFormatInfo float_format_provider = new NumberFormatInfo();
    private static Dictionary<string, string> converters = new Dictionary<string, string>();
    
    static Converter()
    {
      float_format_provider.NumberDecimalSeparator = ".";
      
      converters.Add("string", "ToString");
      converters.Add("bool", "ToBool");
      converters.Add("int", "ToInt");
      converters.Add("float", "ToFloat");
      converters.Add("double", "ToDouble");
      converters.Add("DateTime", "ToDateTime");
      converters.Add("Color", "ToColor");
      converters.Add("AnchorStyles", "ToAnchorStyles");
      converters.Add("DockStyle", "ToDockStyle");
      converters.Add("Size", "ToSize");
      converters.Add("Point", "ToPoint");
      converters.Add("Padding", "ToPadding");
    }
    
    public static string GetConverterName(string typename)
    {
      if (!converters.ContainsKey(typename))
        throw new KeyNotFoundException("Type definition '" + typename + "' not found in converter's dictionary");
      return converters[typename];
    }
    
    //--------------------------------------------------------------------------------
    // Converters to string
    //--------------------------------------------------------------------------------
    public static string ToString (string value)
    {
      return value;
    }
    public static string ToString (bool value)
    {
      return value ? "true" : "false";
    }
    public static string ToString (int value)
    {
      return value.ToString ();
    }
    public static string ToString (float value)
    {
      return value.ToString (float_format_provider);
    }
    public static string ToString (double value)
    {
      return value.ToString (float_format_provider);
    }
    public static string ToString (DateTime value)
    {
      return value.ToString ();
    }
    public static string ToString (decimal value)
    {
      return value.ToString ();
    }
    public static string ToString (System.Drawing.Color value)
    {
      return
        System.Convert.ToString (value.R / 255.0, float_format_provider) + "; " + 
        System.Convert.ToString (value.G / 255.0, float_format_provider) + "; " +
        System.Convert.ToString (value.B / 255.0, float_format_provider);
    }
    //--------------------------------------------------------------------------------
    // Converters from string
    //--------------------------------------------------------------------------------
    public static bool ToBool (string value)
    {
      switch (value.Trim())
      {
        case "True":
        case "true":
        case "T":
        case "t":
        case "Yes":
        case "yes":
        case "Y":
        case "y":
        case "1":
        case "+":
          return true;
        case "False":
        case "false":
        case "F":
        case "f":
        case "No":
        case "no":
        case "N":
        case "n":
        case "0":
        case "-":
          return false;
      }
      throw new InvalidCastException("Error reading bool from string");
    }
    public static int ToInt (string value)
    {
      return Convert.ToInt32 (value);
    }
    public static float ToFloat (string value)
    {
      return Convert.ToSingle (value, float_format_provider);
    }
    public static double ToDouble (string value)
    {
      return Convert.ToDouble (value, float_format_provider);
    }
    public static decimal ToDecimal (string value)
    {
      return Convert.ToDecimal (value, float_format_provider);
    }
    public static System.DateTime ToDateTime (string value)
    {
      return Convert.ToDateTime (value);
    }
    public static Color ToColor (string value)
    {
      value = value.Trim();
      Color c;
      // try to read predefined system color by name
      if ((c = Color.FromName(value)) != Color.Empty)
        return c;
      List<string> elems;
      byte middle;
      try
      {
        switch (value[0])
        {
          case '#':
            //color is described in HTML way, like #RRGGBB or #AARRGGBB
            elems = new List<string>();
            for (int i = 1; i < value.Length; i += 2)
              elems.Add(value.Substring(i, 2));
            switch (elems.Count)
            {
              case 1:
                middle = Convert.ToByte(elems[0], 16);
                return Color.FromArgb(middle, middle, middle);
              case 2:
                middle = Convert.ToByte(elems[1], 16);
                return Color.FromArgb(
                  Convert.ToByte(elems[0], 16),
                  Color.FromArgb(middle, middle, middle));
              case 3:
                return Color.FromArgb(
                  Convert.ToByte(elems[0], 16),
                  Convert.ToByte(elems[1], 16),
                  Convert.ToByte(elems[2], 16));
              case 4:
                return Color.FromArgb(
                  Convert.ToByte(elems[0], 16),
                  Convert.ToByte(elems[1], 16),
                  Convert.ToByte(elems[2], 16),
                  Convert.ToByte(elems[3], 16));
            }
            break;
          case 'c':
          case 'C':
            // color is described in integral notation
            elems = new List<string>(value.Substring(1).Split(" ;,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries));
            switch (elems.Count)
            {
              case 1:
                middle = Convert.ToByte(elems[0], 10);
                return Color.FromArgb(middle, middle, middle);
              case 2:
                middle = Convert.ToByte(elems[1], 10);
                return Color.FromArgb(
                  Convert.ToByte(elems[0], 10),
                  Color.FromArgb(middle, middle, middle));
              case 3:
                return Color.FromArgb(
                  Convert.ToByte(elems[0], 10),
                  Convert.ToByte(elems[1], 10),
                  Convert.ToByte(elems[2], 10));
              case 4:
                return Color.FromArgb(
                  Convert.ToByte(elems[0], 10),
                  Convert.ToByte(elems[1], 10),
                  Convert.ToByte(elems[2], 10),
                  Convert.ToByte(elems[3], 10));
            }
            break;
          default:
            // color is described in float notation
            elems = new List<string>(value.Substring(1).Split(" ;,".ToCharArray(), StringSplitOptions.RemoveEmptyEntries));
            switch (elems.Count)
            {
              case 1:
                middle = Convert.ToByte(Convert.ToSingle(elems[0], float_format_provider) * 255.0);
                return Color.FromArgb(middle, middle, middle);
              case 2:
                middle = Convert.ToByte(Convert.ToSingle(elems[1], float_format_provider) * 255.0);
                return Color.FromArgb(
                  Convert.ToByte(Convert.ToSingle(elems[0], float_format_provider) * 255.0),
                  Color.FromArgb(middle, middle, middle));
              case 3:
                return Color.FromArgb(
                  Convert.ToByte(Convert.ToSingle(elems[0], float_format_provider) * 255.0),
                  Convert.ToByte(Convert.ToSingle(elems[1], float_format_provider) * 255.0),
                  Convert.ToByte(Convert.ToSingle(elems[2], float_format_provider) * 255.0));
              case 4:
                return Color.FromArgb(
                  Convert.ToByte(Convert.ToSingle(elems[0], float_format_provider) * 255.0),
                  Convert.ToByte(Convert.ToSingle(elems[1], float_format_provider) * 255.0),
                  Convert.ToByte(Convert.ToSingle(elems[2], float_format_provider) * 255.0),
                  Convert.ToByte(Convert.ToSingle(elems[3], float_format_provider) * 255.0));
            }
            break;
        }
      }
      catch (Exception e)
      {
        throw new InvalidCastException("Error reading Color from string", e);
      }
      throw new InvalidCastException("Error reading Color from string");
    }
    public static AnchorStyles ToAnchorStyles(string text)
    {
      List<string> styles = new List<string>(text.Trim().Split(" ,;".ToCharArray(), StringSplitOptions.RemoveEmptyEntries));
      AnchorStyles result = new AnchorStyles();
      foreach (string style in styles)
      {
        switch (style)
        {
          case "Left":
            result |= AnchorStyles.Left;
            break;
          case "Top":
            result |= AnchorStyles.Top;
            break;
          case "Right":
            result |= AnchorStyles.Right;
            break;
          case "Bottom":
            result |= AnchorStyles.Bottom;
            break;
          case "None":
            return AnchorStyles.None;
          default:
            throw new InvalidCastException("Error reading Anchor styles from string");
        }
      }
      return result;
    }
    public static DockStyle ToDockStyle(string text)
    {
      switch (text.Trim())
      {
        case "Left":
          return DockStyle.Left;
        case "Top":
          return DockStyle.Top;
        case "Right":
          return DockStyle.Right;
        case "Bottom":
          return DockStyle.Bottom;
        case "Fill":
          return DockStyle.Fill;
        case "None":
          return DockStyle.None;
      }
      throw new InvalidCastException("Error reading DockStyle from string");
    }
    public static Size ToSize(string text)
    {
      List<string> elems = new List<string>(text.Trim().Split(" ,;".ToCharArray(), StringSplitOptions.RemoveEmptyEntries));
      if (elems.Count == 2)
        return new Size(Convert.ToInt32(elems[0], 10), Convert.ToInt32(elems[1], 10));
      throw new InvalidCastException("Error reading Size from string");
    }
    public static Point ToPoint(string text)
    {
      return new Point(ToSize(text));
    }
    public static Padding ToPadding(string text)
    {
      List<string> elems = new List<string>(text.Trim().Split(" ,;".ToCharArray(), StringSplitOptions.RemoveEmptyEntries));
      switch (elems.Count)
      {
        case 1:
          return new Padding(Convert.ToInt32(elems[0], 10));
        case 4:
          return new Padding(
            Convert.ToInt32(elems[0], 10),
            Convert.ToInt32(elems[1], 10),
            Convert.ToInt32(elems[2], 10),
            Convert.ToInt32(elems[3], 10));
      }
      throw new InvalidCastException("Error reading Padding from string");
    }
  }
}
