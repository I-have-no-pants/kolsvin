// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	#include "MicroGlut.h"
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "GL_utilities.h"
#include <math.h>
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"


#define Pi 3.1415

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 600

int screen_center_x = SCREEN_WIDTH/2;
int screen_center_y = SCREEN_HEIGHT/2;

float mouse_sensitivity = 0.005f;

#define near 1.0
#define far 300.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5


Point3D lightSourcesColorsArr[] = { {1.0f, 0.0f, 0.0f}, // Red light
                                 {0.0f, 1.0f, 0.0f}, // Green light
                                 {0.0f, 0.0f, 1.0f}, // Blue light
                                 {1.0f, 1.0f, 1.0f} }; // White light

GLfloat specularExponent[] = {10.0, 20.0, 60.0, 5.0};
GLint isDirectional[] = {0,0,1,1};

Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional
                                       {0.0f, 5.0f, 10.0f}, // Green light, positional
                                       {-1.0f, 0.0f, 0.0f}, // Blue light along X
                                       {0.0f, 0.0f, -1.0f} }; // White light along Z



GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                            0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                            0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                            0.0f, 0.0f, -1.0f, 0.0f };



typedef struct worldObject {
    Model * model;
    mat4 matrix;
    GLuint texture[3];
    GLuint program;
} worldObject;

int nStatic=12;
worldObject staticObjects[12];

worldObject *windmill_blades[4];

Model *skybox;
Model *windmill_blade;

GLuint normalShader;
GLuint program;
GLuint skyShader;

GLuint skyTexture;
GLuint maskrosTexture;

mat4 CameraMatrix;
vec3 CameraPos;
vec3 CameraTarget= {0,0,1};

int oldX=100, oldY=100;
float vx=0, vy=0;

void RotateCamera(int x, int y) {
    if (x==screen_center_x && y == screen_center_y) {
        return;
    }

    int newX = x;
    int newY = y;

    // Camera rotation
    int dx = -screen_center_x+newX;
    int dy = -screen_center_y+newY;
    oldX = newX;
    oldY = newY;

    vx-= dx*mouse_sensitivity;
    vy-= dy*mouse_sensitivity;

    CameraTarget = (vec3){1*sin(vx)*cos(vy) ,1*sin(vy), 1*cos(vx)*cos(vy)};

    glutPassiveMotionFunc(0);
    glutWarpPointer(screen_center_x,screen_center_y);
    glutPassiveMotionFunc(RotateCamera);
}

void DrawObject(worldObject obj) {
    GLuint program = obj.program;
    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "CameraMatrix"), 1, GL_TRUE, CameraMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE,  obj.matrix.m);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj.texture[0]);
    glUniform1i(glGetUniformLocation(program, "texUnit0"), 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, obj.texture[1]);
    glUniform1i(glGetUniformLocation(program, "texUnit1"), 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, obj.texture[2]);
    glUniform1i(glGetUniformLocation(program, "texUnit2"), 2);

    DrawModel(obj.model, program, "in_Position", "in_Normal", "in_TexCoord");
}


void LoadWorld() {
    glUseProgram(program);

    GLuint texture;
    GLuint conc;
    GLuint spots;
    GLuint maskros512;

    Model *windmill_walls;
    Model *windmill_roof;
    Model *windmill_balcony;
    Model *ground;
    Model *bunny;

    LoadTGATextureSimple("rutor.tga", &texture);
    LoadTGATextureSimple("conc.tga", &conc);
    LoadTGATextureSimple("spots.tga", &spots);
    LoadTGATextureSimple("maskros512.tga", &maskros512);

    windmill_walls = LoadModelPlus("windmill/windmill-walls.obj");
    windmill_roof = LoadModelPlus("windmill/windmill-roof.obj");
    windmill_balcony = LoadModelPlus("windmill/windmill-balcony.obj");
    ground = LoadModelPlus("cubeplus.obj");

    bunny = LoadModelPlus("bunnyplus.obj");

    staticObjects[0].model = ground;
    staticObjects[0].matrix = S(100, 0.01, 100);
    staticObjects[0].texture[0] = texture;
    staticObjects[0].texture[1] = spots;
    staticObjects[0].texture[2] =  maskros512;
    staticObjects[0].program = program;

    staticObjects[1].model = windmill_walls;
    staticObjects[1].matrix = T(0, 0, 0);
    staticObjects[1].texture[0] = texture;
    staticObjects[1].texture[1] = texture;
    staticObjects[1].texture[2] = texture;
    staticObjects[1].program = normalShader;

    staticObjects[2].model = windmill_roof;
    staticObjects[2].matrix = T(0, 0, 0);
    staticObjects[2].texture[0] = texture;
    staticObjects[2].texture[1] = texture;
    staticObjects[2].texture[2] = texture;
    staticObjects[2].program = normalShader;

    staticObjects[3].model = windmill_balcony;
    staticObjects[3].matrix = T(0, 0, 0);
    staticObjects[3].texture[0] = texture;
    staticObjects[3].texture[1] = texture;
    staticObjects[3].texture[2] = texture;
    staticObjects[3].program = normalShader;

    staticObjects[4].model = bunny;
    staticObjects[4].matrix = T(10, 0.5, 0);
    staticObjects[4].texture[0] = texture;
    staticObjects[4].texture[1] = spots;
    staticObjects[4].texture[2] = texture;
    staticObjects[4].program = program;

    staticObjects[5].model = bunny;
    staticObjects[5].matrix = T(0, 0.5, 30);
    staticObjects[5].texture[0] = conc;
    staticObjects[5].texture[1] = spots;
    staticObjects[5].texture[2] = texture;
    staticObjects[5].program = program;

    staticObjects[6].model = bunny;
    staticObjects[6].matrix = T(-10, 0.5, -10);
    staticObjects[6].texture[0] = spots;
    staticObjects[6].texture[1] = texture;
    staticObjects[6].texture[2] = texture;
    staticObjects[6].program = program;

    staticObjects[7].model = skybox;
    staticObjects[7].matrix = T(-10, 0.5, -10);
    staticObjects[7].texture[0] = skyTexture;
    staticObjects[7].texture[1] = texture;
    staticObjects[7].texture[2] = texture;
    staticObjects[7].program = skyShader;

    int i;
    for (i=0; i<4;i++) {
        windmill_blades[i] = &staticObjects[8+i];
        staticObjects[8+i].model = windmill_blade;
        staticObjects[8+i].texture[0] = texture;
        staticObjects[8+i].texture[1] = texture;
        staticObjects[8+i].texture[2] = texture;
        staticObjects[8+i].program = normalShader;
    }


    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);


}


