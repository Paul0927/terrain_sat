#include "skybox.h"
#include "utils.h"

Skybox::Skybox() : 
	name(skyboxPicName), suffix(skyboxPicSuf) {}

Skybox::~Skybox() {
}

void Skybox::init() {
	//string test = "skybox/" + name + "_rt" + suffix;
	/*texID = SOIL_load_OGL_cubemap(
		("skybox/" + name + "_rt" + suffix).c_str(),
		("skybox/" + name + "_lf" + suffix).c_str(),
		("skybox/" + name + "_up" + suffix).c_str(),
		("skybox/" + name + "_dn" + suffix).c_str(),
		("skybox/" + name + "_ft" + suffix).c_str(),
		("skybox/" + name + "_bk" + suffix).c_str(),
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);*/
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	int width, height;
	unsigned char* image;
	for (int i = 0; i < 6; i++) {
		string picName = "skybox/" + name + "_" + partName[i] + suffix;
		image = SOIL_load_image(picName.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 
			width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
		SOIL_free_image_data(image);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	shader.attach(GL_VERTEX_SHADER, SKYBOXVERTEXSHADERPATH);
	shader.attach(GL_FRAGMENT_SHADER, SKYBOXFRAGMENTSHADERPATH);
	shader.link();

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 36 * 3 * sizeof(GLfloat), 
		skyboxVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
		(GLvoid*)0);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Skybox::render(glm::mat4 &projection, glm::mat4 &view) {
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

	shader.begin();

	setUniformMatrix4f("projection", projection, shader);
	setUniformMatrix4f("view", view, shader);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);

	shader.end();

	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);
}
