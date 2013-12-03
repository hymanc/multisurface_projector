// Include GLFW
#include <GL/glfw.h>

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "controls.hpp"

// Temporary defines as they can't be found...
#define GLFW_KEY_W 87
#define GLFW_KEY_A 65
#define GLFW_KEY_S 83
#define GLFW_KEY_D 68
glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 projViewMatrix;
glm::mat4 projProjectionMatrix;

glm::mat4 getViewMatrix(){
	return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
	return ProjectionMatrix;
}

glm::mat4 getProjViewMatrix(){
	return projViewMatrix;
}
glm::mat4 getProjProjectionMatrix(){
	return projProjectionMatrix;
}


// Initial position : on +Z
glm::vec3 position = glm::vec3( 0, 3, -8 ); 
// Initial horizontal angle : toward -Z
float horizontalAngle = 3.14f;
// Initial vertical angle : none
float verticalAngle = 10.0f;
// Initial Field of View
float initialFoV = 60.0f;

float speed = 3.0f; // 3 units / second
float mouseSpeed = 0.005f;


// Initial position : on +Z
glm::vec3 projposition = glm::vec3( 3,2, 3 ); 
// Initial horizontal angle : toward -Z
float projhorizontalAngle = 3.14f;
// Initial vertical angle : none
float projverticalAngle = 10.0f;
// Initial Field of View
float projinitialFoV = 60.0f;

void computeMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	int xpos, ypos;
	glfwGetMousePos(&xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetMousePos(1024/2, 768/2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024/2 - xpos );
	verticalAngle   += mouseSpeed * float( 768/2 - ypos );

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
	if (glfwGetKey( GLFW_KEY_UP ) == GLFW_PRESS){
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( GLFW_KEY_DOWN ) == GLFW_PRESS){
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( GLFW_KEY_RIGHT ) == GLFW_PRESS){
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( GLFW_KEY_LEFT ) == GLFW_PRESS){
		position -= right * deltaTime * speed;
	}

	float FoV = initialFoV - 5 * glfwGetMouseWheel();

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	ViewMatrix       = glm::lookAt(
								position,           // Camera is here
								position+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

void computeProjMatricesFromInputs(){

	// glfwGetTime is called only once, the first time this function is called
	static double lastTime = glfwGetTime();

	// Compute time difference between current and last frame
	double currentTime = glfwGetTime();
	float deltaTime = float(currentTime - lastTime);

	// Get mouse position
	int xpos, ypos;
	glfwGetMousePos(&xpos, &ypos);

	// Reset mouse position for next frame
	glfwSetMousePos(1024/2, 768/2);

	// Compute new orientation
	projhorizontalAngle += mouseSpeed * float(1024/2 - xpos );
	projverticalAngle   += mouseSpeed * float( 768/2 - ypos );

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
	if (glfwGetKey( GLFW_KEY_W ) == GLFW_PRESS){
		projposition += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey( GLFW_KEY_S ) == GLFW_PRESS){
		projposition -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey( GLFW_KEY_D ) == GLFW_PRESS){
		projposition += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey( GLFW_KEY_A ) == GLFW_PRESS){
		projposition -= right * deltaTime * speed;
	}

	float FoV = projinitialFoV;// - 5 * glfwGetMouseWheel();

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	projProjectionMatrix = glm::perspective(FoV, 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	projViewMatrix       = glm::lookAt(
								projposition,           // Camera is here
								projposition+direction, // and looks here : at the same position, plus "direction"
								up                  // Head is up (set to 0,-1,0 to look upside-down)
						   );

	// For the next frame, the "last time" will be "now"
	lastTime = currentTime;
}

