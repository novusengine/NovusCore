using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Runtime.InteropServices;
using Microsoft.Scripting.Hosting;

namespace Converter.Converters
{
    class JpgToNTextureConverter : BaseConverter
    {
        public override void Init()
        {

        }

        public override bool CanConvert(string pythonPath, ScriptScope scope, out string error)
        {
            error = "";
            string assetPath = GetAssetPath(pythonPath);

            if (!assetPath.EndsWith(".jpg") && !assetPath.EndsWith(".jpeg"))
            {
                error = "We only convert .jpg or .jpeg files";
                return false;
            }

            if (!scope.TryGetVariable("channelCount", out _))
            {
                error = "Python script needs to specify channelCount";
                return false;
            }

            if (!scope.TryGetVariable("bitsPerChannel", out _))
            {
                error = "Python script needs to specify bitsPerChannel";
                return false;
            }

            return File.Exists(assetPath);
        }

        public override bool Convert(string pythonPath, ScriptScope scope, string outputDirectory, out string error)
        {
            error = "";
            string assetPath = GetAssetPath(pythonPath);

            using (FileStream assetStream = new FileStream(assetPath, FileMode.Open))
            {
                using (Bitmap bmp = new Bitmap(Image.FromStream(assetStream)))
                {
                    int bitsPerChannel = scope.GetVariable<int>("bitsPerChannel");

                    NovusTexture texture = new NovusTexture();
                    texture.bitsPerChannel = bitsPerChannel;
                    texture.width = bmp.Width;
                    texture.height = bmp.Height;
                    texture.channelCount = scope.GetVariable<int>("channelCount");

                    // Lock the bitmap's bits.  
                    Rectangle rect = new Rectangle(0, 0, bmp.Width, bmp.Height);
                    BitmapData bmpData = bmp.LockBits(rect, ImageLockMode.ReadWrite, bmp.PixelFormat);

                    // Get the address of the first line.
                    IntPtr ptr = bmpData.Scan0;

                    // Declare an array to hold the bytes of the bitmap.
                    int bytes = bmpData.Stride * bmp.Height * (bitsPerChannel/8);
                    byte[] byteValues = new byte[bytes];

                    // Copy the color values into the array.
                    Marshal.Copy(ptr, byteValues, 0, bytes);

                    int stride = bmpData.Stride;

                    // Loop over the pixels
                    for (int column = 0; column < bmpData.Height; column++)
                    {
                        for (int row = 0; row < bmpData.Width; row++)
                        {
                            Color color = new Color();
                            for(int i = 0; i < texture.channelCount; i++)
                            {
                                color.data[i] = byteValues[(column * stride) + (row * (bitsPerChannel / 8)) + i];
                            }
                            texture.colorData.Add(color);
                        }
                    }

                    bmp.UnlockBits(bmpData);

                    // Create output file
                    string outputFileName = Path.ChangeExtension(Path.GetFileName(assetPath), ".novustexture");
                    using (FileStream outStream = new FileStream(Path.Combine(outputDirectory, outputFileName), FileMode.Create))
                    {
                        texture.Serialize(new BinaryWriter(outStream));
                    }
                }
            }
                
            return true;
        }
    }

    class Color
    {
        public int[] data = new int[4];

        public void Serialize(BinaryWriter binaryWriter, int channelCount, int bitsPerChannel)
        {
            if (bitsPerChannel == 8)
            {
                for(int i = 0; i < channelCount; i++)
                {
                    binaryWriter.Write((byte)data[i]);
                }
            }
            else if (bitsPerChannel == 16)
            {
                for (int i = 0; i < channelCount; i++)
                {
                    binaryWriter.Write((UInt16)data[i]);
                }
            }
            else if (bitsPerChannel == 32)
            {
                for (int i = 0; i < channelCount; i++)
                {
                    binaryWriter.Write((UInt32)data[i]);
                }
            }
        }
    }

    class NovusTexture : NovusType
    {
        protected override int GetNovusType() { return 43; } // Never change this one

        protected override int GetVersion() { return 1; } // Increment this if you ever change how NovusModel is serialized

        public int width;
        public int height;
        public int bitsPerChannel;
        public int channelCount;
        public List<Color> colorData = new List<Color>();
       
        public void Serialize(BinaryWriter binaryWriter)
        {
            // Remember to always serialize the header first!
            SerializeNovusHeader(binaryWriter);

            // Write width
            binaryWriter.Write(width);

            // Write height
            binaryWriter.Write(height);

            // Write bits per channel
            binaryWriter.Write(bitsPerChannel);

            // Write channelCount
            binaryWriter.Write(channelCount);

            // Write colorDataCount
            binaryWriter.Write(colorData.Count);

            // Write the actual color data
            foreach (Color color in colorData)
            {
                color.Serialize(binaryWriter, channelCount, bitsPerChannel);
            }
        }

    }
}