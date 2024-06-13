#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string>
#include <sstream>
#include <ctime>
#include <gl/glut.h>

// Camera                 
float cameraAngleX = 0.0f;
float cameraAngleY = 0.0f;
float cameraDistance = 30.0f;

float cameraPosX = 0.0f;
float cameraPosY = 0.0f;
float cameraPosZ = 5.0f; // Initial distance from the origin

static GLfloat theta[] = { 0.0, 0.0, 0.0 };
static GLint axis = 2;
float lastx, lasty;
const float M_PI = 3.14159265358979323846;
float xpos = 0, ypos = 0, zpos = 0, xrot = 0, yrot = 0, angle = 0.0;



// Variables for the score and timer
int homeScore = 0;
int awayScore = 0;
time_t startTime;
GLuint texture[10];
GLfloat ang = 0.0, z = 1.0;



GLuint LoadTexture(const char* filename, int width, int height)
{
    GLuint texture;
    unsigned char* data;
    FILE* file;
    //The following code will read in our RAW file
    file = fopen(filename, "rb");
    if (file == NULL) return 0;
    data = (unsigned char*)malloc(width * height * 3);
    fread(data, width * height * 3, 1, file);
    fclose(file);
    glGenTextures(1, &texture); //generate the texture with the loaded data
    glBindTexture(GL_TEXTURE_2D, texture); //bind the textureto it’s array
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); //set texture environment parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //Generate the texture
    //Please be informed to switch from GL_RGB to GL_BGR_EXT for bitmap image
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_BGR_EXT,
        GL_UNSIGNED_BYTE, data);
    free(data); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture(GLuint texture)
{
    glDeleteTextures(1, &texture);
}
// Function to initialize the OpenGL environment
void init() {
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Set the background color to sky blue
    glEnable(GL_DEPTH_TEST);

}

