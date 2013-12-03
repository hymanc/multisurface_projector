Matrix.frustumM(texProjMatrix, 
		0, 
		-nearPlaneSize.x, 
		nearPlaneSize.x,
		-nearPlaneSize.y,
		nearPlaneSize.y,
		nearPlaneDistance,
		farPlaneDistance);

Matrix.setLookAtM(texViewMatrix, 0,
		  0, 100, 10,
		  0, 0	, 0,
		  0, 1  , 0);

float[] scaleBiasMatrix = new float[]{0.5f, 0.0f, 0.0f, 0.0f,
				      0.0f, 0.5f, 0.0f, 0.0f,
				      0.0f, 0.0f, 0.5f, 0.0f,
				      0.5f, 0.5f, 0.5f, 1.0f);
				    
Matrix.multiplyMM(texGenMatrix, 0, scaleBiasMatrix, 0, texProjMatrix,0);
Matrix.multiplyMM(texGenMatrix, 0, texGenMatrix, 0, texViewMatrix, 0);