using System;
using System.Collections.Generic;
using System.IO;
using System.Xml;
using System.Reflection;
using System.CodeDom.Compiler;

namespace tools.ui.windows_forms
{
  // Proxy class which allows many delegates on the same code base
  public class EventCommandProxy
  {
    protected string command;
    protected IApplicationServer server;
    protected EventCommandProxy(IApplicationServer server, string command) 
    {
      this.command = command;
      this.server = server;
    }
    protected void Handler(object sender, EventArgs e)
    {
      if (!string.IsNullOrEmpty(command))
        server.ExecuteCommand(command);
    }
    public static EventHandler Create(IApplicationServer server, string cmd)
    {
      return new EventHandler(new EventCommandProxy(server, cmd).Handler);
    }
  }
  
  public static class CodeBuilder
  {
    /// <summary>
    /// Provides code generation procedure with any type of source
    /// </summary>
    /// <typeparam name="SrcType">Source type</typeparam>
    /// <param name="source">Source object</param>
    /// <param name="writer">Output code</param>
    public delegate void CodeGenerator<SrcType>(SrcType source, StringWriter writer);
    /// <summary>
    /// Builds assembly from XML tree
    /// </summary>
    /// <param name="modules">List of required assembly names</param>
    /// <param name="generator">Code generation procedure</param>
    /// <param name="source">Starting XML node</param>
    /// <returns>Compiled assembly object</returns>
    public static Assembly Build(IEnumerable<string> modules, CodeGenerator<XmlNode> generator, XmlNode source)
    {
      // generate code
      StringWriter code = new StringWriter();
      generator(source, code);
      // and then compile it
      return Build(modules, code.ToString());
    }
    /// <summary>
    /// Builds raw source code in strings into assembly
    /// </summary>
    /// <param name="modules">Referenced assemblies names</param>
    /// <param name="sources">Source code</param>
    /// <returns>Assembly object or null on failure</returns>
    public static Assembly Build( IEnumerable<string> modules, params string[] sources)
    {
      // init compiler
      CompilerParameters compiler_parameters = new CompilerParameters();
      compiler_parameters.GenerateExecutable = false;
      compiler_parameters.GenerateInMemory = true;
      compiler_parameters.WarningLevel = 3;
      compiler_parameters.CompilerOptions = "/target:library /optimize /nologo";
      // add assembly references
      compiler_parameters.ReferencedAssemblies.Add("System.dll");
      compiler_parameters.ReferencedAssemblies.Add("System.Drawing.dll");
      foreach (string modname in modules)
        compiler_parameters.ReferencedAssemblies.Add(modname);
      // compile all this stuff
      Microsoft.CSharp.CSharpCodeProvider cs_code_provider = new Microsoft.CSharp.CSharpCodeProvider();
      CompilerResults results = cs_code_provider.CompileAssemblyFromSource(
        compiler_parameters, sources);
      // print building output to console
      foreach (string outstring in results.Output)
        System.Console.WriteLine(outstring);
      return results.CompiledAssembly;
    }
    /// <summary>
    /// Instantiates specified object from assembly; this is just a wrapper
    /// </summary>
    /// <param name="assembly">Assembly which contains specified object</param>
    /// <param name="objstring">Fully qualified name of object</param>
    /// <param name="args">Constructor arguments</param>
    /// <returns>Object instance on success or null on failure</returns>
    public static object Instantiate(Assembly assembly, string objstring, params object[] args)
    {
      return assembly.CreateInstance(objstring, false, BindingFlags.Default, null, args, null, null);
    }
  }  
}
