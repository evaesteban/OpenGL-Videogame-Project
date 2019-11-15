/* Source code for Tank Maze Game */

/* Includes */

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <Shader.h>
#include <Vector.h>
#include <Matrix.h>
#include <Mesh.h>
#include <Texture.h>
#include <SphericalCameraManipulator.h>
#include <iostream>
#include <math.h>
#include <string>
#include <GL/gl.h>
#include <sstream>
#include<stdlib.h>

/* Function Prototypes */

bool initGL(int argc, char** argv);
void display(void);
void keyboard(unsigned char key, int x, int y);
void keyUp(unsigned char key, int x, int y);
void handleKeys();
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void Timer(int value);
void initShader();
void initTexture(std::string filename, GLuint & textureID);
void render2dText(std::string text, float r, float g, float b, float x, float y);
void drawTank();
void checkTank();
void updateTank();
void drawBall();
void createMenu();
void menu(int num);
void turretMotion(float x, float y);
Vector3f GetOGLPos(int x, int y);
void tankFall();
void fireBall();
void setBall();
void ballCollision();

/* Global Variables */

//Shader parameters
GLuint shaderProgramID; // Shader Program ID
GLuint vertexPositionAttribute; // Vertex Position Attribute Location
GLuint parameterLocation;

//Global strings for score and time
std::string scoreString;
std::string timeString;

//Global meshes
Mesh cube;
Mesh coin;
Mesh chassis;
Mesh turret;
Mesh frontWheel;
Mesh backWheel; 
Mesh ball;

//Material Properties
GLuint vertexNormalAttribute;
GLuint LightPositionUniformLocation;                // Light Position Uniform   
GLuint AmbientUniformLocation;
GLuint SpecularUniformLocation;
GLuint SpecularPowerUniformLocation;

//Lighting of the scene
Vector3f lightPosition= Vector3f(20.0,20.0,20.0);   // Light Position 
Vector3f ambient    = Vector3f(0.15,0.15,0.15);
Vector3f specular   = Vector3f(1.0,1.0,1.0);
float specularPower = 10.0;
int light = 0;
int medium = 1;
int dark = 0;

//Global viewing parameters 
SphericalCameraManipulator cameraManip;
Matrix4x4 ModelViewMatrix; //ModelView Matrix
Matrix4x4 tempMat; //To store model view matrix

GLuint MVMatrixUniformLocation; //ModelView Matrix Uniform
Matrix4x4 ProjectionMatrix;  //Projection Matrix
GLuint ProjectionUniformLocation; //Projection Matrix Uniform Location

GLuint vertexTexcoordAttribute; //Vertex Texcoord Attribute Location
GLuint TextureMapUniformLocation; //Texture Map Location

GLuint texture_cube; //To store cube texture  
GLuint texture_coin; //To store coin texture  
GLuint texture_tank; //To store tank texture  
GLuint texture_ball; //To store ball texture  

//Screen size
int screenWidth  = 720;
int screenHeight  = 720;

//Array of key states
bool keyStates[256]; 

//Maze and game parameters
int maze[50][50]; //Maze
int rows, columns;
int flag = 0; //To find initial tank position
int paused = 0; //Will equal true if the game is paused
int far = 1; //To set different camera perspectives
int stop = 0; //Will equal true if tank has fallen
int falling = 0; //Will equal true if the tank needs to fall
int firing = 0; //Will equal one to fire the ball
int over = 0; //Will equal true if the tank has fallen
int nCoins = 0; //Number of coins
float angleCoin = 0; //Rotation of coins
int path = 0;

//Time and score parameters
int score = 0; //Score
int timeMilli = 0; //Time in milliseconds
int timeSec; //Time in seconds
int maxSeconds = 60; //Maximum allowed time
int timePaused = 0; //Time at which the game has been paused
int timeDifference = 0; //Amount of time the game has been paused

//Angle parameters
float angleTurret = 0.0; //Angle of rotation of turret
int angleFall = 0; //Angle of rotation of tank when falling
int angleBall = 0; //Angle of ball with respect to turret
int angleWheel = 0; //Angle of rotation of wheels
float angle = 0.0; //Angle of rotation of tank

