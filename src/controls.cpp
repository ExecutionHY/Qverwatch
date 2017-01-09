
#include "controls.hpp"

//#include "ObjectManager.hpp"

glm::mat4 View;
glm::mat4 Projection;

glm::mat4 getViewMatrix(){
    return View;
}
glm::mat4 getProjectionMatrix(){
    return Projection;
}



// Initial position : on +Z
glm::vec3 position = glm::vec3( -12, 2, -85 );
//glm::vec3 position = glm::vec3( 0, 2, 0 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f*1.25;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 30.0f; // 3 units / second
float mouseSpeed = 0.005f;
extern ALuint source;
bool play = true;
bool spaceReleased = false;

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetCursorPos(window, windowWidth/2, windowHeight/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(windowWidth/2 - xpos );
	verticalAngle   += mouseSpeed * float(windowHeight/2 - ypos );

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle), 
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle)
	);
	
	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f/2.0f), 
		0,
		cos(horizontalAngle - 3.14f/2.0f)
	);
	
	// Up vector
	glm::vec3 up = glm::cross( right, direction );

	// Move forward
	if (glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}
    
    if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_RELEASE)
        spaceReleased = true;
    
    if (glfwGetKey( window, GLFW_KEY_SPACE ) == GLFW_PRESS) {
        if (spaceReleased) {
            if (play) {
                alSourcePause(source);
                play = false;
            }
            else {
                alSourcePlay(source);
                play = true;
            }
            spaceReleased = false;
            printf("%f, %f, %f\n", position.x, position.y, position.z);
        }
    }
    
    

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45∞ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	Projection = glm::perspective(FoV, float(windowWidth / windowHeight), 0.1f, 10000.0f);
	// Camera matrix
	View       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
    
    for (int i = 0; i < rcdIndex; i++) {
        //object[i].checkPos(position, 50.0f-2*object[i].range);
        object[i].checkPos(position, 1000);
    }
}

