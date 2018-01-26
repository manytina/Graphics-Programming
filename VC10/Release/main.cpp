#include <glew.h>
#include <glut.h>
#include <freeglut.h>
#define GLM_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cstdio>
#include <cstdlib>
#include <tiny_obj_loader.h>
#include "fbxloader.h"
#include <IL/il.h>
#include <iostream>

#define WALK 0
#define FURY 1
#define DEAD 2
#define BAR -1
#define RED_BLUE 0
#define ABSTRACTION 1
#define SINWAVE 2
#define PIXELIZATION 3
#define WATERCOLOR 4
#define MAGNIFIER 5
#define RIPPLE 6

GLubyte timer_cnt = 0;
bool timer_enabled = true;
unsigned int timer_speed = 16;

using namespace glm;
using namespace std; 

mat4 mvp;
GLint um4mvp, tex_obj, img_size, img_size_2, shader_now, shader_now_2, offset, line_Pos, center, iGlobalTime;

GLuint textures[50], FBX_textures[3][50];
GLuint vao;
GLuint buffer_position[500], buffer_normals[500], buffer_texcoords[500], buffer_indices[500];

int shapeCount;
int **material_ids, *vertexCount, *triangleCount;

float proj_x = 0, proj_y = 0, proj_z = 0;

int animateMode = WALK;
float frame = 0.5;

fbx_handles myFbx[3]; // Save this Object, You Will Need It to Retrieve Animations Later.
std::vector<tinyobj::shape_t> shapes, FBX_shapes[3]; 
std::vector<tinyobj::material_t> materials, FBX_materials[3];
std::string err, FBX_errs[3];

typedef struct struct_FBX
{
	int FBX_shapeCount;
	int **FBX_material_ids, *FBX_vertexCount, *FBX_triangleCount;
	GLuint FBX_buffer_position[500], FBX_buffer_normals[500], FBX_buffer_texcoords[500], FBX_buffer_indices[500];
}FBX;

FBX FBX_obj[3];

static const GLfloat window_positions[] =
{
	1.0f,-1.0f,1.0f,0.0f,
	-1.0f,-1.0f,0.0f,0.0f,
	-1.0f,1.0f,0.0f,1.0f,
	1.0f,1.0f,1.0f,1.0f
};

GLuint program, program_2, program_3, program_4, programs[15];
GLuint window_vao, bar_vao;
GLuint window_buffer, bar_buffer;
GLuint FBO[3], bar_FBO;
GLuint depthRBO[3], bar_depthRBO;
GLuint FBODataTexture[3], bar_FBODataTexture[3];
 
float windowWidth = 600.0f, windowHeight = 600.0f;
int imageProcess = ABSTRACTION;
float timer = 0.0f, linePos = 0.5f, center_x = 0.25, center_y = 0.5;
bool ComparisonBarActived = false, mouseDrag; 

void checkError(const char *functionName)
{
    GLenum error;
    while (( error = glGetError() ) != GL_NO_ERROR) {
        fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
}

// Print OpenGL context related information.
void dumpInfo(void)
{
    printf("Vendor: %s\n", glGetString (GL_VENDOR));
    printf("Renderer: %s\n", glGetString (GL_RENDERER));
    printf("Version: %s\n", glGetString (GL_VERSION));
    printf("GLSL: %s\n", glGetString (GL_SHADING_LANGUAGE_VERSION));
}

char** loadShaderSource(const char* file)
{
	FILE* fp = fopen(file, "rb");
	fseek(fp, 0, SEEK_END);
	long sz = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *src = new char[sz + 1];
	fread(src, sizeof(char), sz, fp);
	src[sz] = '\0';
	char **srcp = new char*[1];
	srcp[0] = src;
	return srcp;
}

void freeShaderSource(char** srcp)
{
	delete srcp[0];
	delete srcp;
}

void shaderLog(GLuint shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		GLchar* errorLog = new GLchar[maxLength];
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);

		printf("%s\n", errorLog);
		delete errorLog;
	}
}