//Tank parameters
float v = 0.0; //Tank velocity magnitude
float vX = 0.0; //Tank velocity component in the x direction
float vY = 0.0; //Tank velocity component in the y direction
float vZ = 0.0; //Tank velocity component in the z direction
float vMax = 0.1; //Maximum tank velocity
float tankX; //Tank position in the x direction
float tankY = 1.0; //Tank position in the y direction
float tankZ; //Tank position in the z direction
float gravity = 9.82; //Gravity value, constant

//Ball parameters
float ballX; //Ball position in the x direction
float ballY; //Ball position in the y direction
float ballZ; //Ball position in the z direction
float ballVX; //Ball velocity component in the x direction
float ballVY = 0.0; //Ball velocity component in the y direction
float ballVZ; //Ball velocity component in the z direction


/* Main Program Entry */

int main(int argc, char** argv)
{    

   // a:
    //Initialise OpenGL
    if(!initGL(argc, argv))
        return -1;

    //Play intructions recording
    system("canberra-gtk-play -f graphics3.wav &");

    //Create menu
    createMenu();
     
    //Initialise Key States to false;    
    for(int i = 0 ; i < 256; i++)
        keyStates[i] = false;

    char wholeMLine[350]; //To store the whole first line of the input file
    char wholeNLine[350]; //To store the whole second line of the input file
    char wholeCoordLine[350]; //To store the whole coordenate line of the input file
    FILE* file; //Pointer to file
    int i; //To iterate through the file
    int value; //Value of maze position: 0 for empty space, 1 for cube and 2 for coin
    int x = 0; //Maze position coordinates
    int y = 0;

    file = stdin; //Open file with read access

    if(file != NULL){ //Check file is not empty
    
	    fgets(wholeMLine, sizeof(wholeMLine), file); //Get whole first line
		sscanf(wholeMLine, "%i", &rows); //Get number of rows
	    

	    fgets(wholeNLine, sizeof(wholeNLine), file); //Get whole second line
		sscanf(wholeNLine, "%i", &columns); //Get number of columns

	    for(i = 0; i < rows*columns; i++) { // Read file until end 

		fgets(wholeCoordLine, sizeof(wholeCoordLine), file);
		sscanf(wholeCoordLine, "%i", &value);
		
		maze[x][y] = value; //Set maze position to appropriate value
		if(value == 2){
			nCoins++;
		}
			if((maze[x][y] != 0) && (maze[x][y] != 2) && (!flag)){

				tankX = 2*x + 8;
				tankZ = 2*y; 
				flag++;
				maze[x][y] = 1; 
			}
		x++;

		if(x == columns){
		    y++;
		    x = 0;
		}
	    } 
		
	    //Initialise OpenGL Shader
	    initShader();

	    //Load different OBJ models

	    cube.loadOBJ("../models/cube.obj");  

	    coin.loadOBJ("../models/coin.obj"); 

	    chassis.loadOBJ("../models/chassis.obj");

	    turret.loadOBJ("../models/turret.obj");

	    frontWheel.loadOBJ("../models/front_wheel.obj");

	    backWheel.loadOBJ("../models/back_wheel.obj");

	    ball.loadOBJ("../models/ball.obj");
	    
	    //Load different textures
	    
	    initTexture("../models/ceiling.bmp", texture_cube);

	    initTexture("../models/coin.bmp", texture_coin);

	    initTexture("../models/hamvee.bmp", texture_tank);

	    initTexture("../models/ball.bmp", texture_ball);

	    glutMainLoop(); //Enter main loop

	    glDeleteProgram(shaderProgramID); //Delete shader program

	    fclose(file); //Close file

    }

    return 0;
}

/* Initialise OpenGL */