void OnTimer(int value)
{
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}



void init(void)
{
   
    windmill_blade = LoadModelPlus("windmill/blade.obj");   
    skybox = LoadModelPlus("skybox.obj");
	// vertex buffer object, used for uploading the geometry

	// Reference to shader program

	dumpInfo();

	// GL inits
	glClearColor(0.5,0.2,0.2,0);
    
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	skyShader = loadShaders("lab3-3.vert", "lab3-3.frag");
	printError("init shader");
	program = loadShaders("lab3-5.vert", "lab3-5.frag");

    normalShader = loadShaders("lab3-5.vert", "normal_shader.frag");
	

    // End of upload of geometry
    glutTimerFunc(20, &OnTimer, 0);

    // Load textures


    glUseProgram(program);
    glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

    glUseProgram(skyShader);
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

    glUseProgram(normalShader);
    glUniformMatrix4fv(glGetUniformLocation(normalShader, "projectionMatrix"), 1, GL_TRUE, projectionMatrix);

    LoadTGATextureSimple("SkyBox512.tga", &skyTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyTexture);   
    glUniform1i(glGetUniformLocation(skyShader, "texUnit"), 0); // Texture unit 0

    

    glActiveTexture(GL_TEXTURE0);

    printError("init arrays");


    // Set dafault camera

    vec3 pos = {20,2,-20};
    vec3 look = {0,5,0};
    vec3 up = {0,1,0};

    CameraPos = (vec3){0,4,20};

    CameraMatrix = lookAtv(pos,look,up);

    glutPassiveMotionFunc(RotateCamera);
    initKeymapManager();

    LoadWorld();
}


void display(void) {
    printError("pre display");
    // Update animation
    GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);
    GLfloat ts = t*0.003;

    // Camera position

    vec3 Speed = {0,0,0};
    bool moving = false;
    
    if (keyIsDown('a')) {
        Speed.x = -1;
        moving = true;
    } else if (keyIsDown('d')) {
        Speed.x = 1;
        moving = true;
    }

    if (keyIsDown('w')) {
        Speed.z = -1;
        moving = true;
    } else if (keyIsDown('s')) {
        Speed.z = 1;
        moving = true;
    }

    Speed = MultMat3Vec3(mat4tomat3(Transpose(CameraMatrix)), Speed);

    if (moving) {
        Speed.y=0;
        Speed = Normalize(Speed);
    }

    CameraPos = VectorAdd(CameraPos, ScalarMult(Speed,0.5f));

    vec3 up = {0,1,0};
    CameraMatrix = lookAtv(CameraPos,VectorAdd(CameraPos,CameraTarget),up);

	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // DRAW SKYBOX
    glDisable(GL_DEPTH_TEST);
    glUseProgram(skyShader);
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "CameraMatrix"), 1, GL_TRUE, CameraMatrix.m);
    mat4 trans = T(CameraPos.x, CameraPos.y-0.5, CameraPos.z);
    glUniformMatrix4fv(glGetUniformLocation(skyShader, "myMatrix"), 1, GL_TRUE, trans.m);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, skyTexture);
    DrawModel(skybox, skyShader, "in_Position", "in_Normal", "in_TexCoord");
    glEnable(GL_DEPTH_TEST);

    //glUseProgram(program);
    //glUniformMatrix4fv(glGetUniformLocation(program, "CameraMatrix"), 1, GL_TRUE, CameraMatrix.m);

    int i;
    for (i = 0; i<nStatic;i++) {
        DrawObject(staticObjects[i]);
    }

    mat4 bladeRot = Rx(ts*2);
    // Blade 1
    trans = T(4.5, 9.18, 0);

    mat4 rot = Rx(0);
    rot = Mult(bladeRot, rot);
    mat4 comb = Mult( trans, rot);

    windmill_blades[0]->matrix = comb;

    // Blade 2
    rot = Rx(Pi/2);
    rot = Mult(bladeRot, rot);
    comb = Mult( trans, rot);
    windmill_blades[1]->matrix = comb;

    // Blade 3
    rot = Rx(Pi);
    rot = Mult(bladeRot, rot);
    comb = Mult( trans, rot);
    windmill_blades[2]->matrix = comb;

    // Blade 4
    rot = Rx(-Pi/2);
    rot = Mult(bladeRot, rot);
    comb = Mult( trans, rot);
    windmill_blades[3]->matrix = comb;

	glutSwapBuffers();
}



int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow (">Half Life 3");
	glutDisplayFunc(display); 
	init ();
	glutMainLoop();
}