// Function to display text on the screen
void renderBitmapString(float x, float y, void* font, const char* string) {
    const char* c;
    glRasterPos2f(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

// Function to draw a cuboid with specified dimensions and color
void drawCuboid(float x, float y, float z, float scaleX, float scaleY, float scaleZ, float r, float g, float b) {
    glColor3f(r, g, b);
    glPushMatrix();
    glTranslatef(x, y, z);
    glScalef(scaleX, scaleY, scaleZ);
    glutSolidCube(1.0f);
    glPopMatrix();

}
// Function to draw the soccer field with scoreboard
void drawSoccerField() {
    glPushMatrix();
    glTranslatef(9.0f, -4.0f, -10.0f);
    glScalef(1.5f, 2.0f, 1.5f); // Scale to make it tall

    // Draw the two poles as cubes
    glColor3f(0.6f, 0.3f, 0.0f); // Brown color
    // Pole 1
    glPushMatrix();
    glTranslatef(9.0f, 3.5f, -45.0f);
    glScalef(0.4f, 10.0f, 0.1f); // Scale to make it tall
    glutSolidCube(1.0f);
    glPopMatrix();
    // Pole 2
    glPushMatrix();
    glTranslatef(-0.9f, 3.5f, -45.0f);
    glScalef(0.4f, 10.0f, 0.1f); // Scale to make it tall
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the scoreboard as a cube
    glColor3f(0.0f, 0.0f, 0.0f); // Black color
    glPushMatrix();
    glTranslatef(4.0f, 4.5f, -45.0f); // Move the scoreboard above the field and slightly forward
    glScalef(9.55f, 5.2f, 0.1f); // Scale to make it wide and thin
    glutSolidCube(1.0f);
    glPopMatrix();


    glPopMatrix();
    // Draw the text for the score and timer
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    std::stringstream ss;
    ss << "Home: " << homeScore << " Away: " << awayScore;
    renderBitmapString(-0.6f, 0.8f, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());

    // Calculate elapsed time
    time_t now = time(0);
    int elapsedTime = difftime(now, startTime);
    int minutes = elapsedTime / 60;
    int seconds = elapsedTime % 60;
    ss.str("");
    ss << "Time: " << minutes << ":" << (seconds < 10 ? "0" : "") << seconds;
    renderBitmapString(-0.6f, 0.75f, GLUT_BITMAP_HELVETICA_18, ss.str().c_str());
}

void square(int a, int b, int c, int d) {
    GLfloat Cvertices[8][3] = {
        { -1.0f, -1.0f, -1.0f }, { 1.0f, -1.0f, -1.0f },
        { 1.0f,  1.0f, -1.0f }, { -1.0f,  1.0f, -1.0f },
        { -1.0f, -1.0f,  1.0f }, { 1.0f, -1.0f,  1.0f },
        { 1.0f,  1.0f,  1.0f }, { -1.0f,  1.0f,  1.0f }
    };

    glBegin(GL_POLYGON);
    glTexCoord2f(0.06f, 0.0f); glVertex3fv(Cvertices[a]);
    glTexCoord2f(1.08f, 0.0f); glVertex3fv(Cvertices[b]);
    glTexCoord2f(1.08f, 1.0f); glVertex3fv(Cvertices[c]);
    glTexCoord2f(0.06f, 1.0f); glVertex3fv(Cvertices[d]);
    glEnd();
}

void drawCubeWithTexture() {
    // Front face
    square(4, 5, 6, 7);
    // Back face
    square(1, 0, 3, 2);
    // Left face
    square(0, 4, 7, 3);
    // Right face
    square(5, 1, 2, 6);
    // Top face
    square(3, 7, 6, 2);
    // Bottom face
    square(0, 1, 5, 4);
}

void drawPlaneWithTexture() {
  
    // Top face
    square(3, 7, 6, 2);

   
}

// Function to draw the scoreboard
void drawScoreboard() {
    // Draw the two poles
    glColor3f(0.6f, 0.3f, 0.0f); // Brown color
    glBegin(GL_QUADS);
    glVertex2f(0.45f, -0.5f);
    glVertex2f(0.47f, 0.5f);
    glVertex2f(0.53f, 0.5f);
    glVertex2f(0.55f, -0.5f);
    glEnd();
    glBegin(GL_QUADS);
    glVertex2f(-0.55f, -0.5f);
    glVertex2f(-0.53f, 0.5f);
    glVertex2f(-0.47f, 0.5f);
    glVertex2f(-0.45f, -0.5f);
    glEnd();

    // Draw the scoreboard
    glColor3f(0.0f, 0.0f, 0.0f); // Black color
    glBegin(GL_QUADS);
    glVertex2f(-0.5f, 0.3f);
    glVertex2f(-0.5f, 0.5f);
    glVertex2f(0.5f, 0.5f);
    glVertex2f(0.5f, 0.3f);
    glEnd();

    // Draw the text for the score and timer
    glColor3f(1.0f, 1.0f, 1.0f); // White color

    glPushMatrix();
    glTranslatef(-14.0f, 0.35f, 0.0f); // Translate text position
    glRotatef(0.0f, 0.0f, 1.0f, 0.0f); // Rotate text

   
    glPopMatrix();
}

void drawFlagWithBase(float x, float z) {
    // Draw the base (tapak)
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color
    glPushMatrix();
    glTranslatef(x, -0.5f, z); // Position the base
    glScalef(1.5f, 0.1f, 1.5f); // Scale the base
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the pole
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color
    glPushMatrix();
    glTranslatef(x, 1.0f, z); // Position the pole
    glScalef(0.1f, 5.0f, 0.1f); // Scale the pole
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the flag (rectangle plane)
    glColor3f(1.0f, 0.0f, 0.0f); // Red color (you can change this)
    glPushMatrix();
    glTranslatef(x, 5.5f, z); // Position the flag
    glScalef(2.0f, 1.0f, 0.1f); // Scale the flag (adjust as needed)
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to draw the soccer field with flags, poles, and bases
void drawSoccerFieldWithFlagsAndBases() {
    // Draw the soccer field
    drawSoccerField();

    // Draw the flags with poles and bases
    drawFlagWithBase(20.0f, -45.0f); // Adjust the position as needed
    drawFlagWithBase(21.0f, -45.0f); // Adjust the position as needed
    drawFlagWithBase(22.0f, -45.0f); // Adjust the position as needed
    drawFlagWithBase(23.0f, -45.0f); // Adjust the position as needed
}

// Function to draw a simple building
void drawBuilding() {

    //right
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
    glPushMatrix();
    glTranslatef(-27.0f, 9.0f, -6.0f);
    glScalef(33.0f, 10.0f, 2.0f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //2 
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
    glPushMatrix();
    glTranslatef(-27.0f, -1.9f, -6.0f);
    glScalef(33.0f, 11.8f, 2.0f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //left
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
    glPushMatrix();
    glTranslatef(24.0f, 9.0f, -6.0f); //hujung nak jauh atau dekat dngn tangga
    glScalef(33.0f, 10.0f, 2.0f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //2
    glColor3f(0.8f, 0.8f, 0.8f); // Light gray color
    glPushMatrix();
    glTranslatef(24.0f, -1.9f, -6.0f); //hujung nak jauh atau dekat dngn tangga
    glScalef(33.0f, 11.8f, 2.0f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();


}

// Function to draw a fence
void drawFence(float startX, float startY, float startZ, float endX, float endY, float endZ) {
    glColor3f(0.8f, 0.8f, 0.8f); // Grey color for the fence

    // Calculate the number of posts needed
    int numPosts = 10; // Reduced number of posts for a shorter fence
    float postSpacingX = (endX - startX) / (numPosts - 1);
    float postSpacingZ = (endZ - startZ) / (numPosts - 1);

    for (int i = 0; i < numPosts; ++i) {
        float x = startX + i * postSpacingX;
        float z = startZ + i * postSpacingZ;

        // Draw fence post
        glPushMatrix();
        glTranslatef(x, startY, z); // Use startY for the vertical position
        glScalef(0.1f, 0.9f, 0.1f); // Increased scale for the fence post
        glutSolidCube(1.0f);
        glPopMatrix();
    }

    // Draw horizontal bars
    for (int i = 0; i < numPosts - 1; ++i) {
        float x = startX + i * postSpacingX;
        float z = startZ + i * postSpacingZ;
        float nextX = startX + (i + 1) * postSpacingX;
        float nextZ = startZ + (i + 1) * postSpacingZ;

        glPushMatrix();
        glTranslatef((x + nextX) / 2, startY + 0.25f, (z + nextZ) / 2);
        glScalef(postSpacingX, 0.1f, 0.1f); // Increased scale for the horizontal bars
        glutSolidCube(1.0f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef((x + nextX) / 2, startY - 0.25f, (z + nextZ) / 2);
        glScalef(postSpacingX, 0.1f, 0.1f); // Increased scale for the horizontal bars
        glutSolidCube(1.0f);
        glPopMatrix();
    }
}

// Function to draw a stadium light pole
void drawLightPole(float x, float z) {
    // Draw the pole
    glColor3f(0.5f, 0.5f, 0.5f); // Gray color
    glPushMatrix();
    glTranslatef(x, 7.0f, z); // Position the pole
    glScalef(0.6f, 30.0f, 0.2f); // Scale the pole
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the light fixture
    glColor3f(1.0f, 1.0f, 1.0f); // White color
    glPushMatrix();
    glTranslatef(x, 22.0f, z); // Position the light fixture
    glScalef(5.0f, 3.0f, 1.0f); // Scale the light fixture
    glutSolidCube(1.0f);
    glPopMatrix();
}

void BuildUnderSeat() {
    //left
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(24.0f, -4.8f, -13.5f); // Moved closer to the seats
    glScalef(33.0f, 6.0f, 15.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //right
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-27.0f, -4.8f, -13.5f); // Moved closer to the seats
    glScalef(33.0f, 6.0f, 15.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //middle
    //glColor3f(1.0f, 1.0f, 1.0f);
    //glPushMatrix();
    //glTranslatef(0.0f, 0.2f, -9.5f); // Moved closer to the seats
    //glScalef(8.0f, 5.0f, 4.7f); // Increased scale
    //glutSolidCube(1.0f);
    //glPopMatrix();

    //entrance
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-1.4f, -0.4f, -12.7f); // Moved closer to the seats
    glScalef(17.9f, 1.0f, 14.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance1
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-1.4f, 0.0f, -20.0f); // Moved closer to the seats
    glScalef(17.9f, 2.0f, 0.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //top entrance
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-1.4f, 4.2f, -10.7f); // Moved closer to the seats
    glScalef(17.9f, 1.0f, 9.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //Top entrance1
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glTranslatef(-1.4f, 5.0f, -15.0f); // Moved closer to the seats
    glScalef(17.9f, 2.0f, 0.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance wall
    glColor3f(0.65f, 0.16f, 0.16f);
    glPushMatrix();
    glTranslatef(-1.4f, 10.1f, -3.0f); // Moved closer to the seats
    glScalef(18.7f, 13.5f, 0.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //front entrance roof
    glColor3f(0.6f, 0.6f, 0.6f);
    glPushMatrix();
    glTranslatef(-1.3f, 9.6f, -5.7f); // Moved closer to the seats
    glScalef(18.4f, 1.0f, 9.7f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //top entrance 
    //glColor3f(1.0f, 0.0f, 1.0f);
    //glPushMatrix();
    //glTranslatef(0.0f, 3.0f, -8.7f); // Moved closer to the seats
    //glScalef(8.0f, 1.0f, 3.0f); // Increased scale
    //glutSolidCube(1.0f);
    //glPopMatrix();

    //signboard stadium
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[3]);
    glEnable(GL_TEXTURE_2D); // Bind the texture to its array
    glTranslatef(-1.5f, 10.0f, 19.5f);
    glRotatef(90, 0.0f, 0.0f, 1.0f);
    glRotatef(-360, 0.0f, 1.0f, 0.0f);
    glRotatef(90, 1.0f, 0.0f, 0.0f);
    glScalef(2.0f, 0.001f, 5.0f); // Scale the cube to be larger for signboard stadium
    drawPlaneWithTexture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

//roof top stadium (astaka)
void drawStadiumRooftop() {
    glPushMatrix(); // Save the current transformation matrix

    glRotatef(360.0f, 0.0f, 1.0f, 0.0f); // Rotate 90 degrees anticlockwise around the y-axis
    glTranslatef(0.0f, 2.0f, -12.5f); // Adjust the values as needed
    glScalef(1.6f, 1.6f, 1.6f); // Scale the roof to half size in all dimensions (x, y, z)

    // Set the color for the rooftop
    glColor3f(0.6f, 0.6f, 0.6f); // Light grey color

    glBegin(GL_QUADS);
    // Front face (lower part)
    glVertex3f(-16.0f, 2.0f, 5.0f);
    glVertex3f(16.0f, 2.0f, 5.0f);
    glVertex3f(16.0f, 4.0f, 0.0f); // higher z-coordinate
    glVertex3f(-16.0f, 4.0f, 0.0f); // higher z-coordinate
    glEnd();

    glBegin(GL_TRIANGLES);
    // Left triangle
    glVertex3f(-16.0f, 2.0f, 5.0f);
    glVertex3f(-16.0f, 4.0f, 0.0f); // higher z-coordinate
    glVertex3f(-16.0f, 2.0f, 0.0f);

    // Right triangle
    glVertex3f(16.0f, 2.0f, 5.0f);
    glVertex3f(16.0f, 4.0f, 0.0f); // higher z-coordinate
    glVertex3f(16.0f, 2.0f, 0.0f);
    glEnd();

    glPopMatrix(); // Restore the original transformation matrix

}

void drawParking() {
    //glColor3f(0.5f, 0.5f, 0.5f); // Gray color for the parking area
    //glPushMatrix();
    //glTranslatef(0.0f, -0.5f, 22.5f); // yuni tukar sini 
    //glScalef(100.0f, 0.2f, 70.0f); // Increased scale // yuni pun tukar sini
    //glutSolidCube(1.0f);
    //glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[4]);
    glEnable(GL_TEXTURE_2D); // Bind the texture to its array
    glTranslatef(5.0f, -0.5f ,40.5f);
    glScalef(800.0f, 0.001f, 50.0f); // Scale the cube to be larger for leaves
    drawCubeWithTexture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

}

//void drawGrass() {
//    glColor4f(0.0f, 0.5f, 0.0f, 1.0f);  // Grass green color with full opacity
//    glPushMatrix();
//    glTranslatef(0.0f, -7.0f, -50.0f); // yuni tukar sini 
//    glScalef(100.0f, 0.2f, 90.0f); // Increased scale // yuni pun tukar sini
//    glutSolidCube(1.0f);
//    glPopMatrix();
//
//
//}

void drawParkingLines(float startX, float startY, float startZ, int numSpaces) {
    float spaceWidth = 0.7f; // Width of each parking space
    float totalWidth = numSpaces * spaceWidth; // Total width of the row
    float gap = 2.0f; // Gap between parking spaces

    // Draw the parking spaces
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the lines

    // Draw the lines
    for (int i = 0; i < numSpaces; ++i) {
        glBegin(GL_LINE_LOOP);
        glVertex3f(startX + i * (spaceWidth + gap), startY, startZ);
        glVertex3f(startX + (i + 1) * (spaceWidth + gap), startY, startZ);
        glVertex3f(startX + (i + 1) * (spaceWidth + gap), startY, startZ + totalWidth);
        glVertex3f(startX + i * (spaceWidth + gap), startY, startZ + totalWidth);
        glEnd();
    }
}

// Function to draw a car
void drawCar(float x, float z) {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the car body
    glPushMatrix();
    glTranslatef(x, -0.4f, 1.5); // Adjusted y-position
    glScalef(1.2f, 0.6f, 2.4f); // Increased scale for the car body
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the car roof
    glColor3f(0.8f, 0.0f, 0.0f); // Darker red for the roof
    glPushMatrix();
    glTranslatef(x, -0.1f, 1.5); // Adjusted y-position for the roof
    glScalef(1.2f, 0.3f, 1.2f); // Increased scale for the roof
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawdivider(float x, float y, float z, float angle, float length) {
    glColor3f(0.0f, 0.0f, 0.0f); // Black color for the divider
    glPushMatrix();
    glTranslatef(x, y, z); // Adjusted position based on parameters
    glRotatef(angle, 0.0f, 1.0f, 0.0f); // Rotate based on angle parameter
    glScalef(0.5f, 0.2f, length); // Scale based on length parameter
    glutSolidCube(1.0f);
    glPopMatrix();
}

void drawlineRoad(float x, float y, float z, float angle, float length) {
    glColor3f(1.0f, 1.0f, 1.0f); // Black color for the divider
    glPushMatrix();
    glTranslatef(x, y, z); // Adjusted position based on parameters
    glRotatef(angle, 0.0f, 1.0f, 0.0f); // Rotate based on angle parameter
    glScalef(0.2f, 0.05f, length); // Scale based on length parameter
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to draw trees


void drawTree(float x, float z) {
    // Draw the trunk
    glPushMatrix();
    glColor3f(0.55f, 0.27f, 0.07f); // Brown color
    glTranslatef(x, 0.5f, z); // Adjusted y-position
    glScalef(0.2f, 3.0f, 0.2f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw the leaves as cubes with texture
    //glColor3f(1.0f, 1.0f, 1.0f); // White color to show the texture color

    // First cube of leaves
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glEnable(GL_TEXTURE_2D); // Bind the texture to its array
    glTranslatef(x, 2.0f, z);
    glScalef(0.8f, 0.8f, 0.8f); // Scale the cube to be larger for leaves
    drawCubeWithTexture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // Second cube of leaves slightly above the first
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glEnable(GL_TEXTURE_2D);
    glTranslatef(x, 3.0f, z);
    glScalef(1.0f, 1.0f, 1.0f); // Scale the cube to be slightly smaller for variety
    drawCubeWithTexture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    

}


void drawForestTree(float x, float z) {
    // Draw the trunk
    glColor3f(0.55f, 0.27f, 0.07f); // Brown color
    glPushMatrix();
    glTranslatef(x, 0.5f, z); // Adjusted y-position
    glScalef(0.2f, 3.0f, 0.2f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // Draw leaves along the branches
    glColor3f(0.0f, 0.5f, 0.0f); // Green color

    // Leaves on the right side
    glPushMatrix();
    glTranslatef(x + 0.2f, 1.8f, z); // Adjusted position for right side
    glRotatef(45.0f, 0.0f, 1.0f, 0.0f); // Rotate for branch orientation
    glScalef(0.5f, 1.0f, 0.5f); // Scale for leaf size
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();

    // Leaves on the left side
    glPushMatrix();
    glTranslatef(x - 0.2f, 1.8f, z); // Adjusted position for left side
    glRotatef(-45.0f, 0.0f, 1.0f, 0.0f); // Rotate for branch orientation
    glScalef(0.5f, 1.0f, 0.5f); // Scale for leaf size
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();

    // Draw leaves on the trunk
    glPushMatrix();
    glTranslatef(x, 1.0f, z); // Adjusted y-position
    glScalef(1.0f, 1.0f, 1.0f); // Scale for leaf size
    glutSolidSphere(0.5f, 20, 20);
    glPopMatrix();
}

// Function to draw a palm tree
void drawPalmTree(float x, float z) {
    // Draw the trunk
    glColor3f(0.55f, 0.27f, 0.07f); // Brown color
    glPushMatrix();
    glTranslatef(x, -0.9f, z);
    GLUquadric* quad = gluNewQuadric();
    glRotatef(-90, 1.0f, 0.0f, 0.0f); // Rotate to draw the cylinder vertically
    gluCylinder(quad, 0.2f, 0.2f, 3.0f, 10, 10); // Tall, slender trunk
    glPopMatrix();

    // Draw the leaves
    glColor3f(0.0f, 0.5f, 0.0f); // Green color
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(x, 2.1f, z); // Position at the top of the trunk
        glRotatef(i * 72, 0.0f, 1.0f, 0.0f); // Rotate around the trunk
        glRotatef(-45, 1.0f, 0.0f, 0.0f); // Tilt the leaves
        gluCylinder(quad, 0.1f, 0.0f, 2.0f, 10, 10); // Draw a leaf
        glPopMatrix();
    }
    glColor3f(0.0f, 0.5f, 0.0f); // Green color
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(x, 1.9f, z); // Position at the top of the trunk
        glRotatef(i * 72, 0.0f, 1.0f, 0.0f); // Rotate around the trunk
        glRotatef(-45, 1.0f, 0.0f, 0.0f); // Tilt the leaves
        gluCylinder(quad, 0.1f, 0.0f, 2.0f, 10, 10); // Draw a leaf
        glPopMatrix();
    }
    // Draw the leaves
    glColor3f(0.0f, 0.5f, 0.0f); // Green color
    for (int i = 0; i < 8; ++i) {
        glPushMatrix();
        glTranslatef(x, 2.0f, z); // Position at the top of the trunk
        glRotatef(i * 72, 0.0f, 1.0f, 0.0f); // Rotate around the trunk
        glRotatef(45, 1.0f, 0.0f, 0.0f); // Tilt the leaves
        gluCylinder(quad, 0.1f, 0.0f, 2.0f, 10, 10); // Draw a leaf
        glPopMatrix();
    }
    gluDeleteQuadric(quad);
}
//
//// Function to draw the running track
//void drawRunningTrack() {
//    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the track
//    glPushMatrix();
//    glTranslatef(0.0f, -6.8f, -60.0f);
//    glScalef(50.0f, 0.2f, 25.0f); // Increased scale
//    glutSolidCube(1.0f);
//    glPopMatrix();
//
//    //// Inner white lines of the running track
//    //glColor3f(1.0f, 1.0f, 1.0f);
//    //for (float i = -9.6f; i <= 9.6f; i += 1.4f) {
//    //    glPushMatrix();
//    //    glTranslatef(0.0f, -0.5f, -8.0f + i);
//    //    glScalef(19.8f, 0.02f, 0.1f); // Increased scale
//    //    glutSolidCube(1.0f);
//    //    glPopMatrix();
//    //}
//}
//
//// Function to draw the green field inside the running track
//void drawField() {
//    glColor3f(0.0f, 0.8f, 0.0f); // Green color for the field
//    glPushMatrix();
//    glTranslatef(0.0f, -6.7f, -60.0f); // Raise the field a bit
//    glScalef(35.0f, 0.2f, 15.0f); // Increased scale
//    glutSolidCube(1.0f);
//    glPopMatrix();
//}

// Function to draw the running track
void drawRunningTrack() {
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the track
    glPushMatrix();
    glTranslatef(0.0f, -4.80f, -50.0f);
    glScalef(50.0f, 0.2f, 25.0f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();

    //// Inner white lines of the running track
    //glColor3f(1.0f, 1.0f, 1.0f);
    //for (float i = -9.6f; i <= 9.6f; i += 1.4f) {
    //    glPushMatrix();
    //    glTranslatef(0.0f, -0.5f, -8.0f + i);
    //    glScalef(19.8f, 0.02f, 0.1f); // Increased scale
    //    glutSolidCube(1.0f);
    //    glPopMatrix();
    //}
}

// Function to draw the green field inside the running track
void drawField() {
    //glColor3f(0.0f, 0.8f, 0.0f); // Green color for the field
    //glPushMatrix();
    //glTranslatef(0.0f, -8.9f, -50.5f); // Raise the field a bit
    //glScalef(35.0f, 0.2f, 15.0f); // Increased scale
    //glutSolidCube(1.0f);
    //glPopMatrix();

    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[1]);
    glEnable(GL_TEXTURE_2D); // Bind the texture to its array
    glTranslatef(0.0f, -8.9f, -50.5f);
    glScalef(20.0f, 0.2f, 12.0f); // Scale the cube to be larger for leaves
    drawCubeWithTexture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void drawGrass() {
    glColor4f(0.0f, 0.5f, 0.0f, 1.0f);  // Grass green color with full opacity
    glPushMatrix();
    glTranslatef(0.0f, -9.9f, -50.0f); // yuni tukar sini 
    glScalef(100.0f, 0.2f, 70.0f); // Increased scale // yuni pun tukar sini
    glutSolidCube(1.0f);
    glPopMatrix();

}

void drawRunningTrack(float cx, float cy, float rx, float rz, int num_segments) {
    glPushMatrix(); // Save the current matrix state
    glColor3f(1.0f, 0.0f, 0.0f); // Red color for the track

    // Translate the track to the ground level
    glTranslatef(0.0f, -9.5f, -50.0f); // Move the track downwards by 3.5 units
    glRotated(90, 0.0, 1.0, 0.0);

    // Draw the oval-shaped running track on the ground
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, 0.0f); // Center of the oval
    for (int i = 0; i <= num_segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // Angle for each segment
        float x = rx * cosf(theta); // Calculate the x-coordinate
        float z = rz * sinf(theta); // Calculate the z-coordinate
        glVertex3f(x, 0.0f, z); // Output vertex
    }
    glEnd();

    // Draw the outline of the track
    glColor3f(0.5f, 0.5f, 0.5f); // Grey color for the track lines
    glLineWidth(3.0f); // Set the line width
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i <= num_segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments); // Angle for each segment
        float x = rx * cosf(theta); // Calculate the x-coordinate
        float z = rz * sinf(theta); // Calculate the z-coordinate
        glVertex3f(x, 0.0f, z); // Output vertex
    }
    glEnd();
    glPopMatrix(); // Restore the previous matrix state
}

void drawOvalTrack(float centerX, float centerY, float radiusX, float radiusY, int segments = 100) {
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float theta = 2.0f * 3.1415926f * float(i) / float(segments);
        float x = radiusX * cosf(theta);
        float y = radiusY * sinf(theta);
        glVertex3f(centerX + x, 0.01f, centerY + y);
    }
    glEnd();
}

// Function to draw multiple oval tracks
void drawMultipleTracks() {
    glPushMatrix(); // Save the current matrix state
    glColor3f(1.0f, 1.0f, 1.0f); // White color for the lane lines
    // Translate the track to the ground level

    glTranslatef(0.3, -9.0f, -51.0); // Move the track downwards by 3.5 units
    glRotated(90, 0.0, 1.0, 0.0);

    float centerX = 0.0f;
    float centerY = 0.0f;
    float initialRadiusX = 5.3f;
    float initialRadiusY = 16.0f;
    float trackWidth = 3.2f; // Width between each track

    for (int i = 0; i < 5; ++i) {
        float radiusX = initialRadiusX + i * trackWidth;
        float radiusY = initialRadiusY + i * trackWidth;
        drawOvalTrack(centerX, centerY, radiusX, radiusY);
    }
    glPopMatrix(); // Restore the previous matrix state

}

void drawSeating() {
    //right seat
    glColor3f(0.0f, 0.0f, 1.0f); // yellow color for the seats
    int numRows = 23; // Number of rows
    int numCols = 6; // Number of columns in each row
    float seatWidth = 1.5f; // Width of each seat
    float seatHeight = 0.5f; // Height difference between rows
    float seatDepth = 0.5f; // Depth of each seat
    float zOffset = -19.0f; // Starting Z position to move seats closer to the field
    float xOffset = -3.0f; // Distance from the building

    //right seat 2
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glPushMatrix();
            // Adjusted positions to create a grid
            glTranslatef(-27.0f + col * seatWidth + xOffset, -1.0f + row * seatHeight, zOffset + row * seatDepth);
            // No rotation needed for seats facing forward
            glScalef(1.0f, 0.5f, 1.0f); // Scale for better appearance
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    //right seat 1
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glPushMatrix();
            // Adjusted positions to create a grid
            glTranslatef(-16.0f + col * seatWidth + xOffset, -1.0f + row * seatHeight, zOffset + row * seatDepth);
            // No rotation needed for seats facing forward
            glScalef(1.0f, 0.5f, 1.0f); // Scale for better appearance
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    //right seat 3
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glPushMatrix();
            // Adjusted positions to create a grid
            glTranslatef(-38.0f + col * seatWidth + xOffset, -1.0f + row * seatHeight, zOffset + row * seatDepth);
            // No rotation needed for seats facing forward
            glScalef(1.0f, 0.5f, 1.0f); // Scale for better appearance
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    //left seat 1 
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glPushMatrix();
            // Adjusted positions to create a grid
            glTranslatef(12.0f + col * seatWidth + xOffset, -1.0f + row * seatHeight, zOffset + row * seatDepth);
            // No rotation needed for seats facing forward
            glScalef(1.0f, 0.5f, 1.0f); // Scale for better appearance
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    //left seat 2
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glPushMatrix();
            // Adjusted positions to create a grid
            glTranslatef(23.0f + col * seatWidth + xOffset, -1.0f + row * seatHeight, zOffset + row * seatDepth);
            // No rotation needed for seats facing forward
            glScalef(1.0f, 0.5f, 1.0f); // Scale for better appearance
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

    //left seat 3
    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glPushMatrix();
            // Adjusted positions to create a grid
            glTranslatef(34.0f + col * seatWidth + xOffset, -1.0f + row * seatHeight, zOffset + row * seatDepth);
            // No rotation needed for seats facing forward
            glScalef(1.0f, 0.5f, 1.0f); // Scale for better appearance
            glutSolidCube(1.0f);
            glPopMatrix();
        }
    }

}

// Define right-angled triangle vertices
GLfloat triangleVertices[][3] = {
    {0.0f, 0.0f, 0.0f},  // Bottom left (right angle)
    {0.0f, 0.0f, 33.0f},  // Bottom right
    {0.0f, 0.4f, 0.0f}   // Top left
};

// Define normal vector for the plane
GLfloat triangleNormal[] = { 0.0f, 0.0f, 1.0f };  // Normal pointing up

void drawTrianglePlane1(float x, float y, float z) {
    glPushMatrix();  // Save the current matrix
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // Rotate 90 degrees anticlockwise around the y-axis
    glTranslatef(-7.4, -1.8, 5.1);  // Apply translation
    glScalef(0.0f, 40.2f, 0.5f); // Increased scale


    glBegin(GL_TRIANGLES);
    glNormal3fv(triangleNormal);  // Set the normal for the plane
    glColor3f(1.0f, 1.0f, 1.0f); // Gray color

    for (int i = 0; i < 3; ++i) {
        glVertex3fv(triangleVertices[i]);
    }

    glEnd();

    glPopMatrix();  // Restore the previous matrix
}

void drawTrianglePlane2(float x, float y, float z) {
    glPushMatrix();  // Save the current matrix
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // Rotate 90 degrees anticlockwise around the y-axis
    glTranslatef(10.4, -1.8, 5.1);  // Apply translation
    glScalef(0.0f, 40.2f, 0.5f); // Increased scale


    glBegin(GL_TRIANGLES);
    glNormal3fv(triangleNormal);  // Set the normal for the plane
    glColor3f(1.0f, 1.0f, 1.0f); // Gray color

    for (int i = 0; i < 3; ++i) {
        glVertex3fv(triangleVertices[i]);
    }

    glEnd();

    glPopMatrix();  // Restore the previous matrix
}

void drawTrianglePlane3(float x, float y, float z) {
    glPushMatrix();  // Save the current matrix
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // Rotate 90 degrees anticlockwise around the y-axis
    glTranslatef(43.5, -1.8, 5.1);  // Apply translation
    glScalef(0.0f, 40.2f, 0.5f); // Increased scale


    glBegin(GL_TRIANGLES);
    glNormal3fv(triangleNormal);  // Set the normal for the plane
    glColor3f(1.0f, 1.0f, 1.0f); // Gray color

    for (int i = 0; i < 3; ++i) {
        glVertex3fv(triangleVertices[i]);
    }

    glEnd();

    glPopMatrix();  // Restore the previous matrix
}

void drawTrianglePlane4(float x, float y, float z) {
    glPushMatrix();  // Save the current matrix
    glRotatef(-180.0f, 0.0f, 1.0f, 0.0f); // Rotate 90 degrees anticlockwise around the y-axis
    glTranslatef(-40.1, -1.8, 5.1);  // Apply translation
    glScalef(0.0f, 40.2f, 0.5f); // Increased scale


    glBegin(GL_TRIANGLES);
    glNormal3fv(triangleNormal);  // Set the normal for the plane
    glColor3f(1.0f, 1.0f, 1.0f); // Gray color

    for (int i = 0; i < 3; ++i) {
        glVertex3fv(triangleVertices[i]);
    }

    glEnd();

    glPopMatrix();  // Restore the previous matrix
}

// Function to draw the stadium entrance
void drawEntrance() {
    glColor3f(0.6f, 0.6f, 0.6f); // Darker gray color
    glPushMatrix();
    glTranslatef(0.0f, -0.75f, -2.0f); // Moved closer to the seats
    glScalef(7.0f, 0.2f, 1.0f); // Increased scale
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Function to draw a pole
void drawPole(float x, float y, float z) {

    //right pole
    glPushMatrix();
    glTranslatef(7.5f, 4.5f, -2.5f);
    glScalef(0.9f, 11.0f, 3.9f); // Adjust the scale for the pole
    glColor3f(0.6f, 0.6f, 0.6f); //Dark Grey color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //left pole
    glPushMatrix();
    glTranslatef(-10.0f, 4.5f, -2.5f);
    glScalef(0.9f, 11.0f, 3.9f); // Adjust the scale for the pole
    glColor3f(0.6f, 0.6f, 0.6f); //Dark Grey color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //tiang depan dewan
    //left1
    glPushMatrix();
    glTranslatef(-10.0f, 4.5f, 9.5f);
    glScalef(1.3f, 11.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //Dark Grey color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //left2
    glPushMatrix();
    glTranslatef(-14.0f, 4.5f, 9.5f);
    glScalef(1.3f, 11.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //Dark Grey color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //right1
    glPushMatrix();
    glTranslatef(8.3f, 4.5f, 9.5f);
    glScalef(1.3f, 11.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //Dark Grey color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //right2
    glPushMatrix();
    glTranslatef(12.3f, 4.5f, 9.5f);
    glScalef(1.3f, 11.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //Dark Grey color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //tiang atas 

    //entrance pole long1 right
    glPushMatrix();
    glTranslatef(10.5f, 9.5f, -2.5f);
    glScalef(1.3f, 20.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //orange color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance pole long2 right
    glPushMatrix();
    glTranslatef(13.7f, 9.5f, -2.5f);
    glScalef(1.3f, 20.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //orange color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance pole long3 right
    glPushMatrix();
    glTranslatef(17.0f, 9.5f, -2.5f);
    glScalef(1.3f, 20.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //orange color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance pole long1 left
    glPushMatrix();
    glTranslatef(-13.0f, 9.5f, -2.5f);
    glScalef(1.3f, 20.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //orange color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance pole long2 left
    glPushMatrix();
    glTranslatef(-16.0f, 9.5f, -2.5f);
    glScalef(1.3f, 20.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //orange color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();

    //entrance pole long3 left
    glPushMatrix();
    glTranslatef(-20.0f, 9.5f, -2.5f);
    glScalef(1.3f, 20.0f, 1.3f); // Adjust the scale for the pole
    glColor3f(1.0f, 0.5f, 0.0f); //orange color for the poles
    glutSolidCube(1.0f);
    glPopMatrix();
}

// Define plane vertices (wider rectangle)
GLfloat plateStadiumRoofVertices[][3] = {
    {-25.0f, -0.15f, 0.0f},  // Bottom left (extended width)
    {25.0f, -0.15f, 0.0f},   // Bottom right (extended width)
    {25.0f, -0.15f, 20.0f},    // Top right
    {-25.0f, -0.15f, 20.0f}    // Top left
};

// Define normal vector for the plane
GLfloat planeNormal[] = { 0.0f, 0.0f, 1.0f };  // Normal pointing up

void drawPlateStadiumRoof(float x, float y, float z) {

    glPushMatrix();  // Save the current matrix
    glTranslatef(-1.5, 17.2, -20.45);  // Apply translation

    glBegin(GL_QUADS);
    glNormal3fv(planeNormal);  // Set the normal for the plane
    glColor3f(0.8f, 0.8f, 0.8f); // Gray color

    for (int i = 0; i < 4; ++i) {
        glVertex3fv(plateStadiumRoofVertices[i]);
    }
    glEnd();

    glPopMatrix();  // Restore the previous matrix
}

//entrance stadium roof

GLfloat plateEntranceStadiumVertices[][3] = {
    {-15.0f, -0.15f, 0.0f},  // Bottom left (extended width)
    {15.0f, -0.15f, 0.0f},   // Bottom right (extended width)
    {15.0f, -0.15f, 20.0f},    // Top right
    {-15.0f, -0.15f, 20.0f}    // Top left
};

void zombie(GLuint texture[], GLfloat angle)
{
    GLfloat y = 1.0;
    glPushMatrix();
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture[4]);

    //head
    glPushMatrix();
    glTranslatef(0.0f, 4.0f, 0.0f);
    glScalef(2.0f, 2.0f, 2.1f);
    glColor3f(0.14f, 0.43f, 0.14f);
    drawCubeWithTexture();
    glPopMatrix();

    //body
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.0f);
    glScalef(1.0f, 4.0f, 2.0f);
    glColor3f(0.21f, 0.58f, 0.60f);
    drawCubeWithTexture();
    glPopMatrix();

    for (int x = 0; x < 2; x++)
    {
        if (x == 1)
        {
            glScalef(-1.0f, 1.0f, -1.0f);
            y = -y;
        }

        //arm
        glPushMatrix();
        glColor3f(0.14f, 0.43f, 0.14f);
        glTranslatef(0.0f, -1.0f, 3.0f);
        glPushMatrix();
        glTranslatef(0.0f, 3.0f, 0.0f);
        glRotatef(angle, 0, 0, 1);
        glTranslatef(0.0f, -3.0f, 0.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        drawCubeWithTexture();
        glPopMatrix();
        glPopMatrix();

        //leg
        glPushMatrix();
        glColor3f(0.21f, 0.22f, 0.57f);
        glTranslatef(0.0f, -7.0f, 1.0f);
        glPushMatrix();
        glTranslatef(0.0f, 3.0f, 0.0f);
        glRotatef(angle, 0, 0, 1);
        glTranslatef(0.0f, -3.0f, 0.0f);
        glScalef(1.0f, 3.0f, 1.0f);
        drawCubeWithTexture();
        glPopMatrix();
        glPopMatrix();

        glPushMatrix();
        glColor3f(0.0f, 0.0f, 0.0f);
        glTranslatef(2.0f * y, 4.4f, -1.0f);
        glScalef(0.1f, 0.3f, 0.5f);
        drawCubeWithTexture();
        glPopMatrix();
    }
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}


void drawPlateEntranceStadium(float x, float y, float z) {

    glPushMatrix();  // Save the current matrix
    glTranslatef(-0.8, 9.9, -0.5);  // Apply translation

    glBegin(GL_QUADS);
    glNormal3fv(planeNormal);  // Set the normal for the plane
    glColor3f(0.8f, 0.8f, 0.8f); // Gray color

    for (int i = 0; i < 4; ++i) {
        glVertex3fv(plateEntranceStadiumVertices[i]);
    }
    glEnd();

    glPopMatrix();  // Restore the previous matrix
}

void setCamera() {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(cameraPosX, cameraPosY, cameraPosZ,
        cameraPosX + sin(cameraAngleY), cameraPosY + sin(cameraAngleX), cameraPosZ - cos(cameraAngleY),
        0.0f, 1.0f, 0.0f);
}

void drawBush(float x, float z) {
    // Set color for the bush (green)
    glColor3f(0.0f, 0.1f, 0.0f);

    // Draw the main body of the bush using spheres
    glPushMatrix();
    glTranslatef(x, 0.0f, z); // Adjusted y-position
    glutSolidSphere(0.5f, 20, 20); // Center sphere
    glTranslatef(0.2f, 0.0f, 0.0f);
    glutSolidSphere(0.4f, 20, 20); // Small sphere to the right
    glTranslatef(-0.4f, 0.0f, 0.0f);
    glutSolidSphere(0.4f, 20, 20); // Small sphere to the left
    glTranslatef(0.2f, 0.2f, 0.0f);
    glutSolidSphere(0.4f, 20, 20); // Small sphere to the top
    glTranslatef(0.0f, -0.4f, 0.0f);
    glutSolidSphere(0.4f, 20, 20); // Small sphere to the bottom
    glPopMatrix();
}

void drawSportComplex() {

    glPushMatrix();
    glTranslatef(55.0f, 0.0f, 20.0f);
    glScalef(0.6f, 0.6f, 0.6f);

    // main building
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(10.0f, 6.0f, 0.0f); // Adjusted coordinates
    glScalef(50.0f, 12.0f, 45.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // main roof
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(10.0f, 12.5f, 0.0); // Adjusted coordinates
    glScalef(50.0f, 0.8f, 45.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // top main
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(10.0f, 15.0f, 0.0f); // Adjusted coordinates
    glScalef(45.0f, 9.0f, 30.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // top main roof
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(10.0f, 19.5f, 0.0f); // Adjusted coordinates
    glScalef(48.0f, 0.6f, 35.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // yang depan entrance
    // main building
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-25.0f, 9.3f, 0.0f); // Adjusted coordinates
    glScalef(15.0f, 18.0f, 12.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right top entrace
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(-25.0f, 8.0f, 12.0f); // Adjusted coordinates
    glScalef(15.0f, 15.0f, 12.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right top entrace roof
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(-25.0f, 16.0f, 12.0f); // Adjusted coordinates
    glScalef(15.0f, 0.8f, 12.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right beside entrace wall
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-34.5f, 4.2f, 9.0f); // Adjusted coordinates
    glScalef(17.0f, 8.0f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left top entrace
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(-25.0f, 8.0f, -12.0f); // Adjusted coordinates
    glScalef(15.0f, 14.0f, 12.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left top entrace roof
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(-25.0f, 15.5f, -12.0f); // Adjusted coordinates
    glScalef(15.0f, 0.8f, 12.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left beside entrace wall
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-34.5f, 4.2f, -9.0f); // Adjusted coordinates
    glScalef(17.0f, 8.0f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right beside entrace
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(-34.5f, 5.0f, 18.3f); // Adjusted coordinates
    glScalef(19.0f, 10.0f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right beside entrace roof
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(-34.5f, 10.5f, 18.3f); // Adjusted coordinates
    glScalef(19.0f, 0.8f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // side wall right
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-40.5f, 3.5f, 27.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-34.5f, 3.5f, 27.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-28.5f, 3.5f, 27.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-14.0f, 3.5f, 22.8f); // Adjusted coordinates
    glScalef(18.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(-0.0f, 6.0f, 22.8f); // Adjusted coordinates
    glScalef(9.5f, 12.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(14.0f, 3.5f, 22.8f); // Adjusted coordinates
    glScalef(18.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(34.0f, 3.65f, 26.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(28.5f, 3.65f, 26.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // side wall left
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-40.5f, 3.5f, -27.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-34.5f, 3.5f, -27.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-28.5f, 3.5f, -27.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-14.0f, 3.5f, -22.8f); // Adjusted coordinates
    glScalef(18.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(-0.0f, 6.0f, -22.8f); // Adjusted coordinates
    glScalef(9.5f, 12.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(14.0f, 3.5f, -22.8f); // Adjusted coordinates
    glScalef(18.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(34.0f, 3.65f, -26.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(28.5f, 3.65f, -26.3f); // Adjusted coordinates
    glScalef(5.0f, 7.0f, 0.3f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // main back wall
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(25.0f, 4.3f, 0.0f); // Adjusted coordinates
    glScalef(0.7f, 8.0f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(31.5f, 3.7f, 8.0f); // Adjusted coordinates
    glScalef(10.0f, 7.0f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(31.5f, 3.7f, -8.0f); // Adjusted coordinates
    glScalef(10.0f, 7.0f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // back wall
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-38.2f, 3.65f, 12.0f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 7.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-38.2f, 3.65f, 20.3f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 7.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(38.0f, 3.65f, -12.5f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 8.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(38.0f, 3.65f, -21.5f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 8.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(38.0f, 3.65f, 12.5f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 8.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(38.0f, 3.65f, 21.5f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 8.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // front wall
    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, -14.0f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 6.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, -23.0f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 6.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, 14.0f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 6.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(1.0f, 0.73f, 0.0f, 1.0f);//yellow
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, 23.0f); // Adjusted coordinates
    glScalef(0.5f, 7.0f, 6.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // windows front
    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, -12.6f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, -15.45f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, -21.6f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, -24.45f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, 12.6f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, 15.45f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, 21.6f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-44.0f, 3.5f, 24.45f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-32.5f, 13.5f, 10.6f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-32.5f, 13.5f, 14.45f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-32.5f, 13.0f, -10.6f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-32.5f, 13.0f, -14.45f); // Adjusted coordinates
    glScalef(0.3f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // windows right
    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-40.5f, 2.8f, 27.45f); // Adjusted coordinates
    glScalef(2.7f, 5.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-28.5f, 2.8f, 27.45f); // Adjusted coordinates
    glScalef(2.7f, 5.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(-18.2f, 2.8f, 23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(-9.5f, 2.8f, 23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(18.2f, 2.8f, 23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(9.5f, 2.8f, 23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(33.8f, 3.8f, 26.45f); // Adjusted coordinates
    glScalef(3.0f, 2.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(28.3f, 3.8f, 26.45f); // Adjusted coordinates
    glScalef(3.0f, 2.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // windows left
    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-40.5f, 2.8f, -27.45f); // Adjusted coordinates
    glScalef(2.7f, 5.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-28.5f, 2.8f, -27.45f); // Adjusted coordinates
    glScalef(2.7f, 5.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(-18.2f, 2.8f, -23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(-9.5f, 2.8f, -23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(18.2f, 2.8f, -23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(9.5f, 2.8f, -23.3f); // Adjusted coordinates
    glScalef(8.0f, 5.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(33.8f, 3.8f, -26.45f); // Adjusted coordinates
    glScalef(3.0f, 2.5f, 0.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(28.3f, 3.8f, -26.45f); // Adjusted coordinates
    glScalef(3.0f, 2.5f, 0.5); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // windows back
    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, -10.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, -13.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, -19.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, -22.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, 10.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, 13.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, 19.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(38.0f, 3.5f, 22.55f); // Adjusted coordinates
    glScalef(0.7f, 3.5f, 2.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left beside entrace
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(-34.5f, 5.0f, -18.3f); // Adjusted coordinates
    glScalef(19.0f, 10.0f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left beside entrace roof
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(-34.5f, 10.5f, -18.3f); // Adjusted coordinates
    glScalef(19.0f, 0.8f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right behind
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(31.3f, 4.3f, 17.0f); // Adjusted coordinates
    glScalef(13.0f, 8.0f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // right behind
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(31.3f, 8.8f, 17.0f); // Adjusted coordinates
    glScalef(13.0f, 0.8f, 19.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left behind
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(31.3f, 4.3f, -17.0f); // Adjusted coordinates
    glScalef(13.0f, 8.0f, 18.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // left behind
    glColor3f(0.0f, 0.0f, 1.0f);//Blue
    glPushMatrix();
    glTranslatef(31.3f, 8.8f, -17.0f); // Adjusted coordinates
    glScalef(13.0f, 0.8f, 19.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // entrance roof
    glColor4f(0.62f, 0.47f, 0.36f, 1.0f);//brown
    glPushMatrix();
    glTranslatef(-33.0f, 10.5f, 0.0f); // Adjusted coordinates
    glScalef(15.0f, 0.3f, 10.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // signboard
    //glColor4f(1.0f, 1.0f, 1.0f, 0.0f);//white
    //glPushMatrix();
    //glTranslatef(-32.5f, 16.0f, 0.1f); // Adjusted coordinates
    //glScalef(0.2f, 3.5f, 10.0f); // Doubled scale
    //glutSolidCube(1.0f);
    //glPopMatrix();
     // signboard
    glPushMatrix();
    glBindTexture(GL_TEXTURE_2D, texture[2]);
    glEnable(GL_TEXTURE_2D); // Bind the texture to its array
   
    glTranslatef(-35.5f, 16.0f, 0.1f);
    glScalef(0.2f, 3.5f, 10.0f); // Scale the cube to be larger for leaves
    drawCubeWithTexture();
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();

    // entrance wall
    glColor3f(0.8f, 0.8f, 0.8f);
    glPushMatrix();
    glTranslatef(-32.5f, 5.1f, 0.1f); // Adjusted coordinates
    glScalef(0.2f, 9.5f, 10.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // entrance top windows
    glColor3f(0.0f, 0.0f, 0.0f);//Black
    glPushMatrix();
    glTranslatef(-33.5f, 8.5f, 0.1f); // Adjusted coordinates
    glScalef(0.2f, 1.0f, 8.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // entrance floor
    glColor4f(0.94f, 0.85f, 0.76f, 1.0f);//brown
    glPushMatrix();
    glTranslatef(-33.0f, 0.2f, 0.0f); // Adjusted coordinates
    glScalef(24.0f, 0.3f, 19.0f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    // entrance door
    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(-32.5f, 3.5f, 1.8f); // Adjusted coordinates
    glScalef(0.4f, 5.5f, 3.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glColor4f(0.85f, 0.94f, 1.0f, 1.0f);//baby Blue
    glPushMatrix();
    glTranslatef(-32.5f, 3.5f, -1.8f); // Adjusted coordinates
    glScalef(0.4f, 5.5f, 3.5f); // Doubled scale
    glutSolidCube(1.0f);
    glPopMatrix();

    glPopMatrix();

}

// Function to draw the canopy walkway
void drawCanopyWalkway() {

    glPushMatrix();
    glTranslatef(65.0f, 0.0f, 22.0f);
    glScalef(0.6f, 0.6f, 0.6f);

    // Base walkways
    drawCuboid(-38.0f, 0.0f, -31.0f, 23.0f, 0.1f, 6.0f, 1.0f, 1.0f, 1.0f); // White
    drawCuboid(-47.5f, 0.0f, -33.0f, 6.0f, 0.1f, 10.0f, 1.0f, 1.0f, 1.0f); // White
    drawCuboid(-56.0f, 0.0f, -40.0f, 23.0f, 0.1f, 6.0f, 1.0f, 1.0f, 1.0f); // White

    // Roofs
    drawCuboid(-38.0f, 10.0f, -31.0f, 23.0f, 0.5f, 6.0f, 0.0f, 0.0f, 1.0f); // Blue
    drawCuboid(-47.5f, 10.0f, -33.0f, 6.0f, 0.5f, 10.0f, 0.0f, 0.0f, 1.0f); // Blue
    drawCuboid(-56.0f, 10.0f, -40.0f, 23.0f, 0.5f, 6.0f, 0.0f, 0.0f, 1.0f); // Blue
    drawCuboid(-38.0f, 10.2f, -31.0f, 25.0f, 0.5f, 8.0f, 0.8f, 0.8f, 0.8f); // Gray
    drawCuboid(-47.5f, 10.2f, -33.0f, 8.0f, 0.5f, 12.0f, 0.8f, 0.8f, 0.8f); // Gray
    drawCuboid(-56.0f, 10.2f, -40.0f, 25.0f, 0.5f, 8.0f, 0.8f, 0.8f, 0.8f); // Gray

    // Columns (poles) between base walkway and roof
    float columnHeight = 10.0f;
    float columnRadius = 0.2f;
    float positions[10][3] = {
        {-38.0f, columnHeight / 2, -28.0f},
        {-38.0f, columnHeight / 2, -34.0f},
        {-47.5f, columnHeight / 2, -30.0f},
        {-47.5f, columnHeight / 2, -36.0f},
        {-56.0f, columnHeight / 2, -37.0f},
        {-56.0f, columnHeight / 2, -43.0f},
        {-38.0f, columnHeight / 2, -28.0f}, // Repeat for 10 pairs
        {-38.0f, columnHeight / 2, -34.0f},
        {-47.5f, columnHeight / 2, -30.0f},
        {-47.5f, columnHeight / 2, -36.0f}
    };

    for (int i = 0; i < 10; ++i) {
        drawCuboid(positions[i][0], positions[i][1], positions[i][2], columnRadius, columnHeight, columnRadius, 0.55f, 0.27f, 0.07f); // Brown
    }

    glPopMatrix();
}
void enable(void) {
    glEnable(GL_DEPTH_TEST); //enable the depth testing
    //glEnable(GL_LIGHTING); //enable the lighting
    //glEnable(GL_LIGHT0); //enable LIGHT0, our Diffuse Light
    //glShadeModel(GL_SMOOTH); //set the shader to smooth shader
}

void timer(int value) {
    glutPostRedisplay();
    glutTimerFunc(1000, timer, 0);
}

void camera(void) {
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // Calculate the position of the camera to make it revolve around the cube
    float camX = xpos + 15 * sin(yrot * 3.142 / 180);
    float camY = ypos - 15 * sin(xrot * 3.142 / 180);
    float camZ = zpos - 15 * cos(yrot * 3.142 / 180);

    gluLookAt(camX, camY, camZ, xpos, ypos, zpos, 0.0, 1.0, 0.0);
}

// Function to draw the scene
void display() {
    angle = 0.0f;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float camX = cameraDistance * sinf(cameraAngleY) * cosf(cameraAngleX);
    float camY = cameraDistance * sinf(cameraAngleX);
    float camZ = cameraDistance * cosf(cameraAngleY) * cosf(cameraAngleX);

    gluLookAt(camX, camY, camZ, // Camera position
        0.0f, 0.0f, 0.0f, // Look at point
        0.0f, 1.0f, 0.0f); // Up vector

    camera();
    enable();

    drawBuilding();
    drawScoreboard();
    drawSoccerFieldWithFlagsAndBases();
    drawSoccerField();
    BuildUnderSeat();
    drawParking();
    /*drawRunningTrack();
    drawField();
    drawGrass()*/;
    drawField();
    drawGrass();
    drawRunningTrack(-5.0f, 0.0f, 20.0f, 30.0f, 100);
    drawMultipleTracks();
    drawSeating();



    drawFence(-10.0f, 1.5f, -20.0f, 7.0f, 1.5f, -20.0f);// Example call to draw a fence in front of the parking area
    drawFence(-10.0f, 6.5f, -15.0f, 7.0f, 6.5f, -15.0f); // Example call to draw a fence behind the parking area


    drawTrianglePlane1(0.0f, 0.0f, -1.0f);  // Translate the triangle plane
    drawTrianglePlane2(0.0f, 0.0f, -1.0f);
    drawTrianglePlane3(0.0f, 0.0f, -1.0f);
    drawTrianglePlane4(0.0f, 0.0f, -1.0f);


    //pole for entrance front
    drawPole(10.0f, -3.2f, 20.0f);
    drawPlateStadiumRoof(0.0f, 0.0f, -1.0f);  // Move the plane 1 unit down in the z-direction
    drawPlateEntranceStadium(0.0f, 0.0f, -1.0f);




    // Draw the stadium light poles
    drawLightPole(45.0f, -35.0f);//first
    drawLightPole(-45.0f, -35.0f);//sec
    drawLightPole(45.0f, -78.0f);
    drawLightPole(-45.0f, -78.0f);



    drawCar(5.0f, 3.0f);
    drawCar(7.0f, 3.0f);

    drawdivider(-12.0f, -0.4f, 8.0f, -90.0f, 21.4f);
    drawdivider(-1.2f, -0.4f, 19.0f, -180.0f, 8.0f);
    drawdivider(-1.2f, -0.4f, 25.0f, -180.0f, 6.0f);
    drawdivider(-23.0f, -0.4f, 21.0f, -180.0f, 11.0f);
    drawdivider(-12.0f, -0.4f, 21.0f, -90.0f, 21.4f);
    drawdivider(-20.0f, -0.4f, 37.0f, -90.0f, 40.4f);
    // drawdivider(4.0f, -0.4f, 25.0f, -180.0f, 15.0f);
    drawdivider(5.0f, -0.4f, 47.0f, -90.0f, 110.0f);
    drawdivider(10.0f, -0.4f, 25.0f, -180.0f, 30.0f);
    drawdivider(20.0f, -0.4f, 5.0f, -180.0f, 15.0f);
    drawdivider(20.0f, -0.4f, 30.0f, -180.0f, 17.0f);
    drawdivider(-40.0f, -0.4f, 13.0f, -180.0f, 30.0f);
    // Draw some trees around at first row
    drawTree(-1.2f, 19.0f);
    drawTree(-1.2f, 23.0f);
    drawTree(-1.2f, 27.0f);


    drawTree(-28.0f, 48.0f);
    drawTree(-30.0f, 48.0f);
    drawTree(-32.0f, 48.0f);
    drawTree(-34.0f, 48.0f);
    drawTree(-36.0f, 48.0f);
    drawTree(-38.0f, 48.0f);
    drawTree(-40.0f, 48.0f);

    drawTree(-12.0f, 48.0f);
    drawTree(-14.0f, 48.0f);
    drawTree(-16.0f, 48.0f);
    drawTree(-18.0f, 48.0f);
    drawTree(-20.0f, 48.0f);
    drawTree(-22.0f, 48.0f);
    drawTree(-24.0f, 48.0f);

    drawTree(-6.0f, 48.0f);
    drawTree(-4.0f, 48.0f);
    drawTree(-2.0f, 48.0f);
    drawTree(0.0f, 48.0f);
    drawTree(2.0f, 48.0f);
    drawTree(4.0f, 48.0f);
    drawTree(6.0f, 48.0f);

    drawTree(12.0f, 48.0f);
    drawTree(14.0f, 48.0f);
    drawTree(16.0f, 48.0f);
    drawTree(18.0f, 48.0f);
    drawTree(20.0f, 48.0f);
    drawTree(22.0f, 48.0f);
    drawTree(24.0f, 48.0f);

    drawTree(28.0f, 48.0f);
    drawTree(30.0f, 48.0f);
    drawTree(32.0f, 48.0f);
    drawTree(34.0f, 48.0f);
    drawTree(36.0f, 48.0f);
    drawTree(38.0f, 48.0f);
    drawTree(40.0f, 48.0f);
    //bush
    drawBush(-1.2f, 16.5f);
    drawBush(-1.2f, 17.0f);
    drawBush(-1.2f, 17.5f);
    drawBush(-1.2f, 18.0f);

    drawBush(-1.2f, 20.0f);
    drawBush(-1.2f, 20.5f);
    drawBush(-1.2f, 21.0f);

    drawBush(-1.2f, 24.0f);
    drawBush(-1.2f, 24.5f);
    drawBush(-1.2f, 25.0f);

    drawBush(-1.2f, 28.0f);

    drawBush(-23.0f, 16.0f);
    drawBush(-23.0f, 16.5f);
    drawBush(-23.0f, 17.0f);
    drawBush(-23.0f, 20.0f);
    drawBush(-23.0f, 20.5f);
    drawBush(-23.0f, 21.0f);
    drawBush(-23.0f, 24.5f);
    drawBush(-23.0f, 25.0f);
    drawBush(-23.0f, 25.5f);

    drawBush(-1.0f, 48.0f);
    drawBush(-1.0f, 48.0f);
    drawBush(3.0f, 48.0f);
    drawBush(-5.0f, 48.0f);
    drawBush(-7.0f, 48.0f);
    drawBush(-9.0f, 48.0f);
    drawBush(-12.0f, 48.0f);
    drawBush(-15.0f, 48.0f);
    drawBush(-18.0f, 48.0f);
    drawBush(-21.0f, 48.0f);
    drawBush(-23.0f, 48.0f);
    drawBush(-25.0f, 48.0f);
    drawBush(-27.0f, 48.0f);
    drawBush(-30.0f, 48.0f);
    drawBush(-33.0f, 48.0f);
    drawBush(-36.0f, 48.0f);
    drawBush(5.0f, 48.0f);
    drawBush(7.0f, 48.0f);
    drawBush(9.0f, 48.0f);
    drawBush(12.0f, 48.0f);
    drawBush(15.0f, 48.0f);
    drawBush(18.0f, 48.0f);
    drawBush(21.0f, 48.0f);
    drawBush(23.0f, 48.0f);
    drawBush(25.0f, 48.0f);
    drawBush(27.0f, 48.0f);
    drawBush(30.0f, 48.0f);
    drawBush(33.0f, 48.0f);
    drawBush(36.0f, 48.0f);
    drawBush(39.0f, 48.0f);
    drawBush(42.0f, 48.0f);
    drawBush(46.0f, 48.0f);
    drawBush(49.0f, 48.0f);
    drawBush(52.0f, 48.0f);

    //tree
    drawTree(-6.0f, 21.0f);
    drawTree(-10.0f, 21.0f);
    drawTree(-15.0f, 21.0f);
    drawTree(-20.0f, 21.0f);


    drawForestTree(10.0f, 39.0f);
    drawForestTree(10.0f, 35.0f);
    drawForestTree(10.0f, 30.0f);
    drawForestTree(10.0f, 25.0f);
    drawForestTree(10.0f, 20.0f);
    drawForestTree(10.0f, 15.0f);
    drawForestTree(10.0f, 10.0f);



    drawBush(-14.0f, -3.0f);
    drawBush(-13.0f, -3.0f);
    drawBush(-12.0f, -3.0f);
    drawBush(-8.0f, -3.0f);
    drawBush(-7.0f, -3.0f);
    drawBush(-6.0f, -3.0f);
    drawBush(5.0f, -3.0f);
    drawBush(6.0f, -3.0f);
    drawBush(7.0f, -3.0f);
    drawBush(12.0f, -3.0f);
    drawBush(13.0f, -3.0f);
    drawBush(14.0f, -3.0f);

    // parking lines
    drawParkingLines(-5.0f, -0.35f, -1.0f, 5);
    drawParkingLines(-18.5f, -0.35f, -1.0f, 5);
    drawParkingLines(-23.0f, -0.35f, 15.0f, 8);
   drawParkingLines(-23.0f, -0.35f, 21.0f, 8);

    //motorParking
    drawParkingLines(20.0f, -0.35f, 12.8f, 1);
    drawParkingLines(20.0f, -0.35f, 13.6f, 1);
    drawParkingLines(20.0f, -0.35f, 14.2f, 1);
    drawParkingLines(20.0f, -0.35f, 15.0f, 1);
    drawParkingLines(20.0f, -0.35f, 15.8f, 1);
    drawParkingLines(20.0f, -0.35f, 16.6f, 1);
    drawParkingLines(20.0f, -0.35f, 17.2f, 1);
    drawParkingLines(20.0f, -0.35f, 18.0f, 1);
    drawParkingLines(20.0f, -0.35f, 18.8f, 1);
    drawParkingLines(20.0f, -0.35f, 19.6f, 1);
    drawParkingLines(20.0f, -0.35f, 20.2f, 1);
    drawParkingLines(20.0f, -0.35f, 21.0f, 1);

    // 
    // 
    //Road line

    drawlineRoad(1.0f, -0.4f, 5.0f, -90.0f, 6.0f);
    drawlineRoad(-10.0f, -0.4f, 5.0f, -90.0f, 6.0f);
    drawlineRoad(-20.0f, -0.4f, 5.0f, -90.0f, 6.0f);
    //drawlineRoad(-30.0f, -0.4f, 5.0f, -90.0f, 6.0f);
    //drawlineRoad(-40.0f, -0.4f, 5.0f, -90.0f, 6.0f);

    drawlineRoad(4.0f, -0.4f, 19.0f, -180.0f, 6.0f);
    drawlineRoad(4.0f, -0.4f, 30.0f, -180.0f, 6.0f);
    drawlineRoad(15.0f, -0.4f, 19.0f, -180.0f, 6.0f);
    drawlineRoad(15.0f, -0.4f, 30.0f, -180.0f, 6.0f);
    drawlineRoad(15.0f, -0.4f, 10.0f, -180.0f, 6.0f);

    drawlineRoad(-12.0f, -0.4f, 12.0f, -90.0f, 6.0f);
    drawlineRoad(-22.0f, -0.4f, 12.0f, -90.0f, 6.0f);
    //drawlineRoad(-32.0f, -0.4f, 12.0f, -90.0f, 6.0f);
   // drawlineRoad(-42.0f, -0.4f, 12.0f, -90.0f, 6.0f);

    drawlineRoad(-3.0f, -0.4f, 32.0f, -90.0f, 4.0f);
    drawlineRoad(-12.0f, -0.4f, 32.0f, -90.0f, 6.0f);
    drawlineRoad(-22.0f, -0.4f, 32.0f, -90.0f, 6.0f);
    drawlineRoad(-32.0f, -0.4f, 32.0f, -90.0f, 6.0f);
    drawlineRoad(-42.0f, -0.4f, 32.0f, -90.0f, 6.0f);

    drawlineRoad(-12.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(-22.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(-32.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(-42.0f, -0.4f, 43.0f, -90.0f, 6.0f);

    drawlineRoad(1.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(12.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(22.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(32.0f, -0.4f, 43.0f, -90.0f, 6.0f);
    drawlineRoad(42.0f, -0.4f, 43.0f, -90.0f, 6.0f);




    //palm tree
    drawPalmTree(-8.0f, -2.0f); // Add a palm tree
    //drawPalmTree(-3.0f, -2.0f); // Add another palm tree
    drawPalmTree(-15.0f, -2.0f); // Add a palm tree
    drawPalmTree(-21.0f, -2.0f); // Add another palm tree

    //drawPalmTree(0.0f, -2.0f); // Add a palm tree
    drawPalmTree(3.0f, -2.0f); // Add another palm tree
    drawPalmTree(8.0f, -2.0f); // Add a palm tree
    drawPalmTree(15.0f, -2.0f); // Add another palm tree


    drawPalmTree(-8.0f, 8.0f); // Add a palm tree
    drawPalmTree(-3.0f, 8.0f); // Add another palm tree
    drawPalmTree(-15.0f, 8.0f); // Add a palm tree
    drawPalmTree(-21.0f, 8.0f); // Add another palm tree

    drawSportComplex();
    drawCanopyWalkway();

    
    glTranslatef(0.0f, 2.0f,-17.0f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glScalef(0.2f, 0.2f, 0.2f);
    zombie(texture,ang);

    glutSwapBuffers();
    GLfloat z = 10.0;

    ang += z;
    if (ang >= 45 || ang <= -45)
    {
        ang = -ang;
    }
   
}



// Function to handle window resizing
void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w / (float)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

//Function to handle keyboard inputs
void keyboard(unsigned char key, int x, int y) {
    if (key == 'q')
    {
        xrot += 1;
        if (xrot > 360) xrot -= 360;
    }
    if (key == 'z')
    {
        xrot -= 1;
        if (xrot < -360) xrot += 360;
    }
    if (key == 'w')
    {
        float xrotrad, yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos += float(sin(yrotrad));
        zpos -= float(cos(yrotrad));
        ypos -= float(sin(xrotrad));
    }
    if (key == 's')
    {
        float xrotrad, yrotrad;
        yrotrad = (yrot / 180 * 3.141592654f);
        xrotrad = (xrot / 180 * 3.141592654f);
        xpos -= float(sin(yrotrad));
        zpos += float(cos(yrotrad));
        ypos += float(sin(xrotrad));
    }
    if (key == 'd')
    {
        yrot += 1;
        if (yrot > 360) yrot -= 360;
    }

    if (key == 'a')
    {
        yrot -= 1;
        if (yrot < -360)yrot += 360;
    }
    if (key == 27)
    {
        exit(0);
    }
}

void mouseMovement(int x, int y)
{
    int diffx = x - lastx;
    int diffy = y - lasty;
    lastx = x;
    lasty = y;
    xrot += (float)diffy;
    yrot += (float)diffx;
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Stadium and Parking");
    glutDisplayFunc(display);
    glutIdleFunc(display); //update any variables in display,display can be changed to anyhing, as long as you move thevariables to be updated, in this case, angle++;
    glutReshapeFunc(reshape);
    //glEnable(GL_DEPTH_TEST);
    glutKeyboardFunc(keyboard);
    glutPassiveMotionFunc(mouseMovement);
    init();
    texture[0] = LoadTexture("daun.bmp", 1080, 1080);
    texture[1] = LoadTexture("padang.bmp", 1080, 1080);
    texture[2] = LoadTexture("pusatsukan.bmp", 1080, 1080);
    texture[3] = LoadTexture("stadium.bmp", 1080, 1080);
    texture[4] = LoadTexture("jalan.bmp", 1080, 1080);
    glutMainLoop();
    FreeTexture(texture[0]);
    return 0;
}