bool initGL(int argc, char** argv)
{
    //Init GLUT
    glutInit(&argc, argv);

    
    //Set Display Mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);

    //Set Window Size
    glutInitWindowSize(screenWidth, screenHeight);
    
    // Window Position
    glutInitWindowPosition(200, 200);

    //Create Window
    glutCreateWindow("Tank Assignment");

    //Set colour of the scene to medium blue
    glClearColor(0.196078,0.196078,0.8,1.0);

    
    // Init GLEW
    if (glewInit() != GLEW_OK) 
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return false;
    }


    //Set texture function
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    //Set Display function
    glutDisplayFunc(display);
    
    //Set Keyboard Interaction Functions
    glutKeyboardFunc(keyboard);
    glutKeyboardUpFunc(keyUp); 

    //Set Mouse Interaction Functions
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    glutMotionFunc(motion);

    return true;
}

/* Initialise shader */

void initShader()
{




//Create shader
    shaderProgramID = Shader::LoadFromFile("shader.vert","shader.frag");
    
    // Get a handle for our vertex position buffer
	vertexPositionAttribute = glGetAttribLocation(shaderProgramID,  "aVertexPosition");
	vertexNormalAttribute = glGetAttribLocation(shaderProgramID,    "aVertexNormal");
	vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexcoord");

    //!
	MVMatrixUniformLocation         = glGetUniformLocation(shaderProgramID, "MVMatrix_uniform"); 
	ProjectionUniformLocation       = glGetUniformLocation(shaderProgramID, "ProjMatrix_uniform"); 
	LightPositionUniformLocation    = glGetUniformLocation(shaderProgramID, "LightPosition_uniform"); 
	AmbientUniformLocation          = glGetUniformLocation(shaderProgramID, "Ambient_uniform"); 
	SpecularUniformLocation         = glGetUniformLocation(shaderProgramID, "Specular_uniform"); 
	SpecularPowerUniformLocation    = glGetUniformLocation(shaderProgramID, "SpecularPower_uniform");
    TextureMapUniformLocation       = glGetUniformLocation(shaderProgramID, "TextureMap_uniform"); 

   parameterLocation = glGetUniformLocation(shaderProgramID, "brightness");
}


/*
    //Create shader
    shaderProgramID = Shader::LoadFromFile("shader.vert","shader.frag");
    
    vertexPositionAttribute = glGetAttribLocation(shaderProgramID, "aVertexPosition");

    MVMatrixUniformLocation = glGetUniformLocation(shaderProgramID,     "MVMatrix_uniform"); 

    ProjectionUniformLocation = glGetUniformLocation(shaderProgramID,   "ProjMatrix_uniform"); 

    vertexTexcoordAttribute = glGetAttribLocation(shaderProgramID, "aVertexTexcoord");
    
    TextureMapUniformLocation = glGetUniformLocation(shaderProgramID, "TextureMap_uniform");

    parameterLocation = glGetUniformLocation(shaderProgramID, "brightness"); */
    
//}

/* Function to initialise a texture. Takes the texture filename and texture ID as a parameter and returns nothing */

void initTexture(std::string filename, GLuint & textureID)
{

    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //Get texture data

    int width, height;
    char* data;
    Texture::LoadBMP(filename, width, height, data);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    //Cleanup data as copied to GPU
    delete[] data;
    
}


/* Display loop */