void LoadProgram_1()
{
    program = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment.fs.glsl");
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
	shaderLog(vertexShader);
    shaderLog(fragmentShader);
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
	um4mvp = glGetUniformLocation(program, "um4mvp");
	tex_obj = glGetUniformLocation(program, "tex_obj");

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void LoadProgram_2(int id, char* fragment_name)
{
	programs[id] = glCreateProgram();
    GLuint vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource2 = loadShaderSource("vertex2.vs.glsl");
	char** fragmentShaderSource2 = loadShaderSource(fragment_name);  
    glShaderSource(vertexShader2, 1, vertexShaderSource2, NULL);
    glShaderSource(fragmentShader2, 1, fragmentShaderSource2, NULL);
	freeShaderSource(vertexShaderSource2);
	freeShaderSource(fragmentShaderSource2);
    glCompileShader(vertexShader2);
    glCompileShader(fragmentShader2);
	shaderLog(vertexShader2);
    shaderLog(fragmentShader2);  
    glAttachShader(programs[id], vertexShader2);
    glAttachShader(programs[id], fragmentShader2);
    glLinkProgram(programs[id]);
	       
	if(id == ABSTRACTION){ // blur  
		shader_now = glGetUniformLocation(programs[id], "shader_now");
		img_size = glGetUniformLocation(programs[id], "img_size");	
	}
	else if(id == SINWAVE){
		offset = glGetUniformLocation(programs[id], "offset");	
	}
	else if(id == WATERCOLOR){
		shader_now = glGetUniformLocation(programs[id], "shader_now");	
	}
	else if(id == MAGNIFIER){
		center = glGetUniformLocation(programs[id], "mouse");	
	}
	else if(id == RIPPLE){
		iGlobalTime = glGetUniformLocation(programs[id], "iGlobalTime");		
	}
}

void LoadProgram_3()
{
    program_3 = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex3.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("fragment3.fs.glsl");
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);
	freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
	shaderLog(vertexShader);
    shaderLog(fragmentShader);
    glAttachShader(program_3, vertexShader);
    glAttachShader(program_3, fragmentShader);
    glLinkProgram(program_3);
	line_Pos = glGetUniformLocation(program_3, "linePos");
}

void LoadProgram_4()
{
    program_4 = glCreateProgram();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	char** vertexShaderSource = loadShaderSource("vertex2.vs.glsl");
	char** fragmentShaderSource = loadShaderSource("watercolor.fs.glsl");
    glShaderSource(vertexShader, 1, vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, fragmentShaderSource, NULL);
	freeShaderSource(vertexShaderSource);            
	freeShaderSource(fragmentShaderSource);
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
	shaderLog(vertexShader);
    shaderLog(fragmentShader);
    glAttachShader(program_4, vertexShader);
    glAttachShader(program_4, fragmentShader);
    glLinkProgram(program_4);
	img_size_2 = glGetUniformLocation(program_4, "img_size");
	shader_now_2 = glGetUniformLocation(program_4, "shader_now");
}

void My_Init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);

	char* processing_name[] = { "red_blue.fs.glsl", "abstraction.fs.glsl", "sinwave.fs.glsl", "pixelization.fs.glsl",
								"watercolor.fs.glsl", "magnifier.fs.glsl", "ripple.fs.glsl" };

	LoadProgram_1();
	for(int i=0; i<sizeof(processing_name)/sizeof(char*); i++){
		LoadProgram_2(i, processing_name[i]);
	}
	LoadProgram_3(); 
	LoadProgram_4();

	program_2 = programs[ABSTRACTION];	// default

	glUseProgram(program);
}

