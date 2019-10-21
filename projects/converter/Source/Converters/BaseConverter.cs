using System;
using Microsoft.Scripting.Hosting;

namespace Converter.Converters
{
    abstract class BaseConverter
    {
        abstract public void Init();
        abstract public bool CanConvert(string pythonPath, ScriptScope scope, out string error);
        abstract public bool Convert(string pythonPath, ScriptScope scope, string outputDirectory, out string error);

        protected string GetAssetPath(string pythonPath)
        {
            int pyExtIndex = pythonPath.LastIndexOf(".py");
            return pythonPath.Substring(0, pyExtIndex);
        }
    }
}