void display(void){

    int i,j; //To iterate through the maze

    //Handle keys
    handleKeys();

    //Set Viewport
    glViewport(0,0,screenWidth, screenHeight);
      
    // Clear the screen
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );


    //Use shader
    glUseProgram(shaderProgramID);


    glUniform3f(LightPositionUniformLocation, 				   lightPosition.x,lightPosition.y,lightPosition.z);
    glUniform4f(AmbientUniformLocation, ambient.x, ambient.y, ambient.z, 1.0);
    glUniform4f(SpecularUniformLocation, specular.x, specular.y, specular.z, 1.0);
    glUniform1f(SpecularPowerUniformLocation, specularPower);
	

    if(far){ //If the camera perspective is far
	cameraManip.setPanTiltRadius(angleTurret*3.14/180,-1.f,2.f);
	cameraManip.setFocus(Vector3f(tankX, tankY + 0.5, tankZ));

    } else if (!far){ //If the camera perspective is not far

	cameraManip.setPanTiltRadius((angleTurret*3.14/180),-3.f,-7.f);
	cameraManip.setFocus(Vector3f(tankX,tankY + 0.5,tankZ));
    }

    //Set Colour after program is in use
    glActiveTexture(GL_TEXTURE0);

    //Set brightness parameter in the shader
    glUniform1f(parameterLocation, 1.0);
	
    if(light){	
   	 glUniform1f(parameterLocation, 1.5); //Set scene to light blue
    } else if (medium){
	glUniform1f(parameterLocation, 1); //Set scene to medium blue
    } else if (dark){
	glUniform1f(parameterLocation, 0.5); //Set scene to dark purple
    } 

    glUniform1i(TextureMapUniformLocation, 0);

    //Set matrix for perspective Projection
    ProjectionMatrix.perspective(90, 1.0, 0.01, 100.0);

    //Set projection matrix
    glUniformMatrix4fv(    
        ProjectionUniformLocation,  //Uniform location
        1,                            //Number of Uniforms
        false,                        //Transpose Matrix
        ProjectionMatrix.getPtr());    //Pointer to ModelViewMatrixValues

	//Apply Camera Manipluator to Set Model View Matrix on GPU

	ModelViewMatrix.toIdentity();
	tempMat = cameraManip.apply(ModelViewMatrix);

    for(i = 0; i < rows; i++){ //Iterate through maze
		for(j = 0; j < columns; j++){


		if(maze[i][j] != 0) { //Maze space is not empty

			Matrix4x4 m = tempMat; //Model view matrix
			m.translate(i*2, 0, j*2); //Translate cube

			glBindTexture(GL_TEXTURE_2D, texture_cube); //Bind cube texture
			glUniformMatrix4fv(    
				 MVMatrixUniformLocation,      //Uniform location
				 1,                            //Number of Uniforms
				 false,                        //Transpose Matrix
					m.getPtr());            //Pointer to Matrix Values

			cube.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw cube

				if(maze[i][j] == 2){ //Maze space contains a coin

					m.translate(0, 1.5, 0); //Translate coin
					m.rotate(angleCoin,0,1,0);
					m.scale(0.3,0.3,0.3); //Scale coint

						glBindTexture(GL_TEXTURE_2D, texture_coin); //Bind coin texture
						glUniformMatrix4fv(    
							MVMatrixUniformLocation,      //Uniform location
							1,                            //Number of Uniforms
							false,                        //Transpose Matrix
								m.getPtr());            //Pointer to Matrix Values
            
					coin.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw coin
					if((!paused) && (!over)){
						angleCoin += 0.03; //Rotate coins continuously while the game is not paused
					}
				}    
			} 
		}
	}

	if (!paused) { //If the game is not paused

		updateTank(); //Update tank position

		//Forward direction
		if (v > vMax){
		v = vMax; //If tank velocity exceeds the limit set it to maximum value
		} else if (v > 0.002){ //Decrease tank velocity gradually
		v -= 0.001;
		} else if((v > 0) && (v <= 0.003)){
		v = 0; //Stop the tank when its velocity is minuscule
		} 

		//Backwards direction
		if (v < -vMax){ //If tank velocity exceeds the limit set it to maximum value
		v = -vMax; 
		} else if(v < -0.002){ //Decrease tank velocity gradually
		v+= 0.001;
		} else if ((v < 0) && (v >= -0.003)){
		v = 0; //Stop the tank when its velocity is minuscule
		} 

		if(falling){ //Tank falls
			tankFall();
		}

		if((firing) && (ballY >= 1)){ //Draw the ball at the appropriate position, fire it and check for collisions with coins

			drawBall();
			fireBall();
			ballCollision();	
		}
	}

    //Draw and check tank
    drawTank();
    checkTank();

    glUseProgram(0); //Unuse Shader
    glBindTexture(GL_TEXTURE_2D, 0); //Unbind previous texture

    std::stringstream s1; //Create score string
    s1 << score;
    scoreString = s1.str();

    if((!paused) && (!over)){ //If the game is not paused and the tank has not fallen

    timeMilli = glutGet(GLUT_ELAPSED_TIME) - timeDifference; //Milliseconds left
    timeSec = timeMilli / 1000; //convert to seconds

    } else if(paused){ //If the game is paused
	
	timeDifference = glutGet(GLUT_ELAPSED_TIME) - timeMilli; //Amount of time game has been paused

    }

    std::stringstream s2; //Create time string
    s2 << (maxSeconds - timeSec);
    timeString = s2.str();

    //Output remaining time to screen
    render2dText("Time:", 1.0, 1.0, 1.0, -0.95, 0.9); 
    render2dText(timeString, 1.0, 1.0, 1.0, -0.77, 0.9);

    //Output score to screen
    render2dText("Score:", 1.0, 1.0, 1.0, -0.95, 0.8);
    render2dText(scoreString, 1.0, 1.0, 1.0, -0.77, 0.8);

    //Warning when there are 3 seconds left
    if ((maxSeconds - timeSec) == 3){
    	render2dText("WARNING! 3 seconds left!", 1.0, 1.0, 1.0, -0.3, 0.8);
    }

    //Warning when there are 3 seconds left
    if ((maxSeconds - timeSec) == 2){
    	render2dText("WARNING! 2 seconds left!", 1.0, 1.0, 1.0, -0.3, 0.8);
    }

    //Warning when there are 3 seconds left
    if ((maxSeconds - timeSec) == 1){
    	render2dText("WARNING! 1 second left!", 1.0, 1.0, 1.0, -0.3, 0.8);
    }


    if((falling) || ((maxSeconds - timeSec) == 0)){ //Game ends if the tank falls or the time finishes
  
	 render2dText("GAME OVER", 1.0, 1.0, 1.0, -0.2, 0.8); //Output Game Over message to screen
	 render2dText("Press ESC to exit the game", 1.0, 1.0, 1.0, -0.3, 0.7);  //Instructions
	 over = 1; //Game over
	//goto a;
    }

    if(nCoins == score){

	 render2dText("Congrats! You collected all the coins!", 1.0, 1.0, 1.0, -0.4, 0.8); //Output winning message to screen
	render2dText("Press ESC to exit the game", 1.0, 1.0, 1.0, -0.3, 0.7); //Instructions
	over = 1;
    }

    //Swap buffers and post redisplay
    glutSwapBuffers();
    glutPostRedisplay();
}