void My_LoadModels()
{
    std::string inputfile = "sponza.obj"; 
	bool ret = tinyobj::LoadObj(shapes, materials, err, inputfile.c_str());

	if(ret)
	{
		printf("file open : %s\n", inputfile.c_str()); 
		printf("materials size : %d\n", materials.size());
		
		glGenTextures(materials.size(), &textures[0]);
		
		// For Each Material
		for(int i = 0; i < materials.size(); i++)
		{		 
			printf("%d : %s\n", i, materials[i].diffuse_texname.c_str());

			ILuint ilTexName; 
			ilGenImages(1, &ilTexName);
			ilBindImage(ilTexName);
			if(ilLoadImage(materials[i].diffuse_texname.c_str())) 
			{ 
				printf("load image\n");
				unsigned char *data = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 3];
				int Width = ilGetInteger(IL_IMAGE_WIDTH);
				int Height = ilGetInteger(IL_IMAGE_HEIGHT);
				ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);
				 
				glBindTexture(GL_TEXTURE_2D, textures[i]);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

				delete[] data;
				ilDeleteImages(1, &ilTexName);
			}
		}

		shapeCount = shapes.size();
	
		material_ids = (int**) malloc(sizeof(int*) * shapes.size());
		vertexCount = (int*) malloc(sizeof(int) * shapes.size());
		triangleCount = (int*) malloc(sizeof(int) * shapes.size());

		// For Each Shape (or Mesh, Object)
		for(int i = 0; i < shapes.size(); i++)
		{		
			triangleCount[i] = shapes[i].mesh.indices.size();

			float positions[50000], normals[50000], texcoords[50000];
			unsigned int indices[50000];
			material_ids[i] = (int*) malloc(sizeof(int) * shapes[i].mesh.material_ids.size());

			for(int count=0; count<shapes[i].mesh.positions.size(); count++){
				positions[count] = shapes[i].mesh.positions.at(count) * 1.2;
			}
			for(int count=0; count<shapes[i].mesh.normals.size(); count++){
				normals[count] = shapes[i].mesh.normals.at(count);
			}
			for(int count=0, texCount=0; count<shapes[i].mesh.texcoords.size(); count++){
				if(count % 3 != 2){
				//	if(i == shapeCount - 1)
				//	printf("%d , %f\n", texCount, shapes[i].mesh.texcoords.at(texCount));

					texcoords[count] = shapes[i].mesh.texcoords.at(texCount++);
				}
				else{
					texcoords[count] = 0;					
				}
			//	texcoords[count] = shapes[i].mesh.texcoords.at(count);
			}
			for(int count=0; count<shapes[i].mesh.indices.size(); count++){
			//	if(i == shapeCount - 1)
			//		printf("%d, %d\n", count, shapes[i].mesh.indices.at(count));
				indices[count] = shapes[i].mesh.indices.at(count);
			}
			for(int count=0; count<shapes[i].mesh.material_ids.size(); count++){
				material_ids[i][count] = shapes[i].mesh.material_ids.at(count);
			}
			  
			glGenBuffers(1, &buffer_position[i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_position[i]);
			
			glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
			 
			glGenBuffers(1, &buffer_normals[i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_normals[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

			glGenBuffers(1, &buffer_texcoords[i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffer_texcoords[i]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
			glGenBuffers(1, &buffer_indices[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_indices[i]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);		
		}
	}
	else{
		printf("openning fail\n");
	}

	printf("loading finish\n");

	std::string FBX_input[3] = { "zombie_walk.FBX", "zombie_fury.FBX", "zombie_dead.FBX" };

	for(int animate=0; animate<3; animate++){
		std::vector<tinyobj::shape_t> FBXshapes = FBX_shapes[animate];
		std::vector<tinyobj::material_t> FBXmaterials = FBX_materials[animate];
		std::string FBXerr = FBX_errs[animate];
		bool FBXret = LoadFbx(myFbx[animate], FBXshapes, FBXmaterials, FBXerr, FBX_input[animate].c_str());

		if(FBXret)
		{
			// For Each Material
			for(int i = 0; i < FBXmaterials.size(); i++)
			{		 
				printf("%d : %s\n", i, FBXmaterials[i].diffuse_texname.c_str());

				ILuint ilTexName;
				ilGenImages(1, &ilTexName);
				ilBindImage(ilTexName);
				if(ilLoadImage(FBXmaterials[i].diffuse_texname.c_str()))
				{ 
					printf("load image\n");
					unsigned char *data = new unsigned char[ilGetInteger(IL_IMAGE_WIDTH) * ilGetInteger(IL_IMAGE_HEIGHT) * 3];
					int Width = ilGetInteger(IL_IMAGE_WIDTH);
					int Height = ilGetInteger(IL_IMAGE_HEIGHT);
					ilCopyPixels(0, 0, 0, ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT), 1, IL_RGB, IL_UNSIGNED_BYTE, data);
				 
					glBindTexture(GL_TEXTURE_2D, FBX_textures[animate][i]);
					glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

					delete[] data;
					ilDeleteImages(1, &ilTexName);
				}
			}

			FBX_obj[animate].FBX_shapeCount = FBXshapes.size();

			FBX_obj[animate].FBX_material_ids = (int**) malloc(sizeof(int*) * FBXshapes.size());
			FBX_obj[animate].FBX_vertexCount = (int*) malloc(sizeof(int) * FBXshapes.size());
			FBX_obj[animate].FBX_triangleCount = (int*) malloc(sizeof(int) * FBXshapes.size());

			// For Each Shape (or Mesh, Object)
			for(int i = 0; i < FBXshapes.size(); i++)
			{		
				FBX_obj[animate].FBX_triangleCount[i] = FBXshapes[i].mesh.indices.size();

				float positions[50000], normals[50000], texcoords[50000];
				unsigned int indices[50000];
			
				FBX_obj[animate].FBX_material_ids[i] = (int*) malloc(sizeof(int) * FBXshapes[i].mesh.material_ids.size());

				for(int count=0; count<FBXshapes[i].mesh.positions.size(); count++){
					positions[count] = FBXshapes[i].mesh.positions.at(count) * 10;
					if(count%3 == 2){
						float z = positions[count] + 105; 
						positions[count] = positions[count-1];
						positions[count-1] = z;
					}
				}
				for(int count=0; count<FBXshapes[i].mesh.normals.size(); count++){
					normals[count] = FBXshapes[i].mesh.normals.at(count);
				}
				for(int count=0, texCount=0; count<FBXshapes[i].mesh.texcoords.size(); count++){
					if(count % 3 != 2){
						texcoords[count] = FBXshapes[i].mesh.texcoords.at(texCount++);
					}
					else{
						texcoords[count] = 0;
					}
				}
				for(int count=0; count<FBXshapes[i].mesh.indices.size(); count++){
					indices[count] = FBXshapes[i].mesh.indices.at(count);
				}
				for(int count=0; count<FBXshapes[i].mesh.material_ids.size(); count++){
					FBX_obj[animate].FBX_material_ids[i][count] = FBXshapes[i].mesh.material_ids.at(count);
				}
			  
				glGenBuffers(1, &FBX_obj[animate].FBX_buffer_position[i]);
				glBindBuffer(GL_ARRAY_BUFFER, FBX_obj[animate].FBX_buffer_position[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0); 
			 
				glGenBuffers(1, &FBX_obj[animate].FBX_buffer_normals[i]);
				glBindBuffer(GL_ARRAY_BUFFER, FBX_obj[animate].FBX_buffer_normals[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
		//		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

				glGenBuffers(1, &FBX_obj[animate].FBX_buffer_texcoords[i]);
				glBindBuffer(GL_ARRAY_BUFFER, FBX_obj[animate].FBX_buffer_texcoords[i]);
				glBufferData(GL_ARRAY_BUFFER, sizeof(texcoords), texcoords, GL_STATIC_DRAW);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	
				glGenBuffers(1, &FBX_obj[animate].FBX_buffer_indices[i]);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FBX_obj[animate].FBX_buffer_indices[i]);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
			}
		}
		else{
			printf("openning fail\n");
		}
	}

	glGetError();
	glBindTexture(GL_TEXTURE_2D, 0);

	/* for window FBO (second FBO) */
	glGenVertexArrays(1, &window_vao);
	glBindVertexArray(window_vao);

	glGenBuffers(1, &window_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, window_buffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(window_positions),window_positions,	GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, 0);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, (const GLvoid*)(sizeof(GL_FLOAT)*2));

	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	glGenFramebuffers( 3, &FBO[0] );

	/* for comparison bar FBO (third FBO) */
	glGenVertexArrays(1, &bar_vao);
	glBindVertexArray(bar_vao);

	glGenBuffers(1, &bar_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, bar_buffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(window_positions),window_positions,	GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, 0);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(GL_FLOAT)*4, (const GLvoid*)(sizeof(GL_FLOAT)*2));

	glEnableVertexAttribArray( 0 );
	glEnableVertexAttribArray( 1 );

	glGenFramebuffers( 1, &bar_FBO );
}

// GLUT callback. Called to draw the scene.
void My_Display()
{	
	timer += 0.5f;       
	 
	glBindTexture( GL_TEXTURE_2D, 0 ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO[0] );
	glDrawBuffer( GL_COLOR_ATTACHMENT0 );
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0); 

	glUseProgram(program);
	glBindVertexArray(vao);

	std::vector<tinyobj::shape_t> new_shapes;
	fbx_handles MyFbx = myFbx[animateMode];
	GetFbxAnimation(MyFbx, new_shapes, frame); // The Last Parameter is A Float in [0, 1], Specifying The Animation Location You Want to Retrieve
	  
	for(int i = 0; i < new_shapes.size(); i++)
	{ 
		float new_position[50000];
		for(int count=0; count<new_shapes[i].mesh.positions.size(); count++){
			new_position[count] = new_shapes[i].mesh.positions.at(count) * 10;
			if(count%3 == 2){
				float z = new_position[count] + 105; 
				new_position[count] = new_position[count-1];
				new_position[count-1] = z;
			}
		}

		glBindBuffer(GL_ARRAY_BUFFER, FBX_obj[animateMode].FBX_buffer_position[i]);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(new_position), new_position);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		
		glBindBuffer(GL_ARRAY_BUFFER, FBX_obj[animateMode].FBX_buffer_texcoords[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, FBX_obj[animateMode].FBX_buffer_indices[i]);
	
		glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp)); 
		 
		for(int j=0; j<FBX_obj[animateMode].FBX_triangleCount[i]/3; j++){
			int FBX_texture_ids = FBX_obj[animateMode].FBX_material_ids[i][j];
			glBindTexture(GL_TEXTURE_2D, FBX_textures[animateMode][FBX_texture_ids]); 
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * j));	
		}	   
	} 

	frame = (frame >= 1.0) ? 0.0f : frame+0.05;
		
	// For Each Shapes You Loaded In My_LoadModels()
	for(int i = 0; i < shapeCount; i++)
	{ 
		glBindBuffer(GL_ARRAY_BUFFER, buffer_position[i]); 
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		 
	//	glBindBuffer(GL_ARRAY_BUFFER, buffer_normals[i]);
	//	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  
		 
		glBindBuffer(GL_ARRAY_BUFFER, buffer_texcoords[i]);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);  

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_indices[i]);
		glUniformMatrix4fv(um4mvp, 1, GL_FALSE, value_ptr(mvp));

		for(int j=0; j<triangleCount[i]/3; j++){
			int texture_ids = material_ids[i][j];   
			glBindTexture(GL_TEXTURE_2D, textures[texture_ids]); 
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, (void*)(sizeof(unsigned int) * 3 * j));
		}
	}
	        
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
	glBindTexture(GL_TEXTURE_2D, 0);


	if(imageProcess == WATERCOLOR){
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO[1] );
		glDrawBuffer( GL_COLOR_ATTACHMENT1 );
	}
	else if(ComparisonBarActived){ 
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, bar_FBO );
		glDrawBuffer( GL_COLOR_ATTACHMENT1 );
	}
	
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glClearColor( 1.0f, 1.0f, 1.0f, 1.0f ); 

	glBindVertexArray(window_vao);
	glUseProgram(program_2);
	glUniform2f(img_size, windowWidth, windowHeight);
	glUniform1f(offset, timer);
	glUniform1f(shader_now, 0);
	glUniform2f(center, center_x, center_y); 	
	glUniform1f(iGlobalTime, timer);
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture( GL_TEXTURE_2D, FBODataTexture[0] );
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	 
	if(imageProcess == WATERCOLOR){
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		 
		if(ComparisonBarActived){
			glBindFramebuffer( GL_DRAW_FRAMEBUFFER, bar_FBO );
			glDrawBuffer( GL_COLOR_ATTACHMENT1 );    
		}   
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f ); 
		   
		glBindVertexArray(window_vao); 
		glUseProgram(program_4); 
		glUniform1i(shader_now_2, 1);  
		glUniform2f(img_size_2, windowWidth, windowHeight);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture( GL_TEXTURE_2D, FBODataTexture[0] );
		glActiveTexture(GL_TEXTURE1);
		glBindTexture( GL_TEXTURE_2D, FBODataTexture[1] );
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	} 
	 
