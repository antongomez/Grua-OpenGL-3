#version 330 core

in vec3 ourColor;

void main()
{
	gl_FragColor = vec4(ourColor,1.0f);
	//gl_FragColor = vec4 (1.0f, 1.0f, 1.0f ,1.0f);
}