/* Function to handle pressed keyboard keys. Takes no parameters and returns nothing */

void keyboard(unsigned char key, int x, int y)
{
    //Quits program when esc is pressed
    if (key == 27)    //esc key code
    {
        exit(0);
    } 

    //Set key status
    keyStates[key] = true;
}

/* Function to handle released keyboard keys. Takes no parameters and returns nothing */

void keyUp(unsigned char key, int x, int y)
{
    //Set key status
    keyStates[key] = false;
}

/* Function to handle the keyboard. Takes no parameters and returns nothing */

void handleKeys(){
    
	if((keyStates['w']) || (keyStates['W'])){ //If key w is pressed

		if((!stop) && (!paused)){ //If the tank has not fallen and the game is not paused
			v += 0.002; //Increase velocity
		}
	} 

	if((keyStates['s']) || (keyStates['S'])){ //If key s is pressed

		if((!stop) && (!paused)){ //If the tank has not fallen and the game is not paused
			v -= 0.002; //Reduce velocity
		}
	} 

	if((keyStates['d']) || (keyStates['D'])){ //If key d is pressed

		if((!stop) && (!paused)){ //If the tank has not fallen and the game is not paused
			angle -= 3.0; //Rotate the tank anticlockwise
		} 
	}

	if((keyStates['a']) || (keyStates['A'])){ //If key a is pressed

		if((!stop) && (!paused)){ //If the tank has not fallen and the game is not paused
			angle += 3.0; //Rotate the tank clockwise
		}
	} 

	if((keyStates['y']) || (keyStates['Y'])){ //If key y is pressed
			glClearColor(0.0,0.5,0.8,1.0); //Set background color to light blue
			light = 1; //Set light illumination for the shader
			medium = 0;
			dark = 0;

	} 

	if((keyStates['u']) || (keyStates['U'])){ //If key u is pressed
			glClearColor(0.196078,0.196078,0.8,1.0); //Set background color to medium blue
			light = 0; //Set medium illumination for the shader
			medium = 1;
			dark = 0;

	} 

	if((keyStates['i']) || (keyStates['I'])){ //If key i is pressed
			glClearColor(0.184314,0.184314,0.309804,1.0); //Set background color to dark blue
			light = 0; //Set dark illumination for the shader
			medium = 0;
			dark = 1;

	}  

	if((keyStates['p']) || (keyStates['P'])){ //If key p is pressed

		screenWidth = 1280; //Display game fullscreen
		screenHeight = 1024;
		glViewport(0,0,screenWidth, screenHeight);
		glutFullScreen(); 

	}

	if((keyStates['o']) || (keyStates['O'])){ //If key 'o' is pressed

		screenWidth = 720; //Exit full screen mode
		screenHeight = 720;
		glViewport(0,0,screenWidth, screenHeight);

		glutPositionWindow(200,200);
		glutReshapeWindow(720, 720);
	}

}