//	int bindNum = (imageProcess == BLOOM) ? 1 : 0;

	if(ComparisonBarActived){
		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, 0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		glClearColor( 1.0f, 1.0f, 1.0f, 1.0f );  
		   
		glBindVertexArray(bar_vao);
		glUseProgram(program_3);
		glUniform1f(line_Pos, linePos);
		glActiveTexture(GL_TEXTURE0);  
		glBindTexture( GL_TEXTURE_2D, FBODataTexture[0] ); 
		glActiveTexture(GL_TEXTURE1); 
		glBindTexture( GL_TEXTURE_2D, bar_FBODataTexture[0] ); 
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
	} 
	 
    glutSwapBuffers();  
}

void ChangeCamera(bool scene)
{
	if(scene){
		mat4 Projection = perspective(radians(45.0f), 3.0f / 3.0f, 0.1f, 2500.0f);
		mat4 View = lookAt(vec3(-200.0f, 100.0f, 200.0f), vec3(0.0f+proj_x, 100.0f+proj_y, 0.0f+proj_z), vec3(0.0f, 1.0f, 0.0f));
		mat4 Model = mat4(1.0f);
		mvp = Projection * View * Model;
	}
	else{
		mvp = ortho(-20.0f, 20.0f, -20.0f, 20.0f, -20.0f, 20.0f);
		mvp = mvp * lookAt(vec3(0.0f+proj_x, 0.0f+proj_y, 0.0f+proj_z), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
	}
}

void My_Reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	windowWidth = width;
	windowHeight = height;

	float viewportAspect = (float)width / (float)height;
	ChangeCamera(true);
	
	/* for window textures */
	glDeleteTextures(3, &FBODataTexture[0]);
	glDeleteRenderbuffers(3, &depthRBO[0]);
//	glDeleteTextures(1, &FBODataTexture);
	glGenRenderbuffers( 3, &depthRBO[0] );
	glGenTextures( 3, &FBODataTexture[0] );

	static const GLenum draw_buffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	for(int i=0; i<2; i++){
		glBindRenderbuffer( GL_RENDERBUFFER, depthRBO[i] );
		glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height );

		glBindTexture( GL_TEXTURE_2D, FBODataTexture[i]);
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		glBindFramebuffer( GL_DRAW_FRAMEBUFFER, FBO[i] );
		glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRBO[i] );
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, draw_buffers[i], GL_TEXTURE_2D, FBODataTexture[i], 0 );
	}
