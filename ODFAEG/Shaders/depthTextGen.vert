/*Vertex shader utilisé pour généré la depth texture.*/
void main () {
    //Transforme la position.
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	// transforme les coordonnées de texture
    gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	//Passage de la couleur au fragment shader.
	gl_FrontColor = gl_Color;
}