/* Function to control the mouse interaction. Takes the button, its state and the coordinates as parameters and returns nothing */

void mouse(int button, int state, int x, int y){

       if((button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) && (!over) && (!paused)){ //If the left button is pressed
	       setBall(); //Set and fire a ball
	       firing = 1; //Set ball firing parameter
	
	} else if((button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) && (!over) && (!paused)){ //if the right button is pressed

		far = !far; //Change camera perspective
       } 
}

/* Function to control the motion of the turret according to the mouse position on screen. Takes two coordinates as parameters and returns nothing */

void motion(int x, int y){

	glutSetCursor(GLUT_CURSOR_NONE); 
	cameraManip.handleMouseMotion(x,0);

	if((!paused) && (!over)){
		angleTurret = cameraManip.getPan()*180/3.14; //Determine turret angle according to camera pan
	}

}

/* Function to update the tank position. Takes no parameters and returns nothing */

void updateTank() {
 
	//Tank moves with an accelerated movement according to the keyboard keys pressed by the user
	vX = v*sin(angle*3.14/180);
	vZ = v*cos(angle*3.14/180);
	tankX = tankX + vX;
	tankZ = tankZ + vZ;

	if(v != 0){ //If the tank is moving
		if(v < 0){ //Rotate wheels backwards
			angleWheel -= 6;
		} else if(v > 0){ //Rotate wheels forwards
			angleWheel += 6; 
		}
	}   
}

/* Function to draw the tank. Takes no parameters and returns nothing */