//	glDrawBuffers(2, draw_buffers);

	/* for bar textures */
	glDeleteTextures(3, &bar_FBODataTexture[0]);
	glDeleteRenderbuffers(1, &bar_depthRBO);
//	glDeleteTextures(1, &FBODataTexture);
	glGenRenderbuffers( 1, &bar_depthRBO );
	glBindRenderbuffer( GL_RENDERBUFFER, bar_depthRBO );
	glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width, height );

	glGenTextures( 3, &bar_FBODataTexture[0] );
	glBindTexture( GL_TEXTURE_2D, bar_FBODataTexture[0]);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, bar_FBO );
	glFramebufferRenderbuffer( GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, bar_depthRBO );
	glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, bar_FBODataTexture[0], 0 );
}

void My_Timer(int val)
{
	timer_cnt++;
	glutPostRedisplay();
	if(timer_enabled)
	{
		glutTimerFunc(timer_speed, My_Timer, val);
	}
}

void My_Mouse(int button, int state, int x, int y)
{
	if(button == 0){
		if(ComparisonBarActived){
			float tex_x = x / windowWidth;

			if(state == GLUT_DOWN)
			{
				if(tex_x - linePos <= 0.005 && tex_x - linePos >= -0.005 && !mouseDrag){
					mouseDrag = true;
				}
			}
			else if(state == GLUT_UP && mouseDrag) 
			{
				mouseDrag = false;
				linePos = (x < 0.995 * windowWidth && x > 0.005 * windowWidth) ? (x / windowWidth) : 
								(x <= 0.005 * windowWidth) ? 0.01 : 0.99;
			}     
		}
		 
		if(imageProcess == MAGNIFIER && state == GLUT_DOWN){
			center_x = x / windowWidth;
			center_y = 1 - y / windowHeight;
			 
			cout << center_x << " " << center_y << endl;
		}
	}
}

