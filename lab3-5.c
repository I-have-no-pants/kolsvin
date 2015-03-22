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


Point3D lightSourcesColorsArr[] = { {0.9f, 0.9f, 0.9f}, // Red light
                                 {0.8f, 0.8f, 0.8f}, // Green light
                                 {0.8f, 0.8f, 0.8f}, // Green light
                                 {0.0f, 0.0f, 0.0f} }; // White light

GLfloat specularExponent[] = {100.0, 200.0, 600.0, 5.0};
GLint isDirectional[] = {0,1,1,0};

Point3D lightSourcesDirectionsPositions[] = { {10.0f, 5.0f, 0.0f}, // Red light, positional
                                       {20.0f, 5.0f, 10.0f}, // Green light, positional
                                       {-1.0f, 10.0f, -10.0f}, // Blue light along X
                                       {0.0f, 0.0f, -1.0f} }; // White light along Z



GLfloat projectionMatrix[] = {    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
                                            0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
                                            0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
                                            0.0f, 0.0f, -1.0f, 0.0f };



typedef struct worldObject {
    Model * model;
    mat4 matrix;
    mat4 LODmatrix;
    GLuint texture[3];
    GLuint program;
} worldObject;

#define nStatic 40
worldObject staticObjects[nStatic];

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

void DrawObject(worldObject obj, GLfloat t) {
    GLuint program = obj.program;
    glUseProgram(program);

    glUniformMatrix4fv(glGetUniformLocation(program, "CameraMatrix"), 1, GL_TRUE, CameraMatrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE,  obj.matrix.m);
    glUniformMatrix4fv(glGetUniformLocation(program, "myLODMatrix"), 1, GL_TRUE,  obj.LODmatrix.m);
    //glUniformFloat(glGetUniformLocation(program, "myMatrix"), 1, GL_TRUE,  obj.matrix.m);

    glUniform1f(glGetUniformLocation(program, "worldTime"), t);
    glUniform1f(glGetUniformLocation(program, "worldLOD"), 0.006);

    glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
    glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
    glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
    glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

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
    GLuint agdgTexture;
    GLuint materialTexture;
    GLuint normalTexture;

    Model *wallL;
    Model *wallLR;
    Model *agdg;

    LoadTGATextureSimple("textures/palette.tga", &texture);
    LoadTGATextureSimple("textures/material.tga", &materialTexture);
    LoadTGATextureSimple("textures/agdg.tga", &agdgTexture);
    LoadTGATextureSimple("textures/normalmap.tga", &agdgTexture);

    wallL = LoadModelPlus("models/platformGreen.obj");
    wallLR = LoadModelPlus("models/wallpiece.obj");
    agdg = LoadModelPlus("models/agdg.obj");

    Model * BlockGrey = LoadModelPlus("models/BlockGrey.obj");

    void SetMaterial(worldObject *o) {
        o->texture[0] = texture;
        o->texture[1] = materialTexture;
        o->program = program;
    }

    int modelN=0;
    
    for (int i=0; i<nStatic;i++) {
        SetMaterial(&staticObjects[i]);
    }

    for (int x = 0; x<5;x++) {

        for (int z = 0; z<5;z++) {

            staticObjects[modelN].model = wallL;
            staticObjects[modelN].matrix = T(x*32, -48, z*32); // 40 is good displacement for walls
            staticObjects[modelN].LODmatrix = Mult(T(x*32, -64, (x+z)*16), S(0.5,0.5,0.5)); // 40 is good displacement for walls
            modelN++;
        }
    }

    for (int x = 0; x<5;x++) {
            staticObjects[modelN].model = wallLR;
            staticObjects[modelN].matrix = T(x*32, -16, -32);
            staticObjects[modelN].LODmatrix = Mult(T(x*16, 16, -64), S(0.5,0.5,0.5));
            modelN++;
    }

    for (int x = 0; x<5;x++) {
            staticObjects[modelN].model = BlockGrey;
            staticObjects[modelN].matrix = Mult(T(x*32, -16, 160), Ry(Pi));
            staticObjects[modelN].LODmatrix = Mult(T((x/2)*16, ((x+1) /2)*16, 256), Mult(Ry(Pi/2.0), S(0.5,0.5,0.5)));
            modelN++;
    }

    for (int x = 0; x<5;x++) {
            staticObjects[modelN].model = wallL;
            staticObjects[modelN].matrix = T((x+1)*-32, -64, 64);
            staticObjects[modelN].LODmatrix = Mult(T(-256, 32+x*16, 64), Mult(Rz(-Pi/2.0), S(0.5,0.5,0.5)));
            modelN++;
    }

    //staticObjects[4].LODmatrix = Mult(T(30, -30, -10), Mult(Rz(90), S(0.4,0.4,0.4))); // 40 is good displacement for walls

/*
    staticObjects[5].model = wallLR;
    staticObjects[5].matrix = T(80, -10, -40); // 40 is good displacement for walls
    staticObjects[5].LODmatrix = Mult(T(20, -10, -50), S(0.4,0.4,0.4)); // 40 is good displacement for walls

    staticObjects[6].model = agdg;
    staticObjects[6].matrix = T(0, 3, -10); // 40 is good displacement for walls
    staticObjects[6].LODmatrix = Mult(T(0, -10, -30), S(0.2,0.2,0.2)); // 40 is good displacement for walls
*/
}


void OnTimer(int value) {
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}



void init(void) {
   
    skybox = LoadModelPlus("skybox.obj");

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

    LoadTGATextureSimple("textures/SkyBox512.tga", &skyTexture);
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
    GLfloat ts = 2;
    GLfloat t = ts * (GLfloat)glutGet(GLUT_ELAPSED_TIME);

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

    CameraPos = VectorAdd(CameraPos, ScalarMult(Speed,0.5f * ts));

    vec3 up = {0,1,0};
    CameraMatrix = lookAtv(CameraPos,VectorAdd(CameraPos,CameraTarget),up);

    lightSourcesDirectionsPositions[0].x = CameraPos.x;
    lightSourcesDirectionsPositions[0].z = CameraPos.z;
    lightSourcesDirectionsPositions[0].y = CameraPos.y;

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

    int i;
    for (i = 0; i<nStatic;i++) {
        DrawObject(staticObjects[i], t);
    }

    glutSwapBuffers();
}



int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitContextVersion(3, 2);
    glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    glutCreateWindow (">Half Life 3");
    glutDisplayFunc(display); 
    init ();
    glutMainLoop();
}
