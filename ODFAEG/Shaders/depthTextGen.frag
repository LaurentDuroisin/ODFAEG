//The depth texture.
uniform sampler2D depthTexture;
/*Génère une texture de profondeur, plus le pixel est noir, plus il se situe en fond d'écran.*/
void main () {	
    // récupère le pixel dans la depth texture    	
	vec4 pixel = texture2D(depthTexture, gl_TexCoord[0].xy);
	/* Si la profondeur du pixel de ce qu'on veut dessiner est plus grande que celle de la depth texture,
	   alors on remet à jour notre depth texture, sinon, on laisse comme ça.*/
	float depth = 1 - gl_FragCoord.z;
	if (depth >= pixel.r)
		gl_FragColor = vec4(depth, depth, depth, depth);
	/*Voilà nous avons maintenant une rendertexture contenant la profondeur de tout les 
	pixels qui ont été dessinés (dans les composantes rgba de la rendertexture)
	nous avons donc créer un depthbuffer dans une texture, c'est note depthTexture!*/
}