void My_Keyboard(unsigned char key, int x, int y)
{
	printf("Key %c is pressed at (%d, %d)\n", key, x, y);
	printf("x : %f\n", proj_x);
	printf("y : %f\n", proj_y);
	printf("z : %f\n", proj_z);
	bool scene = true;
	switch(key)
	{
	case 'A': case 'a':
		proj_x -= 2.0;
		ChangeCamera(scene);
		glutSwapBuffers();
		break;
	case 'D': case 'd':
		proj_x += 2.0;
		ChangeCamera(scene);
		glutSwapBuffers();
		break;
	case 'W': case 'w':
		proj_y += 2.0;
		ChangeCamera(scene);
		glutSwapBuffers();
		break;
	case 'S': case 's':
		proj_y -= 2.0;
		ChangeCamera(scene);
		glutSwapBuffers();
		break;
	case 'Z': case 'z':
		proj_z += 2.0;
		ChangeCamera(scene);
		glutSwapBuffers();
		break;
	case 'X': case 'x':
		proj_z -= 2.0;
		ChangeCamera(scene);
		glutSwapBuffers();
		break;
	}
}

void My_SpecialKeys(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		printf("F1 is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_PAGE_UP:
		printf("Page up is pressed at (%d, %d)\n", x, y);
		break;
	case GLUT_KEY_LEFT:
		printf("Left arrow is pressed at (%d, %d)\n", x, y);
		break;
	default:
		printf("Other special key is pressed at (%d, %d)\n", x, y);
		break;
	}
}

