namespace builders.framework
{
  partial class MainForm
  {
    /// <summary>
    /// Required designer variable.
    /// </summary>
    private System.ComponentModel.IContainer components = null;

    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
    protected override void Dispose (bool disposing)
    {
      if (disposing && (components != null))
      {
        components.Dispose ();
      }
      base.Dispose (disposing);
    }

    #region Windows Form Designer generated code

    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.main_menu = new System.Windows.Forms.MenuStrip ();
      this.file_menu = new System.Windows.Forms.ToolStripMenuItem ();
      this.new_scn_file_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.open_file_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.file_menu_separator1 = new System.Windows.Forms.ToolStripSeparator ();
      this.save_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.save_as_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.save_all_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.file_menu_separator2 = new System.Windows.Forms.ToolStripSeparator ();
      this.exit_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.edit_menu = new System.Windows.Forms.ToolStripMenuItem ();
      this.undo_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.redo_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.edit_menu_separator1 = new System.Windows.Forms.ToolStripSeparator ();
      this.cut_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.copy_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.paste_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.delete_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.edit_menu_separator3 = new System.Windows.Forms.ToolStripSeparator ();
      this.select_all_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.view_menu = new System.Windows.Forms.ToolStripMenuItem ();
      this.properties_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.view_menu_separator1 = new System.Windows.Forms.ToolStripSeparator ();
      this.toolbars_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.full_screen_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.window_menu = new System.Windows.Forms.ToolStripMenuItem ();
      this.help_menu = new System.Windows.Forms.ToolStripMenuItem ();
      this.about_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.full_screen_menu_disable_menu_item = new System.Windows.Forms.ToolStripMenuItem ();
      this.status_bar = new System.Windows.Forms.StatusStrip ();
      this.status_label = new System.Windows.Forms.ToolStripStatusLabel ();
      this.tool_strip_container = new System.Windows.Forms.ToolStripContainer ();
      this.dock_panel = new WeifenLuo.WinFormsUI.Docking.DockPanel ();
      this.standard_tool_bar = new System.Windows.Forms.ToolStrip ();
      this.new_scn_file_button = new System.Windows.Forms.ToolStripButton ();
      this.open_file_button = new System.Windows.Forms.ToolStripButton ();
      this.save_file_button = new System.Windows.Forms.ToolStripButton ();
      this.save_all_button = new System.Windows.Forms.ToolStripButton ();
      this.standard_tool_bar_separator1 = new System.Windows.Forms.ToolStripSeparator ();
      this.main_menu.SuspendLayout ();
      this.status_bar.SuspendLayout ();
      this.tool_strip_container.ContentPanel.SuspendLayout ();
      this.tool_strip_container.TopToolStripPanel.SuspendLayout ();
      this.tool_strip_container.SuspendLayout ();
      this.standard_tool_bar.SuspendLayout ();
      this.SuspendLayout ();
      // 
      // main_menu
      // 
      this.main_menu.Items.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.file_menu,
            this.edit_menu,
            this.view_menu,
            this.window_menu,
            this.help_menu,
            this.full_screen_menu_disable_menu_item});
      this.main_menu.Location = new System.Drawing.Point (0, 0);
      this.main_menu.MdiWindowListItem = this.window_menu;
      this.main_menu.Name = "main_menu";
      this.main_menu.Size = new System.Drawing.Size (1008, 24);
      this.main_menu.TabIndex = 0;
      // 
      // file_menu
      // 
      this.file_menu.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.new_scn_file_menu_item,
            this.open_file_menu_item,
            this.file_menu_separator1,
            this.save_menu_item,
            this.save_as_menu_item,
            this.save_all_menu_item,
            this.file_menu_separator2,
            this.exit_menu_item});
      this.file_menu.Name = "file_menu";
      this.file_menu.Size = new System.Drawing.Size (37, 20);
      this.file_menu.Text = "File";
      // 
      // new_scn_file_menu_item
      // 
      this.new_scn_file_menu_item.Image = global::builders.framework.Properties.Resources.NewDocumentIcon;
      this.new_scn_file_menu_item.Name = "new_scn_file_menu_item";
      this.new_scn_file_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.N)));
      this.new_scn_file_menu_item.Size = new System.Drawing.Size (186, 22);
      this.new_scn_file_menu_item.Text = "New SCN file";
      this.new_scn_file_menu_item.Click += new System.EventHandler (this.OnNewScnFile);
      // 
      // open_file_menu_item
      // 
      this.open_file_menu_item.Image = global::builders.framework.Properties.Resources.OpenFileIcon;
      this.open_file_menu_item.Name = "open_file_menu_item";
      this.open_file_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.O)));
      this.open_file_menu_item.Size = new System.Drawing.Size (186, 22);
      this.open_file_menu_item.Text = "Open file...";
      this.open_file_menu_item.Click += new System.EventHandler (this.OnOpenFile);
      // 
      // file_menu_separator1
      // 
      this.file_menu_separator1.Name = "file_menu_separator1";
      this.file_menu_separator1.Size = new System.Drawing.Size (183, 6);
      // 
      // save_menu_item
      // 
      this.save_menu_item.Image = global::builders.framework.Properties.Resources.SaveFileIcon;
      this.save_menu_item.Name = "save_menu_item";
      this.save_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.S)));
      this.save_menu_item.Size = new System.Drawing.Size (186, 22);
      this.save_menu_item.Text = "Save";
      this.save_menu_item.Click += new System.EventHandler (this.OnSaveFile);
      // 
      // save_as_menu_item
      // 
      this.save_as_menu_item.Name = "save_as_menu_item";
      this.save_as_menu_item.Size = new System.Drawing.Size (186, 22);
      this.save_as_menu_item.Text = "Save as...";
      this.save_as_menu_item.Click += new System.EventHandler (this.OnSaveAsFile);
      // 
      // save_all_menu_item
      // 
      this.save_all_menu_item.Image = global::builders.framework.Properties.Resources.SaveAllIcon;
      this.save_all_menu_item.Name = "save_all_menu_item";
      this.save_all_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)(((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Shift)
                  | System.Windows.Forms.Keys.S)));
      this.save_all_menu_item.Size = new System.Drawing.Size (186, 22);
      this.save_all_menu_item.Text = "Save all";
      this.save_all_menu_item.Click += new System.EventHandler (this.OnSaveAll);
      // 
      // file_menu_separator2
      // 
      this.file_menu_separator2.Name = "file_menu_separator2";
      this.file_menu_separator2.Size = new System.Drawing.Size (183, 6);
      // 
      // exit_menu_item
      // 
      this.exit_menu_item.Name = "exit_menu_item";
      this.exit_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Alt | System.Windows.Forms.Keys.X)));
      this.exit_menu_item.Size = new System.Drawing.Size (186, 22);
      this.exit_menu_item.Text = "Exit";
      this.exit_menu_item.Click += new System.EventHandler (this.OnExit);
      // 
      // edit_menu
      // 
      this.edit_menu.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.undo_menu_item,
            this.redo_menu_item,
            this.edit_menu_separator1,
            this.cut_menu_item,
            this.copy_menu_item,
            this.paste_menu_item,
            this.delete_menu_item,
            this.edit_menu_separator3,
            this.select_all_menu_item});
      this.edit_menu.Name = "edit_menu";
      this.edit_menu.Size = new System.Drawing.Size (39, 20);
      this.edit_menu.Text = "Edit";
      // 
      // undo_menu_item
      // 
      this.undo_menu_item.Image = global::builders.framework.Properties.Resources.EditUndoIcon;
      this.undo_menu_item.Name = "undo_menu_item";
      this.undo_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
      this.undo_menu_item.Size = new System.Drawing.Size (162, 22);
      this.undo_menu_item.Text = "Undo";
      this.undo_menu_item.Click += new System.EventHandler (this.OnUndo);
      // 
      // redo_menu_item
      // 
      this.redo_menu_item.Image = global::builders.framework.Properties.Resources.EditRedoIcon;
      this.redo_menu_item.Name = "redo_menu_item";
      this.redo_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Y)));
      this.redo_menu_item.Size = new System.Drawing.Size (162, 22);
      this.redo_menu_item.Text = "Redo";
      this.redo_menu_item.Click += new System.EventHandler (this.OnRedo);
      // 
      // edit_menu_separator1
      // 
      this.edit_menu_separator1.Name = "edit_menu_separator1";
      this.edit_menu_separator1.Size = new System.Drawing.Size (159, 6);
      // 
      // cut_menu_item
      // 
      this.cut_menu_item.Image = global::builders.framework.Properties.Resources.CutIcon;
      this.cut_menu_item.Name = "cut_menu_item";
      this.cut_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.X)));
      this.cut_menu_item.Size = new System.Drawing.Size (162, 22);
      this.cut_menu_item.Text = "Cut";
      this.cut_menu_item.Click += new System.EventHandler (this.OnCut);
      // 
      // copy_menu_item
      // 
      this.copy_menu_item.Image = global::builders.framework.Properties.Resources.CopyIcon;
      this.copy_menu_item.Name = "copy_menu_item";
      this.copy_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.C)));
      this.copy_menu_item.Size = new System.Drawing.Size (162, 22);
      this.copy_menu_item.Text = "Copy";
      this.copy_menu_item.Click += new System.EventHandler (this.OnCopy);
      // 
      // paste_menu_item
      // 
      this.paste_menu_item.Image = global::builders.framework.Properties.Resources.PasteIcon;
      this.paste_menu_item.Name = "paste_menu_item";
      this.paste_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.V)));
      this.paste_menu_item.Size = new System.Drawing.Size (162, 22);
      this.paste_menu_item.Text = "Paste";
      this.paste_menu_item.Click += new System.EventHandler (this.OnPaste);
      // 
      // delete_menu_item
      // 
      this.delete_menu_item.Image = global::builders.framework.Properties.Resources.DeleteIcon;
      this.delete_menu_item.Name = "delete_menu_item";
      this.delete_menu_item.ShortcutKeys = System.Windows.Forms.Keys.Delete;
      this.delete_menu_item.Size = new System.Drawing.Size (162, 22);
      this.delete_menu_item.Text = "Delete";
      this.delete_menu_item.Click += new System.EventHandler (this.OnDelete);
      // 
      // edit_menu_separator3
      // 
      this.edit_menu_separator3.Name = "edit_menu_separator3";
      this.edit_menu_separator3.Size = new System.Drawing.Size (159, 6);
      // 
      // select_all_menu_item
      // 
      this.select_all_menu_item.Name = "select_all_menu_item";
      this.select_all_menu_item.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.A)));
      this.select_all_menu_item.Size = new System.Drawing.Size (162, 22);
      this.select_all_menu_item.Text = "Select all";
      this.select_all_menu_item.Click += new System.EventHandler (this.OnSelectAll);
      // 
      // view_menu
      // 
      this.view_menu.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.properties_menu_item,
            this.view_menu_separator1,
            this.toolbars_menu_item,
            this.full_screen_menu_item});
      this.view_menu.Name = "view_menu";
      this.view_menu.Size = new System.Drawing.Size (44, 20);
      this.view_menu.Text = "View";
      // 
      // properties_menu_item
      // 
      this.properties_menu_item.Image = global::builders.framework.Properties.Resources.PropertiesIcon;
      this.properties_menu_item.Name = "properties_menu_item";
      this.properties_menu_item.ShortcutKeys = System.Windows.Forms.Keys.F4;
      this.properties_menu_item.Size = new System.Drawing.Size (156, 22);
      this.properties_menu_item.Text = "Properties";
      // 
      // view_menu_separator1
      // 
      this.view_menu_separator1.Name = "view_menu_separator1";
      this.view_menu_separator1.Size = new System.Drawing.Size (153, 6);
      // 
      // toolbars_menu_item
      // 
      this.toolbars_menu_item.Name = "toolbars_menu_item";
      this.toolbars_menu_item.Size = new System.Drawing.Size (156, 22);
      this.toolbars_menu_item.Text = "Toolbars";
      // 
      // full_screen_menu_item
      // 
      this.full_screen_menu_item.Image = global::builders.framework.Properties.Resources.FullScreenIcon;
      this.full_screen_menu_item.Name = "full_screen_menu_item";
      this.full_screen_menu_item.ShortcutKeys = System.Windows.Forms.Keys.F12;
      this.full_screen_menu_item.Size = new System.Drawing.Size (156, 22);
      this.full_screen_menu_item.Text = "Full Screen";
      this.full_screen_menu_item.Click += new System.EventHandler (this.OnFullScreen);
      // 
      // window_menu
      // 
      this.window_menu.Name = "window_menu";
      this.window_menu.Size = new System.Drawing.Size (63, 20);
      this.window_menu.Text = "Window";
      // 
      // help_menu
      // 
      this.help_menu.DropDownItems.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.about_menu_item});
      this.help_menu.Name = "help_menu";
      this.help_menu.Size = new System.Drawing.Size (44, 20);
      this.help_menu.Text = "Help";
      // 
      // about_menu_item
      // 
      this.about_menu_item.Name = "about_menu_item";
      this.about_menu_item.Size = new System.Drawing.Size (107, 22);
      this.about_menu_item.Text = "About";
      this.about_menu_item.Click += new System.EventHandler (this.OnAbout);
      // 
      // full_screen_menu_disable_menu_item
      // 
      this.full_screen_menu_disable_menu_item.BackColor = System.Drawing.SystemColors.GradientActiveCaption;
      this.full_screen_menu_disable_menu_item.Image = global::builders.framework.Properties.Resources.FullScreenIcon;
      this.full_screen_menu_disable_menu_item.Name = "full_screen_menu_disable_menu_item";
      this.full_screen_menu_disable_menu_item.Size = new System.Drawing.Size (92, 20);
      this.full_screen_menu_disable_menu_item.Text = "Full Screen";
      this.full_screen_menu_disable_menu_item.Visible = false;
      this.full_screen_menu_disable_menu_item.Click += new System.EventHandler (this.OnFullScreen);
      // 
      // status_bar
      // 
      this.status_bar.Items.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.status_label});
      this.status_bar.Location = new System.Drawing.Point (0, 708);
      this.status_bar.Name = "status_bar";
      this.status_bar.Size = new System.Drawing.Size (1008, 22);
      this.status_bar.TabIndex = 4;
      this.status_bar.Text = "status_bar";
      // 
      // status_label
      // 
      this.status_label.Name = "status_label";
      this.status_label.Size = new System.Drawing.Size (0, 17);
      // 
      // tool_strip_container
      // 
      // 
      // tool_strip_container.ContentPanel
      // 
      this.tool_strip_container.ContentPanel.Controls.Add (this.dock_panel);
      this.tool_strip_container.ContentPanel.Size = new System.Drawing.Size (983, 659);
      this.tool_strip_container.Dock = System.Windows.Forms.DockStyle.Fill;
      this.tool_strip_container.Location = new System.Drawing.Point (0, 24);
      this.tool_strip_container.Name = "tool_strip_container";
      this.tool_strip_container.Size = new System.Drawing.Size (1008, 684);
      this.tool_strip_container.TabIndex = 7;
      this.tool_strip_container.Text = "toolStripContainer1";
      // 
      // tool_strip_container.TopPanel
      // 
      this.tool_strip_container.TopToolStripPanel.Controls.Add (this.standard_tool_bar);
      // 
      // dock_panel
      // 
      this.dock_panel.ActiveAutoHideContent = null;
      this.dock_panel.Dock = System.Windows.Forms.DockStyle.Fill;
      this.dock_panel.Location = new System.Drawing.Point (0, 0);
      this.dock_panel.Name = "dock_panel";
      this.dock_panel.Size = new System.Drawing.Size (983, 659);
      this.dock_panel.TabIndex = 3;
      // 
      // standard_tool_bar
      // 
      this.standard_tool_bar.Dock = System.Windows.Forms.DockStyle.None;
      this.standard_tool_bar.Items.AddRange (new System.Windows.Forms.ToolStripItem [] {
            this.new_scn_file_button,
            this.open_file_button,
            this.save_file_button,
            this.save_all_button,
            this.standard_tool_bar_separator1});
      this.standard_tool_bar.Location = new System.Drawing.Point (3, 0);
      this.standard_tool_bar.Name = "standard_tool_bar";
      this.standard_tool_bar.Size = new System.Drawing.Size (141, 25);
      this.standard_tool_bar.TabIndex = 0;
      // 
      // new_scn_file_button
      // 
      this.new_scn_file_button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.new_scn_file_button.Image = global::builders.framework.Properties.Resources.NewDocumentIcon;
      this.new_scn_file_button.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.new_scn_file_button.Name = "new_scn_file_button";
      this.new_scn_file_button.Size = new System.Drawing.Size (23, 22);
      this.new_scn_file_button.ToolTipText = "New SCN file";
      this.new_scn_file_button.Click += new System.EventHandler (this.OnNewScnFile);
      // 
      // open_file_button
      // 
      this.open_file_button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.open_file_button.Image = global::builders.framework.Properties.Resources.OpenFileIcon;
      this.open_file_button.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.open_file_button.Name = "open_file_button";
      this.open_file_button.Size = new System.Drawing.Size (23, 22);
      this.open_file_button.Text = "toolStripButton1";
      this.open_file_button.ToolTipText = "Open";
      this.open_file_button.Click += new System.EventHandler (this.OnOpenFile);
      // 
      // save_file_button
      // 
      this.save_file_button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.save_file_button.Image = global::builders.framework.Properties.Resources.SaveFileIcon;
      this.save_file_button.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.save_file_button.Name = "save_file_button";
      this.save_file_button.Size = new System.Drawing.Size (23, 22);
      this.save_file_button.Text = "toolStripButton1";
      this.save_file_button.ToolTipText = "Save";
      this.save_file_button.Click += new System.EventHandler (this.OnSaveFile);
      // 
      // save_all_button
      // 
      this.save_all_button.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
      this.save_all_button.Image = global::builders.framework.Properties.Resources.SaveAllIcon;
      this.save_all_button.ImageTransparentColor = System.Drawing.Color.Magenta;
      this.save_all_button.Name = "save_all_button";
      this.save_all_button.Size = new System.Drawing.Size (23, 22);
      this.save_all_button.Text = "toolStripButton1";
      this.save_all_button.ToolTipText = "Save all";
      this.save_all_button.Click += new System.EventHandler (this.OnSaveAll);
      // 
      // standard_tool_bar_separator1
      // 
      this.standard_tool_bar_separator1.Name = "standard_tool_bar_separator1";
      this.standard_tool_bar_separator1.Size = new System.Drawing.Size (6, 25);
      // 
      // MainForm
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (1008, 730);
      this.Controls.Add (this.tool_strip_container);
      this.Controls.Add (this.status_bar);
      this.Controls.Add (this.main_menu);
      this.IsMdiContainer = true;
      this.MainMenuStrip = this.main_menu;
      this.Name = "MainForm";
      this.Text = "Character builder";
      this.main_menu.ResumeLayout (false);
      this.main_menu.PerformLayout ();
      this.status_bar.ResumeLayout (false);
      this.status_bar.PerformLayout ();
      this.tool_strip_container.ContentPanel.ResumeLayout (false);
      this.tool_strip_container.TopToolStripPanel.ResumeLayout (false);
      this.tool_strip_container.TopToolStripPanel.PerformLayout ();
      this.tool_strip_container.ResumeLayout (false);
      this.tool_strip_container.PerformLayout ();
      this.standard_tool_bar.ResumeLayout (false);
      this.standard_tool_bar.PerformLayout ();
      this.ResumeLayout (false);
      this.PerformLayout ();

    }

    #endregion

    private System.Windows.Forms.MenuStrip main_menu;
    private System.Windows.Forms.ToolStripMenuItem file_menu;
    private System.Windows.Forms.ToolStripMenuItem edit_menu;
    private System.Windows.Forms.ToolStripMenuItem view_menu;
    private System.Windows.Forms.ToolStripMenuItem window_menu;
    private System.Windows.Forms.ToolStripMenuItem help_menu;
    private System.Windows.Forms.ToolStripMenuItem new_scn_file_menu_item;
    private System.Windows.Forms.ToolStripMenuItem open_file_menu_item;
    private System.Windows.Forms.ToolStripMenuItem exit_menu_item;
    private System.Windows.Forms.ToolStripSeparator file_menu_separator1;
    private System.Windows.Forms.ToolStripMenuItem save_menu_item;
    private System.Windows.Forms.ToolStripMenuItem save_as_menu_item;
    private System.Windows.Forms.ToolStripSeparator file_menu_separator2;
    private System.Windows.Forms.ToolStripMenuItem undo_menu_item;
    private System.Windows.Forms.ToolStripMenuItem redo_menu_item;
    private System.Windows.Forms.ToolStripSeparator edit_menu_separator1;
    private System.Windows.Forms.ToolStripMenuItem cut_menu_item;
    private System.Windows.Forms.ToolStripMenuItem copy_menu_item;
    private System.Windows.Forms.ToolStripMenuItem paste_menu_item;
    private System.Windows.Forms.ToolStripMenuItem delete_menu_item;
    private System.Windows.Forms.ToolStripMenuItem about_menu_item;
    private System.Windows.Forms.ToolStripSeparator edit_menu_separator3;
    private System.Windows.Forms.ToolStripMenuItem select_all_menu_item;
    private System.Windows.Forms.ToolStripSeparator view_menu_separator1;
    private System.Windows.Forms.ToolStripMenuItem toolbars_menu_item;
    private System.Windows.Forms.ToolStripMenuItem full_screen_menu_item;
    private System.Windows.Forms.ToolStripMenuItem full_screen_menu_disable_menu_item;
    private System.Windows.Forms.ToolStripMenuItem properties_menu_item;
    private System.Windows.Forms.ToolStripMenuItem save_all_menu_item;
    private System.Windows.Forms.StatusStrip status_bar;
    private System.Windows.Forms.ToolStripStatusLabel status_label;
    private System.Windows.Forms.ToolStripContainer tool_strip_container;
    private WeifenLuo.WinFormsUI.Docking.DockPanel dock_panel;
    private System.Windows.Forms.ToolStrip standard_tool_bar;
    private System.Windows.Forms.ToolStripButton new_scn_file_button;
    private System.Windows.Forms.ToolStripButton open_file_button;
    private System.Windows.Forms.ToolStripButton save_file_button;
    private System.Windows.Forms.ToolStripButton save_all_button;
    private System.Windows.Forms.ToolStripSeparator standard_tool_bar_separator1;
  }
}

