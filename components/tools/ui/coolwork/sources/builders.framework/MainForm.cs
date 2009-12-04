using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace builders.framework
{
  /// <summary>
  /// Implementation of main editor window
  /// </summary>
  public partial class MainForm : Form
  {
    List<ViewInfo> views; //views of editor (like PropertyGrid, SceneView and others)    

    struct ViewInfo
    {
      public DockContent       view;      //view form
      public ToolStripMenuItem menu_item; //menu item with check box of view
    }

    /// <summary>
    /// Constructor
    /// </summary>
    public MainForm ()
    {
      views = new List<ViewInfo> ();

      InitializeComponent ();

      AddView (new PropertyGrid (), properties_menu_item);
    }

    private void AddView (DockContent view, ToolStripMenuItem menu_item)
    {
      SuspendLayout ();

      ViewInfo view_info = new ViewInfo ();

      view_info.view      = view;
      view_info.menu_item = menu_item;

      menu_item.Checked  = view.DockState != DockState.Hidden;
      menu_item.Click   += OnToogleView;

      view.ShowHint        = WeifenLuo.WinFormsUI.Docking.DockState.Float;
      view.VisibleChanged += OnChangeViewVisibility;
      view.HideOnClose     = true;

      view.Show (dock_panel);
      views.Add (view_info);

      ResumeLayout (false);
    }

    private void OnToogleView (object sender, EventArgs e)
    {
      foreach (ViewInfo info in views)
      {
        if (info.menu_item != sender)
          continue;

        System.Console.WriteLine ("Toogle view {0}", info.view.Text);

        if (info.menu_item.Checked)
        {
            //hide view

          info.menu_item.Checked = false;
          info.view.DockState    = DockState.Hidden;
        }
        else
        {
            //show view

          info.menu_item.Checked = true;
          info.view.DockState    = DockState.Float;
        }

        break;
      }
    }

    private void OnChangeViewVisibility (object sender, EventArgs e)
    {      
      foreach (ViewInfo info in views)
      {
        if (info.view != sender)
          continue;
                
        info.menu_item.Checked = false;

        break;
      }
    }

    private void OnNewScnFile (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnNewScnFile");
    }

    private void OnOpenFile (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnOpenFile");
    }

    private void OnSaveFile (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnSaveFile");
    }

    private void OnSaveAsFile (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnSaveAsFile");
    }

    private void OnSaveAll (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnSaveAll");
    }

    private void OnExit (object sender, EventArgs e)
    {
      System.Windows.Forms.Application.Exit ();
    }

    private void OnUndo (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnUndo");
    }

    private void OnRedo (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnRedo");
    }

    private void OnCut (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnCut");
    }

    private void OnCopy (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnCopy");
    }

    private void OnPaste (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnPaste");
    }

    private void OnDelete (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnDelete");
    }

    private void OnSelectAll (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnSelectAll");
    }

    private void OnFullScreen (object sender, EventArgs e)
    {
      SuspendLayout ();

      if (FormBorderStyle == FormBorderStyle.None)
      {
          //disable full screen

        FormBorderStyle = FormBorderStyle.Sizable;
        WindowState     = FormWindowState.Normal;

        full_screen_menu_disable_menu_item.Visible = false;
        full_screen_menu_item.Checked              = false;
      }
      else
      {
          //enable full screen

        FormBorderStyle = FormBorderStyle.None;
        WindowState     = FormWindowState.Maximized;

        full_screen_menu_disable_menu_item.Visible = true;
        full_screen_menu_item.Checked              = true;
      }

      ResumeLayout (true);
    }

    private void OnAbout (object sender, EventArgs e)
    {
      System.Console.WriteLine ("OnAbout");
    }    
  }
}
