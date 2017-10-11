#include "Importer.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "MathGeoLib\MathGeoLib.h"
#include "Devil/include/il.h"
#include "Devil/include/ilu.h"
#include "Devil/include/ilut.h"

#pragma comment (lib, "Devil/libx86/DevIL.lib")
#pragma	comment (lib, "Devil/libx86/ILU.lib")
#pragma	comment (lib, "Devil/libx86/ILUT.lib")

std::vector<Mesh*> Importer::CreateMesh(char * path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	std::vector<Mesh*> ret;

	if (scene != nullptr && scene->HasMeshes())
	{

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (uint i = 0; i < scene->mNumMeshes ; i++) 
		{
			Mesh* mesh = new Mesh;

			mesh->vertex.reserve(scene->mMeshes[i]->mNumVertices);
			memcpy(mesh->vertex.data(), scene->mMeshes[i]->mVertices, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
			
			if (mesh->vertex.empty() != false)
			{
				glGenBuffers(1, (GLuint*) &(mesh->buff_vertex));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->buff_vertex);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, mesh->vertex.data(), GL_STATIC_DRAW);

				MYLOG("Importer - Loading %i vertex succesful!", scene->mMeshes[i]->mNumVertices);
			}
			else
			{
				MYLOG("WARNING, the scene has 0 vertex!");
			}

			if (scene->mMeshes[i]->HasNormals())
			{
				mesh->normals.reserve(scene->mMeshes[i]->mNumVertices);
				memcpy(mesh->normals.data(), scene->mMeshes[i]->mNormals, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
				glGenBuffers(1, (GLuint*) &(mesh->buff_normals));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->buff_normals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, mesh->normals.data(), GL_STATIC_DRAW);
			}
			
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				float2* uv = new float2[scene->mMeshes[i]->mNumVertices]; //BE
				if (uv > &float2(scene->mMeshes[i]->mNumVertices))
				{
					MYLOG("WARNING, don't have UV");
				}
				else
				{
					memcpy(uv, scene->mMeshes[i]->mTextureCoords, sizeof(float2)*scene->mMeshes[i]->mNumVertices);
				}
				

				glGenBuffers(1, (GLuint*) &(mesh->buff_uv));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->buff_uv);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * scene->mMeshes[i]->mNumVertices, uv, GL_STATIC_DRAW);
			}
			
			
			uint* index = new uint[scene->mMeshes[i]->mNumFaces*3];
			for (uint j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
			{
				if (scene->mMeshes[i]->mFaces[j].mNumIndices != 3)
				{
					MYLOG("WARNING, geometry face with != 3 indices!");
				}	
				else
				{
					memcpy(&index[j*3], scene->mMeshes[i]->mFaces[j].mIndices, sizeof(uint)*3);
					mesh->num_index++;
				}
			}

			glGenBuffers(1, (GLuint*) &(mesh->buff_index));
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->buff_index);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * scene->mMeshes[i]->mNumFaces*3, index, GL_STATIC_DRAW);
			MYLOG("Importer - Loading %i index succesful!", (uint)scene->mMeshes[i]->mNumFaces * 3);

			mesh->buff_texture;
			aiString texturePath; // path de la textura, desde donde esta el fbx
			std::string strPath = path;
			scene->mMaterials[scene->mMeshes[i]->mMaterialIndex]->GetTexture(aiTextureType::aiTextureType_DIFFUSE, 0, &texturePath);
			
			bool found = false;
			for (uint i = strlen(path); i >= 0 && !found; i--)
			{
				if (path[i] == '\\')
				{
					for (uint y = 0; !found; y++)
					{
						if (strPath[i + 1] == '\0')
						{
							strPath.resize(strPath.size() + 1);
						}

						i++;
						strPath[i] = texturePath.C_Str()[y];
						if (texturePath.C_Str()[y + 1] == '\0')
						{
							found = true;
						}
					}
				}
			}
			//char* cstr = NULL;
			//strcpy_s(cstr, strPath.length() + 1, strPath.c_str());

			//mesh->buff_texture = ilutGLLoadImage(cstr);


			mesh->aabbBox.SetNegativeInfinity();
			mesh->aabbBox.Enclose(mesh->vertex.data(), scene->mMeshes[i]->mNumVertices);
			
			ret.push_back(mesh);
			
		}
		aiReleaseImport(scene);
	}
	else
		MYLOG("Error loading scene %s", path);

	
	return ret;
}
