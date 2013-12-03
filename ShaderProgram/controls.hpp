#ifndef CONTROLS_HPP
#define CONTROLS_HPP

void computeMatricesFromInputs();
void computeProjMatricesFromInputs();
glm::mat4 getViewMatrix();
glm::mat4 getProjectionMatrix();
glm::mat4 getProjViewMatrix();
glm::mat4 getProjProjectionMatrix();

#endif