void My_Menu(int id){ } 

void animation_func(int id)
{
	switch(id)
	{
	case WALK:
		animateMode = WALK;
		break;
	case FURY:
		animateMode = FURY;
		break;
	case DEAD:
		animateMode = DEAD;
		break;
	}
}

void processing_func(int id)
{	
	if(id == BAR){
		ComparisonBarActived = !ComparisonBarActived;
	}
	else{
		imageProcess = id; 
		program_2 = programs[id];
	}
}

int main(int argc, char *argv[])
{
	// Initialize GLUT and GLEW, then create a window.
	////////////////////
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Assignment 03 102062104"); // You cannot use OpenGL functions before this line; The OpenGL context must be created first by glutCreateWindow()!
	glewInit();
	ilInit();
    ilEnable(IL_ORIGIN_SET);
    ilOriginFunc(IL_ORIGIN_LOWER_LEFT);
	dumpInfo();
	My_Init();
	My_LoadModels();
	////////////////////

	// Create a menu and bind it to mouse right button.
	////////////////////////////
	int menu_main = glutCreateMenu(My_Menu);
	int menu_animation = glutCreateMenu(animation_func);
	int menu_processing = glutCreateMenu(processing_func);

	glutSetMenu(menu_main);
	glutAddSubMenu("Animation", menu_animation);
	glutAddSubMenu("Processing", menu_processing);
	  
	glutSetMenu(menu_animation);
	glutAddMenuEntry("Walk", WALK);
	glutAddMenuEntry("Fury", FURY);
	glutAddMenuEntry("Dead", DEAD);
	
	glutSetMenu(menu_processing);
	glutAddMenuEntry("ComparisonBar", BAR);
	glutAddMenuEntry("Red-Blue", RED_BLUE);
	glutAddMenuEntry("Abstraction", ABSTRACTION);
	glutAddMenuEntry("SinWave", SINWAVE);
	glutAddMenuEntry("Pixelization", PIXELIZATION);
	glutAddMenuEntry("WaterColor", WATERCOLOR);
	glutAddMenuEntry("Magnifier", MAGNIFIER);
	glutAddMenuEntry("Ripple", RIPPLE);

	glutSetMenu(menu_main); 
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	////////////////////////////

	// Register GLUT callback functions.
	///////////////////////////////
	glutDisplayFunc(My_Display);
	glutReshapeFunc(My_Reshape);
	glutMouseFunc(My_Mouse);
	glutKeyboardFunc(My_Keyboard);
	glutSpecialFunc(My_SpecialKeys);
	glutTimerFunc(timer_speed, My_Timer, 0); 
	///////////////////////////////

	// Enter main event loop.
	//////////////
	glutMainLoop();
	//////////////
	return 0;
}