namespace tools.ui.windows_forms
{
  public class Plugin
  {
    public static void InitPlugin (IApplicationServer application_server)
    {
      application_server.AddPlugin ("Ribbon", new RibbonBuilderPlugin(application_server));
    }
  }
}