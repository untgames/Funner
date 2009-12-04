namespace builders.framework
{
  partial class PropertyGrid
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

    #region Component Designer generated code

    /// <summary> 
    /// Required method for Designer support - do not modify 
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent ()
    {
      this.property_grid = new System.Windows.Forms.PropertyGrid ();
      this.SuspendLayout ();
      // 
      // property_grid
      // 
      this.property_grid.Dock = System.Windows.Forms.DockStyle.Fill;
      this.property_grid.Location = new System.Drawing.Point (0, 0);
      this.property_grid.Name = "property_grid";
      this.property_grid.Size = new System.Drawing.Size (284, 262);
      this.property_grid.TabIndex = 0;
      // 
      // PropertyGrid
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF (6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size (284, 262);
      this.Controls.Add (this.property_grid);
      this.Name = "PropertyGrid";
      this.TabText = "Properties";
      this.Text = "Properties";
      this.ResumeLayout (false);

    }

    #endregion

    private System.Windows.Forms.PropertyGrid property_grid;
  }
}
