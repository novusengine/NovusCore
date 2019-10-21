using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Diagnostics;
using IronPython.Hosting;
using Microsoft.Scripting;
using Microsoft.Scripting.Hosting;
using System.Reflection;

namespace Converter
{
    class Program
    {
        static void Main(string[] args)
        {
            Console.ForegroundColor = ConsoleColor.White;

            if (args.Length != 2)
            {
                Console.WriteLine("[ERROR] Converter expected 2 argument while " + args.Length + " was supplied!");
                return;
            }

            string dataDirectory = args[0];
            string outputDirectory = args[1];

            Stopwatch totalTime = Stopwatch.StartNew();

            // Create all converters
            IEnumerable<Converters.BaseConverter> converters = ReflectiveEnumerator.GetEnumerableOfType<Converters.BaseConverter>();
            foreach (Converters.BaseConverter converter in converters)
            {
                converter.Init();
            }

            ScriptEngine pythonEngine = Python.CreateEngine();

            // First we search of dataDirectory for all .py files, these files are scripts that will handle conversion
            string[] pythonFiles = Directory.GetFiles(dataDirectory, "*.py", SearchOption.AllDirectories);
            foreach(string pythonFile in pythonFiles)
            {
                string output = pythonFile;

                Stopwatch scriptTime = Stopwatch.StartNew();

                ScriptScope scope = pythonEngine.CreateScope();
                ScriptSource source = pythonEngine.CreateScriptSourceFromFile(pythonFile);
                CompiledCode compiled = source.Compile();
                dynamic result = compiled.Execute(scope);

                List<string> errors = new List<string>();

                bool availableConverter = false;
                bool wasConverted = false;
                foreach(Converters.BaseConverter converter in converters)
                {
                    string error;
                    if (converter.CanConvert(pythonFile, scope, out error))
                    {
                        errors.Clear();
                        availableConverter = true;

                        
                        if (converter.Convert(pythonFile, scope, outputDirectory, out error))
                        {
                            wasConverted = true;
                        }
                        else if (error != "")
                        {
                            errors.Add(error + " (" + converter.GetType().GetTypeInfo().Name + ")");
                        }
                    }
                    else if (error != "")
                    {
                        errors.Add(error + " (" + converter.GetType().GetTypeInfo().Name + ")");
                    }
                }

                if (!availableConverter)
                {
                    output += " Could not find a suitable converter";
                }
                else if (!wasConverted)
                {
                    output += " Could not successfully be converted";
                }
                else
                {
                    output += " Converted successfully";
                }

                output += " (" + scriptTime.Elapsed.ToString() + ")";

                if (availableConverter && wasConverted)
                {
                    Console.ForegroundColor = ConsoleColor.Green;
                    Console.Write("[SUCCESS]: ");
                }
                else
                {
                    Console.ForegroundColor = ConsoleColor.Red;
                    Console.Write("[ERROR]: ");
                }
                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine(output);

                if (!availableConverter || !wasConverted)
                {
                    foreach (string error in errors)
                    {
                        Console.WriteLine("     [Reason]: " + error);
                    }
                }
            }

            Console.WriteLine("Conversion done in " + totalTime.Elapsed.ToString());
        }
    }
}
