#include "Importer.h"
#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include "MathGeoLib\MathGeoLib.h"

std::vector<Mesh*> Importer::CreateMesh(char * path)
{
	const aiScene* scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
	std::vector<Mesh*> ret;

	if (scene != nullptr && scene->HasMeshes())
	{

		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		for (int i=0; i < scene->mNumMeshes ;i++) 
		{
			Mesh* mesh = new Mesh;

			float3* vertices = new float3[scene->mMeshes[i]->mNumVertices];
			memcpy(vertices, scene->mMeshes[i]->mVertices, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
			
			glGenBuffers(1, (GLuint*) &(mesh->buff_vertex));
			glBindBuffer(GL_ARRAY_BUFFER, mesh->buff_vertex);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, vertices, GL_STATIC_DRAW);

			if (scene->mMeshes[i]->HasNormals())
			{
				float3* normals = new float3[scene->mMeshes[i]->mNumVertices];
				memcpy(normals, scene->mMeshes[i]->mNormals, sizeof(float3)*scene->mMeshes[i]->mNumVertices);
				glGenBuffers(1, (GLuint*) &(mesh->buff_normals));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->buff_normals);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float3) * scene->mMeshes[i]->mNumVertices, normals, GL_STATIC_DRAW);
			}
			
			if (scene->mMeshes[i]->HasTextureCoords(0))
			{
				float2* uv = new float2[scene->mMeshes[i]->mNumVertices]; //BE
				memcpy(uv, scene->mMeshes[i]->mTextureCoords, sizeof(float2)*scene->mMeshes[i]->mNumVertices);

				glGenBuffers(1, (GLuint*) &(mesh->buff_uv));
				glBindBuffer(GL_ARRAY_BUFFER, mesh->buff_uv);
				glBufferData(GL_ARRAY_BUFFER, sizeof(float2) * scene->mMeshes[i]->mNumVertices, uv, GL_STATIC_DRAW);
			}
			
			
			uint* index = new uint[scene->mMeshes[i]->mNumFaces*3];
			for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++)
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

			ret.push_back(mesh);
		}
		aiReleaseImport(scene);
	}
	else
		MYLOG("Error loading scene %s", path);

	
	return ret;
}
