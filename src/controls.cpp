
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
glm::vec3 position = glm::vec3( 0, 10, 0 );
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
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
	if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
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
        }
    }
    
    

	float FoV = initialFoV;// - 5 * glfwGetMouseWheel(); // Now GLFW 3 requires setting up a callback for this. It's a bit too complicated for this beginner's tutorial, so it's disabled instead.

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
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
        object[i].checkPos(position, 50.0f-2*object[i].range);
    }
}
