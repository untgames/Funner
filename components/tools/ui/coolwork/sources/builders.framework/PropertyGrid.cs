using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace builders.framework
{
  /// <summary>
  /// View for edit properties of selected objects
  /// </summary>
  public partial class PropertyGrid : DockContent
  {
    /// <summary>
    /// Constructor
    /// </summary>
    public PropertyGrid ()
    {
      InitializeComponent ();
    }
  }
}
