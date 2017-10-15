#include "Globals.h"

#include "Application.h"
#include "ModuleRenderer3D.h"

#include "glew/include/glew.h"
#include "SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>


#pragma comment (lib, "glu32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */
#pragma comment (lib, "glew/libx86/glew32.lib")

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

// Destructor
ModuleRenderer3D::~ModuleRenderer3D()
{}

// Called before render is available
bool ModuleRenderer3D::Init()
{
	MYLOG("Creating 3D Renderer context");
	bool ret = true;
	
	//SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	//Create context
	context = SDL_GL_CreateContext(App->window->window);
	if(context == NULL)
	{
		MYLOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			MYLOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Check for error
		GLenum error = glGetError();
		if(error != GL_NO_ERROR)
		{
			MYLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			MYLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.105f, 0.105f, 0.105f, 1.f);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			MYLOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize glew and check for errors
		GLenum err = glewInit();
		if (err != 0)
		{
			MYLOG("Error initializing Glew! %s\n", glewGetErrorString(err));
			ret = false;
		}
		else
			MYLOG("Using Glew %s", glewGetString(GLEW_VERSION));
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

//DRAW QUAD
	//FillVertex();
	//DirectCube();
	//glColor4f(0.2f, 0.2f, 1.0f, 1.0f);
	//glGenBuffers(1, (GLuint*) &(myId)); //TANTU
	//glBindBuffer(GL_ARRAY_BUFFER, myId);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 3, vertices, GL_STATIC_DRAW);

//	App->fileSystem->Load("Game/Assets/warrior.fbx");
	
	
	
	//std::vector<Mesh*> tmp = importer.CreateMesh("C:/Users/Usuari/Documents/GitHub/GT-Engine/GameEngine/Game/Assets/warrior.fbx");
	//
	//for (std::vector<Mesh*>::iterator it = tmp.begin(); it != tmp.end(); it++)
	//{
	//	meshes.push_back(*it);
	//}

	return ret;
}

// PreUpdate: clear buffer
update_status ModuleRenderer3D::PreUpdate(float dt)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(App->camera->GetViewMatrix());

	// light 0 on cam pos
	lights[0].SetPos(App->camera->Position.x, App->camera->Position.y, App->camera->Position.z);

	for(uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::Update(float dt)
{
	if (App->input->has_dropped)
	{
		FileExtensions extension = importer.GetExtension(App->input->GetFileDropped());

		switch (extension)
		{
		case FileExtensions::Scene3D:
		{
			LoadMeshes((char*)App->input->GetFileDropped());
			break;
		}
		case FileExtensions::Image:
		{
			LoadImages((char*)App->input->GetFileDropped());
			break;
		}
		case FileExtensions::Unsupported:
		{
			MYLOG("File Type not supported by GT Engine");
			break;
		}
		}

		App->input->has_dropped = false;
	}

	return UPDATE_CONTINUE;
}

// PostUpdate present buffer to screen
update_status ModuleRenderer3D::PostUpdate(float dt)
{
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	if (bEnableWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*App->level->Draw();
	if (debugDraw == true)
	{
		BeginDebugDraw();
		App->DebugDraw();
		EndDebugDraw();
	}
	*/
//DRAW QUAD
	//glEnableClientState(GL_VERTEX_ARRAY);
	//glBindBuffer(GL_ARRAY_BUFFER, myId);
	//glVertexPointer(3, GL_FLOAT, 0, NULL);
	//
	//glDrawArrays(GL_TRIANGLES, 0, 36*3);
	//glDisableClientState(GL_VERTEX_ARRAY);
//---------------------------------------------------------------------------------------------------------------------------
	
//----------------------------------------------------------------------------------------------------------------------------

	for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		if (((*it)->buffTexture) > 0)
		{
			glEnableClientState(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, (*it)->buffTexture);
		}

		if (((*it)->buffVertex) > 0)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, (*it)->buffVertex);
			glVertexPointer(3, GL_FLOAT, 0, NULL);
		}

		if (((*it)->buffNormals) > 0)
		{
			glEnableClientState(GL_NORMAL_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, (*it)->buffNormals);
			glNormalPointer(GL_FLOAT, 0, NULL);
		}

		if (((*it)->buffUv) > 0)
		{
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glBindBuffer(GL_ARRAY_BUFFER, (*it)->buffUv);
			glTexCoordPointer(2, GL_FLOAT, 0, NULL);
		}

		if (((*it)->buffIndex) > 0)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*it)->buffIndex);
			glDrawElements(GL_TRIANGLES, (*it)->numIndex, GL_UNSIGNED_INT, NULL);
		}

		// CleanUp
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		
		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		glDisableClientState(GL_TEXTURE_2D);
	
	}
	
	// Drawing UI
	ImporterUI();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	App->editor->Draw();

	SDL_GL_SwapWindow(App->window->window);
	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModuleRenderer3D::CleanUp()
{
	MYLOG("Destroying 3D Renderer");
	
	CleanScene();

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::AddImGui()
{
	if (ImGui::CollapsingHeader("Renderer"))
	{
		if (ImGui::Checkbox("GL Depth Test", &bEnableDepthTest))
		{
			if (bEnableDepthTest)
				glEnable(GL_DEPTH_TEST);
			else
				glDisable(GL_DEPTH_TEST);
		}
		if (ImGui::Checkbox("GL Cull Face", &bEnableCullFace))
		{
			if (bEnableCullFace)
				glEnable(GL_CULL_FACE);
			else
				glDisable(GL_CULL_FACE);
		}
		if (ImGui::Checkbox("GL Lighting", &bEnableLighting))
		{
			if (bEnableLighting)
				glEnable(GL_LIGHTING);
			else
				glDisable(GL_LIGHTING);
		}
		if (ImGui::Checkbox("GL Color Material", &bEnableColorMaterial))
		{
			if (bEnableColorMaterial)
				glEnable(GL_COLOR_MATERIAL);
			else
				glDisable(GL_COLOR_MATERIAL);
		}
		if (ImGui::Checkbox("GL Texture 2D", &bEnableGLTexture))
		{
			if (bEnableGLTexture)
				glEnable(GL_TEXTURE_2D);
			else
				glDisable(GL_TEXTURE_2D);
		}
		if (ImGui::Checkbox("Wireframe Mode", &bEnableWireframe))
		{
			
		}
	}
}

void ModuleRenderer3D::LoadMeshes(char* path)
{
	CleanScene();

	std::vector<Mesh*> tmp = importer.CreateMesh(path);

	App->camera->referenceDone = true;
	if (!tmp.empty())
	{
		AABB aabb;
		for (std::vector<Mesh*>::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
			aabb.Enclose((*it)->aabbBox);
		}

		App->camera->Position.x = aabb.maxPoint.x * 2;
		App->camera->Position.y = aabb.maxPoint.y * 2;
		App->camera->Position.z = aabb.maxPoint.z * 2;
		App->camera->LookAt(vec3(aabb.CenterPoint().x, aabb.CenterPoint().y, aabb.CenterPoint().z));

		for (std::vector<Mesh*>::iterator it = tmp.begin(); it != tmp.end(); it++)
		{
			meshes.push_back(*it);
		}
	}
}

void ModuleRenderer3D::LoadImages(char * path)
{
	std::vector<Mesh*>::iterator it = meshes.begin();
	for(; it != meshes.end(); it++)
	{
		if ((*it)->buffTexture > 0)
		{
			glDeleteBuffers(1, &(*it)->buffTexture);
		}
		(*it)->buffTexture = importer.LoadImageFile(path);
	}
}

void ModuleRenderer3D::CleanScene()
{
	for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
	{
		if ((*it) != nullptr)
			delete (*it);
	}
	meshes.clear();
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	projectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&projectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::FillVertex()
{
	//QUAD1
	vertices[0] = 0.f;
	vertices[1] = 1.f;
	vertices[2] = 0.f;

	vertices[3] = 0.f;
	vertices[4] = 0.f;
	vertices[5] = 0.f;

	vertices[6] = 1.f;
	vertices[7] = 0.f;
	vertices[8] = 0.f;

	vertices[9] = 0.f;
	vertices[10] = 1.f;
	vertices[11] = 0.f;

	vertices[12] = 1.f;
	vertices[13] = 0.f;
	vertices[14] = 0.f;

	vertices[15] = 1.f;
	vertices[16] = 1.f;
	vertices[17] = 0.f;
	//QUAD2
	vertices[18] = 1.f;
	vertices[19] = 1.f;
	vertices[20] = 0.f;

	vertices[21] = 1.f;
	vertices[22] = 0.f;
	vertices[23] = 0.f;

	vertices[24] = 1.f;
	vertices[25] = 0.f;
	vertices[26] = 1.f;

	vertices[27] = 1.f;
	vertices[28] = 1.f;
	vertices[29] = 0.f;

	vertices[30] = 1.f;
	vertices[31] = 0.f;
	vertices[32] = 1.f;

	vertices[33] = 1.f;
	vertices[34] = 1.f;
	vertices[35] = 1.f;
	//QUAD3
	vertices[36] = 1.f;
	vertices[37] = 1.f;
	vertices[38] = 1.f;

	vertices[39] = 1.f;
	vertices[40] = 0.f;
	vertices[41] = 1.f;

	vertices[42] = 0.f;
	vertices[43] = 0.f;
	vertices[44] = 1.f;

	vertices[45] = 1.f;
	vertices[46] = 1.f;
	vertices[47] = 1.f;

	vertices[48] = 0.f;
	vertices[49] = 0.f;
	vertices[50] = 1.f;

	vertices[51] = 0.f;
	vertices[52] = 1.f;
	vertices[53] = 1.f;

	//QUAD4
	vertices[54] = 0.f;
	vertices[55] = 1.f;
	vertices[56] = 1.f;

	vertices[57] = 0.f;
	vertices[58] = 0.f;
	vertices[59] = 1.f;

	vertices[60] = 0.f;
	vertices[61] = 0.f;
	vertices[62] = 0.f;

	vertices[63] = 0.f;
	vertices[64] = 1.f;
	vertices[65] = 1.f;

	vertices[66] = 0.f;
	vertices[67] = 0.f;
	vertices[68] = 0.f;

	vertices[69] = 0.f;
	vertices[70] = 1.f;
	vertices[71] = 0.f;

	//QUAD5
	vertices[72] = 0.f;
	vertices[73] = 1.f;
	vertices[74] = 0.f;

	vertices[75] = 1.f;
	vertices[76] = 1.f;
	vertices[77] = 0.f;

	vertices[78] = 1.f;
	vertices[79] = 1.f;
	vertices[80] = 1.f;

	vertices[81] = 0.f;
	vertices[82] = 1.f;
	vertices[83] = 0.f;

	vertices[84] = 1.f;
	vertices[85] = 1.f;
	vertices[86] = 1.f;

	vertices[87] = 0.f;
	vertices[88] = 1.f;
	vertices[89] = 1.f;

	//QUAD6
	vertices[90] = 0.f;
	vertices[91] = 0.f;
	vertices[92] = 0.f;

	vertices[93] = 0.f;
	vertices[94] = 0.f;
	vertices[95] = 1.f;

	vertices[96] = 1.f;
	vertices[97] = 0.f;
	vertices[98] = 1.f;

	vertices[99] = 0.f;
	vertices[100] = 0.f;
	vertices[101] = 0.f;

	vertices[102] = 1.f;
	vertices[103] = 0.f;
	vertices[104] = 1.f;

	vertices[105] = 1.f;
	vertices[106] = 0.f;
	vertices[107] = 0.f;
}

void ModuleRenderer3D::DirectCube()
{
	glColor4f(1.0f, 0.2f, 0.8f, 1.0f);

	glBegin(GL_TRIANGLES);

	glVertex3f(2.f, 0.f, 0.f);
	glVertex3f(2.f, 1.f, 0.f);
	glVertex3f(3.f, 0.f, 0.f);

	glVertex3f(2.f, 1.f, 0.f);
	glVertex3f(3.f, 1.f, 0.f);
	glVertex3f(3.f, 0.f, 0.f);

	glVertex3f(2.f, 0.f, 1.f);
	glVertex3f(3.f, 0.f, 1.f);
	glVertex3f(2.f, 1.f, 1.f);

	glVertex3f(3.f, 0.f, 1.f);
	glVertex3f(3.f, 1.f, 1.f);
	glVertex3f(2.f, 1.f, 1.f);

	glVertex3f(3.f, 1.f, 0.f);
	glVertex3f(3.f, 1.f, 1.f);
	glVertex3f(3.f, 0.f, 1.f);

	glVertex3f(3.f, 0.f, 1.f);
	glVertex3f(3.f, 0.f, 0.f);
	glVertex3f(3.f, 1.f, 0.f);

	glVertex3f(2.f, 0.f, 0.f);
	glVertex3f(2.f, 0.f, 1.f);
	glVertex3f(2.f, 1.f, 0.f);

	glVertex3f(2.f, 1.f, 1.f);
	glVertex3f(2.f, 1.f, 0.f);
	glVertex3f(2.f, 0.f, 1.f);

	glVertex3f(2.f, 1.f, 1.f);
	glVertex3f(3.f, 1.f, 1.f);
	glVertex3f(3.f, 1.f, 0.f);

	glVertex3f(2.f, 1.f, 1.f);
	glVertex3f(3.f, 1.f, 0.f);
	glVertex3f(2.f, 1.f, 0.f);

	glVertex3f(2.f, 0.f, 1.f);
	glVertex3f(3.f, 0.f, 0.f);
	glVertex3f(3.f, 0.f, 1.f);

	glVertex3f(2.f, 0.f, 1.f);
	glVertex3f(2.f, 0.f, 0.f);
	glVertex3f(3.f, 0.f, 0.f);

	glEnd();
}

void ModuleRenderer3D::ImporterUI()
{
	ImGuiWindowFlags flag = ImGuiWindowFlags_NoFocusOnAppearing;
	ImGui::Begin("Properties", 0, flag);
	{
		if (ImGui::CollapsingHeader("Transformation"))
		{
			int a = 1;
			for (std::vector<Mesh*>::iterator it = meshes.begin(); it != meshes.end(); it++)
			{
				ImGui::Text("Mesh %i", a);
				ImGui::Text("Number of Vertex: %i", (*it)->vertex.size());
				a++;
			}
		}
		if (ImGui::CollapsingHeader("Geometry"))
		{
		}
		if (ImGui::CollapsingHeader("Texture"))
		{
		}
	}

	ImGui::End();
}
