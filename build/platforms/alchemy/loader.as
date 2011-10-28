package
{
  import flash.display.Sprite;
  import cmodule.axis_aligned_box.CLibInit;

  public class Test extends Sprite
  {
    public function Test()
    {
      var loader:CLibInit = new CLibInit;
      var lib:Object = loader.init();
    }
  }
}