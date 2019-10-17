using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using IronPython.Hosting;
using Microsoft.Scripting;
using Microsoft.Scripting.Hosting;

namespace ConsoleApp1
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.WriteLine("Hello world!");

            string script = "print \"Hello World from Python!\"";
            ScriptEngine pythonEngine = Python.CreateEngine();
            ScriptScope scope = pythonEngine.CreateScope();
            ScriptSource source = pythonEngine.CreateScriptSourceFromString(script, SourceCodeKind.Statements);
            CompiledCode compiled = source.Compile();
            dynamic result = compiled.Execute(scope);

            Console.WriteLine("Hello world!");
        }
    }
}