void drawTank(){

	Matrix4x4 m = tempMat; //Model view matrix

	m.translate(tankX,tankY,tankZ); //Translate tank
	m.rotate(angle,0,1,0); //Rotate tank

	if((v < 0) && (path == 0)){ //Tank falling forwards
		path = 1;
	} else if((v > 0) && (path == 0)){ //Tank falling backwards
		path = 2;
	}

	if(path == 1){
	       m.rotate(-angleFall,1,0,0); //Tank rotation whilst falling forwards
	} else if(path == 2){
		m.rotate(angleFall,1,0,0); //Tank rotation whilst falling backwards
	}
		
	m.scale(0.12,0.12,0.12); //Scale tank

	glBindTexture(GL_TEXTURE_2D, texture_tank); //Bind tank texture

       glUniformMatrix4fv(    
           MVMatrixUniformLocation,      //Uniform location
           1,                            //Number of Uniforms
           false,                        //Transpose Matrix
               m.getPtr());            //Pointer to Matrix Values
   
   chassis.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw chassis

	m.translate(frontWheel.getMeshCentroid().x,frontWheel.getMeshCentroid().y,frontWheel.getMeshCentroid().z);

    m.rotate(angleWheel, 1,0,0); //Rotate front wheels

	m.translate(-frontWheel.getMeshCentroid().x,-frontWheel.getMeshCentroid().y,-frontWheel.getMeshCentroid().z);

           glUniformMatrix4fv(    
               MVMatrixUniformLocation,      //Uniform location
               1,                            //Number of Uniforms
               false,                        //Transpose Matrix
                   m.getPtr());            //Pointer to Matrix Values
   
    frontWheel.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw front wheels

	m.translate(frontWheel.getMeshCentroid().x,frontWheel.getMeshCentroid().y,frontWheel.getMeshCentroid().z);

	m.rotate(-angleWheel, 1,0,0); //Reset matrix after front wheels rotation

	m.translate(-frontWheel.getMeshCentroid().x,-frontWheel.getMeshCentroid().y,-frontWheel.getMeshCentroid().z);

	m.translate(backWheel.getMeshCentroid().x,backWheel.getMeshCentroid().y,backWheel.getMeshCentroid().z);

	m.rotate(angleWheel, 1,0,0); //Rotate back wheels

	m.translate(-backWheel.getMeshCentroid().x,-backWheel.getMeshCentroid().y,-backWheel.getMeshCentroid().z);

           glUniformMatrix4fv(    
               MVMatrixUniformLocation,      //Uniform location
               1,                            //Number of Uniforms
               false,                        //Transpose Matrix
                   m.getPtr());            //Pointer to Matrix Values
   
    backWheel.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw back wheels

	m.translate(backWheel.getMeshCentroid().x,backWheel.getMeshCentroid().y,backWheel.getMeshCentroid().z);

	m.rotate(-angleWheel, 1,0,0); //Reset matrix after back wheels rotation

	m.translate(-backWheel.getMeshCentroid().x,-backWheel.getMeshCentroid().y,-backWheel.getMeshCentroid().z);

    m.rotate(-angle,0,1,0); //Reset matrix after tank rotation

    m.rotate(angleTurret,0,1,0); //Rotate turret

       glUniformMatrix4fv(    
           MVMatrixUniformLocation,      //Uniform location
           1,                            //Number of Uniforms
           false,                        //Transpose Matrix
               m.getPtr());            //Pointer to Matrix Values
   
    turret.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw turret

   m.rotate(-angleTurret,0,1,0); //Reset matrix after turret rotation

}

/* Function to check tank position. Takes no parameters and returns nothing */

void checkTank(){
	
	int i,j; //To iterate through the maze

	for(i = 0; i < rows; i++){ //Iterate through the maze
		for(j = 0; j < columns; j++){

			if(maze[i][j] == 0){ //If there is an empty space and the tank is within its boundaries, the tank falls
				if((tankX + 0.25 >= 2*i-1) && (tankX - 0.25 <= (2*i+1)) && (tankZ + 0.25 >= 2*j-1) && (tankZ - 0.25 <= (2*j+1))){

					falling = 1;
			   }
			}

			if(maze[i][j] == 2){ //If there is a coin and the tank is within its boundaries, the coin is removed and the score is increased by one

				if(!stop){ //If tank has not fallen

					if((tankX + 0.25 >= 2*i-1) && (tankX - 0.25 <= (2*i+1)) && (tankZ + 0.25 >= 2*j-1) && (tankZ - 0.25 <= (2*j+1))){
						maze[i][j] = 1; //Draw cube without coin
						score++;
					}
				}
			}

			if((tankX + 0.25 <= -1) || (tankZ - 0.25 <= -1) || (tankX + 0.25 >= 2*columns - 1) || (tankZ + 0.25 >= 2*rows - 1)){ //If tank is outside the maze boundaries it falls

				falling = 1; //Tank falls
			}
		}
	}
}

/* Function to render text to screen. Takes the colour and coordinartes of the text as parameters and returns nothing */

