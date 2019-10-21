using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using Microsoft.Scripting.Hosting;
using ObjLoader.Loader.Data;
using ObjLoader.Loader.Data.Elements;
using ObjLoader.Loader.Data.VertexData;
using ObjLoader.Loader.Loaders;

namespace Converter.Converters
{
    public class CustomMaterialStreamProvider : IMaterialStreamProvider
    {
        public void SetAssetPath(string assetPath)
        {
            this.assetDir = Path.GetFullPath(Path.Combine(assetPath, @"../")); ;
        }

        public Stream Open(string materialName)
        {
            string materialFilePath = Path.Combine(assetDir, materialName);

            return File.Open(materialFilePath, FileMode.Open, FileAccess.Read);
        }

        string assetDir;
    }

    class ObjToNModelConverter : BaseConverter
    {
        public override void Init()
        {
            objLoaderFactory = new ObjLoaderFactory();
            materialStreamProvider = new CustomMaterialStreamProvider();
            objLoader = objLoaderFactory.Create(materialStreamProvider);
        }

        public override bool CanConvert(string pythonPath, ScriptScope scope, out string error)
        {
            error = "";
            string assetPath = GetAssetPath(pythonPath);

            if (!assetPath.EndsWith(".obj"))
            {
                error = "We only convert .obj files";
                return false;
            }

            return File.Exists(assetPath);
        }

        public override bool Convert(string pythonPath, ScriptScope scope, string outputDirectory, out string error)
        {
            error = "";
            string assetPath = GetAssetPath(pythonPath);
            materialStreamProvider.SetAssetPath(assetPath);

            using (FileStream assetStream = new FileStream(assetPath, FileMode.Open))
            {
                LoadResult result = objLoader.Load(assetStream);

                NovusModel model = new NovusModel();

                // Convert vertices
                foreach (Vertex vertex in result.Vertices)
                {
                    model.vertexPositions.Add(new Vec3(vertex.X, vertex.Y, vertex.Z));
                }

                // Convert indices
                model.indexType = NovusModel.VkPrimitiveTopology.TRIANGLE_LIST; // We only support trianglelist so far
                foreach (Group group in result.Groups) // A group represents a "submesh"
                {
                    foreach (Face face in group.Faces)
                    {
                        Debug.Assert(face.Count == 4); // I haven't seen a model where faces don't have 4 indices yet

                        // We split the face (quad) into two triangles, the first one with index 0 1 and 2
                        model.indices.Add((Int16)(face[0].VertexIndex-1));
                        model.indices.Add((Int16)(face[1].VertexIndex-1));
                        model.indices.Add((Int16)(face[2].VertexIndex-1));

                        // The second one with index 0 2 and 3
                        model.indices.Add((Int16)(face[2].VertexIndex-1));
                        model.indices.Add((Int16)(face[3].VertexIndex-1));
                        model.indices.Add((Int16)(face[0].VertexIndex-1));
                        //break;
                    }
                }

                // Create output file
                string outputFileName = Path.ChangeExtension(Path.GetFileName(assetPath), ".novusmodel");
                using (FileStream outStream = new FileStream(Path.Combine(outputDirectory, outputFileName), FileMode.Create))
                {
                    model.Serialize(new BinaryWriter(outStream));
                }
            }
                
            return true;
        }

        ObjLoaderFactory objLoaderFactory;
        CustomMaterialStreamProvider materialStreamProvider;
        IObjLoader objLoader;
    }

    class NovusModel : NovusType
    {
        protected override int GetNovusType() { return 42; } // Never change this one

        protected override int GetVersion() { return 1; } // Increment this if you ever change how NovusModel is serialized

        public enum VkPrimitiveTopology
        {
            POINT_LIST = 0,
            LINE_LIST = 1,
            LINE_STRIP = 2,
            TRIANGLE_LIST = 3,
            TRIANGLE_STRIP = 4,
            TRIANGLE_FAN = 5,
            LINE_LIST_WITH_ADJACENCY = 6,
            LINE_STRIP_WITH_ADJACENCY = 7,
            TRIANGLE_LIST_WITH_ADJACENCY = 8,
            TRIANGLE_STRIP_WITH_ADJACENCY = 9,
            PATCH_LIST = 10,
        }

        public List<Vec3> vertexPositions = new List<Vec3>();
        public VkPrimitiveTopology indexType;
        public List<Int16> indices = new List<Int16>();

        public void Serialize(BinaryWriter binaryWriter)
        {
            // Remember to always serialize the header first!
            SerializeNovusHeader(binaryWriter);

            // Write vertices
            binaryWriter.Write(vertexPositions.Count);
            foreach(Vec3 vertex in vertexPositions)
            {
                binaryWriter.Write(vertex.X);
                binaryWriter.Write(vertex.Y);
                binaryWriter.Write(vertex.Z);
            }

            // Write indices
            binaryWriter.Write((int)indexType);
            binaryWriter.Write(indices.Count);
            foreach (Int16 index in indices)
            {
                binaryWriter.Write(index);
            }
        }

    }
}