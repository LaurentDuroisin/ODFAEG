//The current depth buffer texture.
uniform sampler2D depthBuffer;
//The current frame buffer texture.
uniform sampler2D frameBuffer;
//The current texture.
uniform sampler2D texture;

void main () {	
    // récupère le pixel dans la depth buffer texture.   	
	vec4 depth = texture2D(depthBuffer, gl_TexCoord[0].xy);
	// récupère le pixel dans la frame buffer texture.
	vec4 color = texture2D(frameBuffer, gl_TexCoord[0].xy);
	vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
	vec4 finalColor = gl_Color * pixel;
	if (finalColor.a != 0 && finalColor.a < color.a) {
		gl_FragColor = finalColor;
	} else if (gl_FragCoord.z >= depth.a) {
		gl_FragColor = finalColor;
	}			
}