void tankFall(){

	stop = 1; //Tank has started falling

        //Tank falls with accelerated movement governed by gravity
	vY += gravity * 0.05;
	tankY -= vY * 0.05;
	angleFall += 1; //Set angle for the fall

}

/* Function to draw a ball. Takes the colour and coordinartes of the text as parameters and returns nothing */

void drawBall(){

	Matrix4x4 m = tempMat; //Model view matrix
	m.translate(ballX,ballY,ballZ); //Translate ball
	m.scale(0.03,0.03,0.03); //Scale ball

	glBindTexture(GL_TEXTURE_2D, texture_ball); //Bind ball texture

		   glUniformMatrix4fv(    
			   MVMatrixUniformLocation,      //Uniform location
			   1,                            //Number of Uniforms
			   false,                        //Transpose Matrix
				   m.getPtr());            //Pointer to Matrix Values
   
	ball.Draw(vertexPositionAttribute, vertexNormalAttribute, vertexTexcoordAttribute); //Draw ball on screen
}

/* Function to fire the ball from the turret. Takes no parameters and returns nothing */

void fireBall(){

	//Ball moves with accelerated movement governed by initial force and gravity

	ballVX += 1.5*0.1;
	ballX += ballVX * 0.15 *cos(-(angleBall - 90)*3.14/180);

	ballVZ += 1.5*0.1;
	ballZ += ballVZ * 0.15 *sin(-(angleBall - 90)*3.14 / 180);

	ballVY += gravity * 0.01;
	ballY -= ballVY * 0.01;

	if(ballY <= 1){
		firing = 0;
	}
}

/* Function to set ball data. Takes no parameters and returns nothing */

void setBall(){

	//Set ball position with respect to tank turret

	angleBall = angleTurret;

	//Determine ball initial psoition

	ballX = tankX + 1.05*cos(-(angleBall - 90)*3.14/180); 
	ballY = tankY + 0.2;
	ballZ = tankZ + 1.05*sin(-(angleBall - 90)*3.14/180);

	ballVX = 0.0; //Reset parameters for next ball
	ballVY = 0.0;
    	ballVZ = 0.0;
}

/* Function to check ball collisions */

void ballCollision(){

	int i,j;

	for(i = 0; i < rows; i++){ //Iterate through the maze
		for(j = 0; j < columns; j++){
			//If the ball is within the boundaries of a cube with a coin remove coin and increment score by one
			if(maze[i][j] == 2){ 

				if(!stop){ 

					if((ballX >= 2*i-0.5) && (ballX <= (2*i+0.5)) && (ballZ >= 2*j-0.5) && (ballZ <= (2*j+0.5))){ 

						maze[i][j] = 1;
						score++;
  					 }
				}
			}
		}
	}
}

/* Timer Function. Takes the time value as a parameter and returns nothing */

void Timer(int value) {
	glutTimerFunc(10, Timer, 0);
}

/* Function to render text to screen. Takes the colour and coordinartes of the text as parameters and returns nothing */

void render2dText(std::string text, float r, float g, float b, float x, float y) {

	glColor3f(r, g, b); //Three colour components: red, green and blue, as detected by the human eye
	glRasterPos2f(x, y); //window coordinates
	for (unsigned int i = 0; i < text.size(); i++) //Render text
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]); //Set font
}

/* Function to create a menu. Takes the selected option number as a parameter and returns nothing */

void createMenu() {

	int submenu = glutCreateMenu(menu);

	if (!paused) { //If the game is not paused
		glutAddMenuEntry("Pause", 1); //Pause or continue game
	}
	else if (paused) { //ASK
		glutAddMenuEntry("Continue", 1);
	}
	glutAddMenuEntry("Exit", 0); //Second option: exit the game
	glutAttachMenu(GLUT_MIDDLE_BUTTON); //Link menu to mouse middle button

}


/* Function to set menu options. Takes the selected option number as a parameter and returns nothing */

void menu(int num) {

	switch (num) {

	case 1:  paused = !paused; break; //For first option pause the game
	case 0: exit(0); //For second option exit the game

	}
}

