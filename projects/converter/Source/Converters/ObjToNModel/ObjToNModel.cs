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
                objLoader = objLoaderFactory.Create(materialStreamProvider);
                LoadResult result = objLoader.Load(assetStream);

                NovusModel model = new NovusModel();
                model.indexType = NovusModel.VkPrimitiveTopology.TRIANGLE_LIST; // We only support trianglelist

                // Get all vertex positions
                List<Vec3> vertexPositions = new List<Vec3>();
                foreach (Vertex vertex in result.Vertices)
                {
                    Vec3 pos = new Vec3(vertex.X, vertex.Y, vertex.Z);
                    vertexPositions.Add(pos);
                }

                // Get all vertex texCoords
                List<Vec2> vertexTexCoords = new List<Vec2>();
                foreach (Texture texture in result.Textures)
                {
                    Vec2 texCoord = new Vec2(texture.X, 1-texture.Y);
                    vertexTexCoords.Add(texCoord);
                }

                // Get all vertex normals
                List<Vec3> vertexNormals = new List<Vec3>();
                foreach (Normal normal in result.Normals)
                {
                    Vec3 vertexNormal = new Vec3(normal.X, normal.Y, normal.Z);
                    vertexNormals.Add(vertexNormal);
                }

                // Because .obj stores vertex positions, vertex texcoords and vertex normals separately and without duplication we need to "unpack" combined vertices from this data
                // Each .obj model has a list of "groups" which represent submeshes
                // Each group has a list of faces which represent quads, they have 4 "indices" that point to vertex positions, vertex texcoords and vertex normals separately
                // We need to iterate over these, and then build one combined vertex for each unique combination of position, texcoord and normal

                // Convert indices

                // This dictionary will hold unique vertices as keys, and it's corresponding index as value, this makes it easy for us to look up indices of duplicated vertices
                Dictionary<NovusModel.Vertex, Int16> combinedVertices = new Dictionary<NovusModel.Vertex, Int16>();

                foreach (ObjLoader.Loader.Data.Elements.Group group in result.Groups) // A group represents a "submesh"
                {
                    foreach (Face face in group.Faces)
                    {
                        Debug.Assert(face.Count == 3 || face.Count == 4); // I haven't seen a model where faces don't have 3 or 4 indices yet

                        Int16[] combinedIndices = new Int16[face.Count];
                        for (int i = 0; i < face.Count; i++)
                        {
                            Vec3 position = vertexPositions[face[i].VertexIndex - 1];
                            Vec3 normal = vertexNormals[face[i].NormalIndex - 1];
                            Vec2 texCoord = vertexTexCoords[face[i].TextureIndex - 1];
                            combinedIndices[i] = CombineVertex(position, normal, texCoord, ref combinedVertices);
                        }

                        if (face.Count == 4)
                        {
                            // We split the face (quad) into two triangles, the first one with index 0 1 and 2
                            model.indices.Add(combinedIndices[2]);
                            model.indices.Add(combinedIndices[1]);
                            model.indices.Add(combinedIndices[0]);

                            // The second one with index 2 3 and 0
                            model.indices.Add(combinedIndices[0]);
                            model.indices.Add(combinedIndices[3]);
                            model.indices.Add(combinedIndices[2]);
                        }
                        else if (face.Count == 3)
                        {
                            // The face is a triangle so just add it like it is
                            model.indices.Add(combinedIndices[2]);
                            model.indices.Add(combinedIndices[1]);
                            model.indices.Add(combinedIndices[0]);
                        }
                        
                    }
                }

                model.vertices.AddRange(combinedVertices.Keys);

                // Create output file
                string outputFileName = Path.ChangeExtension(Path.GetFileName(assetPath), ".novusmodel");
                using (FileStream outStream = new FileStream(Path.Combine(outputDirectory, outputFileName), FileMode.Create))
                {
                    model.Serialize(new BinaryWriter(outStream));
                }
            }
                
            return true;
        }

        Int16 CombineVertex(Vec3 position, Vec3 normal, Vec2 texCoord, ref Dictionary<NovusModel.Vertex, Int16> combinedVertices)
        {
            NovusModel.Vertex combinedVertex = new NovusModel.Vertex();
            combinedVertex.position = position;
            combinedVertex.normal = normal;
            combinedVertex.texCoord = texCoord;

            // If our dict of combined vertices contains a vertex identical to this already
            if (combinedVertices.ContainsKey(combinedVertex))
            {
                // Just return its index
                return combinedVertices[combinedVertex];
            }

            // Otherwise, we add the vertex to the dict and return its index
            Int16 index = (Int16)combinedVertices.Count;
            combinedVertices.Add(combinedVertex, index);
            return index;
        }

        ObjLoaderFactory objLoaderFactory;
        CustomMaterialStreamProvider materialStreamProvider;
        IObjLoader objLoader;
    }

    class NovusModel : NovusType
    {
        protected override int GetNovusType() { return 42; } // Never change this one

        protected override int GetVersion() { return 2; } // Increment this if you ever change how NovusModel is serialized

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

        public struct Vertex
        {
            public Vec3 position;
            public Vec3 normal;
            public Vec2 texCoord;
        }

        public List<Vertex> vertices = new List<Vertex>();
        public VkPrimitiveTopology indexType;
        public List<Int16> indices = new List<Int16>();

        public void Serialize(BinaryWriter binaryWriter)
        {
            // Remember to always serialize the header first!
            SerializeNovusHeader(binaryWriter);

            // Write vertices
            binaryWriter.Write(vertices.Count);
            foreach(Vertex vertex in vertices)
            {
                binaryWriter.Write(vertex.position.X);
                binaryWriter.Write(vertex.position.Y);
                binaryWriter.Write(vertex.position.Z);
                binaryWriter.Write(vertex.normal.X);
                binaryWriter.Write(vertex.normal.Y);
                binaryWriter.Write(vertex.normal.Z);
                binaryWriter.Write(vertex.texCoord.X);
                binaryWriter.Write(vertex.texCoord.Y);
            }

            // Write indices
            binaryWriter.Write((int)indexType);
            binaryWriter.Write((uint)indices.Count);
            foreach (Int16 index in indices)
            {
                binaryWriter.Write(index);
            }
        }

    }
}