/*
glm::mat4 View;
glm::mat4 Projection;

glm::mat4 getViewMatrix() {
    return View;
}
glm::mat4 getProjectionMatrix() {
    return Projection;
}

// Initial position : on +Z
glm::vec3 position = glm::vec3( -12, 1.5, -85 );      // the man tall 2m
// Initial horizontal angle : toward -Zw
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 0.0f;
// Initial Field of View
float initialFoV = 45.0f;

float speed = 10.0f; // 3 units / second
float mouseSpeed = 0.005f;

// Whether I am in the air
const int JS = 50;
static bool JUMP = 0;
static float jumpSpeed = -JS - 1;
static bool SQUAT = 0;
static int squatSpeed = 0;
static vec3 jumpMove(0, 0, 0);
extern vec3 indexed_verticles;
extern vec2 indexed_uvs;
extern vec3 indexed_normals;
extern int phynumber;
extern class OBJ obj[10000];
bool PointinTriangle(vec3 A, vec3 B, vec3 C, vec3 P);

void computeMatricesFromInputs() {
    
    // glfwGetTime is called only once, the first time this function is called
    static double lastTime = glfwGetTime();
    
    // Compute time difference between current and last frame
    double currentTime = glfwGetTime();
    float deltaTime = float(currentTime - lastTime);
    
    // Get mouse position
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    // Reset mouse position for next frame
    glfwSetCursorPos(window, 1024 / 2, 768 / 2);
    
    // Compute new orientation
    horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
    verticalAngle += mouseSpeed * float(768 / 2 - ypos);
    
    // Direction : Spherical coordinates to Cartesian coordinates conversion
    glm::vec3 direction(
                        cos(verticalAngle) * sin(horizontalAngle),
                        sin(verticalAngle),
                        cos(verticalAngle) * cos(horizontalAngle)
                        );
    
    // Right vector
    glm::vec3 right = glm::vec3(
                                sin(horizontalAngle - 3.14f / 2.0f),
                                0,
                                cos(horizontalAngle - 3.14f / 2.0f)
                                );
    
    // Up vector
    glm::vec3 up = glm::cross(right, direction);
    
    vec3 std_dir(
                 sin(horizontalAngle),
                 0,
                 cos(horizontalAngle)
                 );
    vec3 std_up = vec3(0, 1, 0);
    
    vec3 move(0, 0, 0);
    // Move forward
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        int flag=1;
        
        if (flag)move = std_dir;
    }
    // Move backward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        int flag = 1;
        
        if (flag) move = -std_dir * 0.8f;
    }
    // Strafe right
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        int flag = 1;
        
        if (flag)move = right;
        
    }
    // Strafe left
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        int flag = 1;
        
        if (flag)move = -right;
    }
    // Jump up
    if (!JUMP && (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_RELEASE)) {
        jumpSpeed = JS;
        JUMP = 1;
        jumpMove = move;
    }
    // Squat down
    
    if (!SQUAT && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) &&
        (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_RELEASE)) {
        squatSpeed = -JS;
        SQUAT = 1;
    }
    if (SQUAT && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) != GLFW_PRESS) && (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE)) {
        squatSpeed = JS;
        SQUAT = 0;
    }
    
    
    //if (jumpSpeed >= -JS) {
    position += std_up * deltaTime * (float(jumpSpeed) / 3);
    jumpSpeed -= 2 * deltaTime*50;
    //printf("%f", jumpSpeed);
    jumpMove = move;
    position += jumpMove * deltaTime * speed;
    //}
    //else {
    //	JUMP = 0;
    //	if (SQUAT) move = 0.6f * move;
    //	position += move * deltaTime * speed;
    //}
    
    if (SQUAT && squatSpeed < 0) {
        position += std_up * deltaTime * (float(squatSpeed) / 3);
        squatSpeed += 3;
    }
    if (!SQUAT && squatSpeed > 0) {
        position += std_up * deltaTime * (float(squatSpeed) / 3);
        squatSpeed -= 3 ;
    }
    
    
    for (int i = 0; i < obj[0].indices.size(); i += 3) {//obj[0].indices.size()
        
        vec3 v0 = obj[0].indexed_vertices[obj[0].indices[i + 0]];
        vec3 v1 = obj[0].indexed_vertices[obj[0].indices[i + 1]];
        vec3 v2 = obj[0].indexed_vertices[obj[0].indices[i + 2]];
        
        float distance = ((position - v0).x*obj[0].indexed_normals[obj[0].indices[i]].x +
                          (position - v0).y*obj[0].indexed_normals[obj[0].indices[i]].y +
                          (position - v0).z*obj[0].indexed_normals[obj[0].indices[i]].z);
        //printf("%d %f ",i, distance);
        if (fabs(distance) < 1.5f) {
            vec3 add;
            //vec3 ptr = position - add * distance;
            if (distance > 0) add = obj[0].indexed_normals[obj[0].indices[i]];
            else add = -obj[0].indexed_normals[obj[0].indices[i]];
            vec3 ptr = position - add * distance;
            if (PointinTriangle(v0, v1, v2, ptr)) {
                position += add * (1.5f - fabs(distance)); //printf("%d %f ", i, distance);
                if ((obj[0].indexed_normals[obj[0].indices[i]].y/sqrt(obj[0].indexed_normals[obj[0].indices[i]].x*obj[0].indexed_normals[obj[0].indices[i]].x+ obj[0].indexed_normals[obj[0].indices[i]].y*obj[0].indexed_normals[obj[0].indices[i]].y+ obj[0].indexed_normals[obj[0].indices[i]].z* obj[0].indexed_normals[obj[0].indices[i]].z)) > 0.5 && jumpSpeed < 0) {
                    jumpSpeed = 0; JUMP = 0;
                }
                //flag = 0;
            }
        }
    }
    printf("%f %f\n", jumpSpeed, position.y);
    
    
    float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.
    
    // Projection matrix : 45¡Þ Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    Projection = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 10000.0f);
    // Camera matrix
    View = glm::lookAt(
                       position,           // Camera is here
                       position + direction, // and looks here : at the same position, plus "direction"
                       up                  // Head is up (set to 0,-1,0 to look upside-down)
                       );
    
    // For the next frame, the "last time" will be "now"
    lastTime = currentTime;
    
    for (int i = 0; i < rcdIndex; i++) {
        object[i].checkPos(position, 50.0f - 2 * object[i].range);
    }
}

bool PointinTriangle(vec3 A, vec3 B, vec3 C, vec3 P)
{
    vec3 v0 = C - A;
    vec3 v1 = B - A;
    vec3 v2 = P - A;
    
    float dot00 = dot(v0, v0);
    float dot01 = dot(v0, v1);
    float dot02 = dot(v0, v2);
    float dot11 = dot(v1, v1);
    float dot12 = dot(v1, v2);
    
    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);
    
    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno;
    if (u < 0 || u > 1) // if u out of range, return directly
    {
        return false;
    }
    
    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno;
    if (v < 0 || v > 1) // if v out of range, return directly
    {
        return false;
    }
    
    return u + v <= 1;
}

*/
