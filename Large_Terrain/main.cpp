#include "global.h"
#include "camera.h"
#include "shader.h"
#include "frameBuffer.h"
#include "utils.h"
#include "skybox.h"
#include "vTex.h"
#include "treeNode.h"
#include "sat.h"
#include "border.h"


const int WIDTH = 1280, HEIGHT = 720;
const float fov = 45.0f;
const float znear = 10.0f;
const float zfar = 80000.0f;
const float MAXSCALE = CHUNKREALSIZE * VIEWCHUNKNUMBER * 0.5f;
const float S = 2.0f;
const int BLOCKNUMBER = 1 << (LEVEL - 1);
const int GRID = CHUNKSIZE * VIEWCHUNKNUMBER / BLOCKNUMBER / 2;

const int SPACE_ERROR_THRESHOLD = 0;

const int BASICTEXNUM = 9;
GLuint tmpNormal, tmpTex;

bool keys[1024];
bool lineMode = false;
bool displayFeedback = false;
int showBlend = 0;
bool saveBlendTex = false;
bool geometryclipmap = false;

int geoLevel[BLOCKNUMBER][BLOCKNUMBER];

vector<GLfloat> vertices[LEVEL + FINER];
GLuint vao[LEVEL + FINER], vbo[LEVEL + FINER];

Shader shader, feedbackShader;
Camera camera;
FrameBuffer fb, feedback;
Skybox skybox;
VTex htex, btex, ntex;

TreeNode blocks[BLOCKNUMBER][BLOCKNUMBER];
int totNode;

int gWidth, gHeight;

glm::mat4 projection, view, model;

GLuint hLevelTex, hLevelTex1;
unsigned char hLevel1[CHUNKNUMBER * CHUNKNUMBER];
float hLevel[CHUNKNUMBER * CHUNKNUMBER];

bool needUpdate[CHUNKNUMBER][CHUNKNUMBER];

Border border;

void buildGrid(int grid, vector<GLfloat> &vertices) {
	vertices.clear();

	for (int j = 1; j < grid - 1; j++)
		for (int i = 1; i < grid; i++) {

			for (int k = 0; k < ((i == 1) ? 2 : 1); k++) {
				vertices.push_back(float(j) / grid);
				vertices.push_back(0);
				vertices.push_back(float(i) / grid);
			}
			
			j++;

			for (int k = 0; k < ((i == grid - 1) ? 2 : 1); k++) {
				vertices.push_back(float(j) / grid);
				vertices.push_back(0);
				vertices.push_back(float(i) / grid);
			}
			j--;
		}
}

void key_callback(GLFWwindow *windows, int key, int scancode,
	int action, int mode) {
	if (action == GLFW_PRESS)
		keys[key] = true;
	else
		if (action == GLFW_RELEASE)
			keys[key] = false;

	if (keys[GLFW_KEY_M])
		lineMode = !lineMode;

	if (keys[GLFW_KEY_F])
		camera.setPos(86731, 10587, 70656);

	if (keys[GLFW_KEY_B])
		showBlend = (showBlend + 1) % 3;

	if (keys[GLFW_KEY_X])
		saveBlendTex = true;

	if (keys[GLFW_KEY_G])
		camera.setPos(2096, 2500, 2096);

	if (keys[GLFW_KEY_P])
		printf("Current Positon: %d %d %d\n", int(camera.getPos().x),
			int(camera.getPos().z), int(camera.getPos().y));

	if (keys[GLFW_KEY_O])
		camera.setPos(1500, 3870, 1500);

	if (keys[GLFW_KEY_ESCAPE])
		exit(0);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
//	static double lastY = 0;
//	camera.rotate(xpos, ypos - lastY);
//	lastY = ypos;
}

void initBlocks() {
	for (int i = 0; i < BLOCKNUMBER; i++) {
		float size = 1.0f / BLOCKNUMBER;
		float cx, cy;
		cx = -1 + size;
		cy = -1 + size * (i * 2 + 1);
		for (int j = 0; j < BLOCKNUMBER; j++) {
			blocks[i][j].cX = cx;
			blocks[i][j].cY = cy;
			blocks[i][j].size = size;
			cx += 2 * size;
			blocks[i][j].dis = sqrt(sqr(cx * MAXSCALE) + sqr(cy * MAXSCALE));
        }
	}
}

void buildGeoLevel() {
	for (int i = 0; i < BLOCKNUMBER; i++) {
		for (int j = 0; j < BLOCKNUMBER; j++) {
			int k = 0;
			int now = BLOCKNUMBER >> 2;
			while (now <= i && now <= j && i < BLOCKNUMBER - now && j < BLOCKNUMBER - now) {
				k++;
				now += BLOCKNUMBER >> (k + 2);
				if (k + 2 == LEVEL - 1)
					break;
			}
			geoLevel[i][j] = k+1;
			//printf("%d ", geoLevel[i][j]);
		}
		//printf("\n");
	}
}

void init() {
	memset(keys, 0, sizeof(keys));

    camera.setGrid(GRID);

	//camera.setPos(1, 4000, 1);
	//camera.setPos(2096, 2500, 2096);
	camera.setPos(1500, 3870, 1500);
	//camera.setPos(86731, 10587, 70656);

	for (int i = -(LEVEL - 1); i <= FINER; i++) {
		int grid;
		if (i < 0)
			grid = GRID >> (-i);
		else
			grid = GRID << i;
		int k = i + LEVEL - 1;
		buildGrid(grid, vertices[k]);

		glGenVertexArrays(1, &vao[k]);
		glGenBuffers(1, &vbo[k]);
		glBindVertexArray(vao[k]);

		glBindBuffer(GL_ARRAY_BUFFER, vbo[k]);
		glBufferData(GL_ARRAY_BUFFER, vertices[k].size() * sizeof(GLfloat),
			&vertices[k][0], GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
			(GLvoid*)0);
		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	border.init();

	shader.attach(GL_VERTEX_SHADER, VERTEXSHADERPATH);
	shader.attach(GL_FRAGMENT_SHADER, FRAGMENTSHADERPATH);
	shader.link();

	//feedbackShader.attach(GL_VERTEX_SHADER, VERTEXSHADERPATH);
	//feedbackShader.attach(GL_FRAGMENT_SHADER, FEEDBACKFRAGMENT);
	//feedbackShader.link();

	//skybox.init();

	string filename;
	int w, h, channels, type;
	unsigned char *img;

	filename = "ps_texture_16k.png";
	glGenTextures(1, &tmpTex);
	glBindTexture(GL_TEXTURE_2D, tmpTex);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	img = SOIL_load_image(filename.c_str(),	&w, &h, &channels, SOIL_LOAD_AUTO);
	type = (channels == 4) ? GL_RGBA : GL_RGB;
	glTexImage2D(GL_TEXTURE_2D, 0, type, w, h, 0, type, GL_UNSIGNED_BYTE, img);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(img);

	htex.init(CHUNKSIZE, CHUNKNUMBER, "dem", LEVELOFTERRAIN);
	
	initBlocks();

	memset(hLevel, 0, sizeof(hLevel));
	glGenTextures(1, &hLevelTex);
	glBindTexture(GL_TEXTURE_2D, hLevelTex);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenTextures(1, &hLevelTex1);
	glBindTexture(GL_TEXTURE_2D, hLevelTex1);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	buildGeoLevel();

	memset(hLevel, 0, sizeof(hLevel));
	memset(hLevel1, 0, sizeof(hLevel1));
}

void setHTex(Shader &shader) {
	PTex* ptex = htex.getPtex();

	glEnable(GL_TEXTURE_2D);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, htex.getTex());

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, ptex->getTex());

	setUniformi("vtex", 0, shader);
	setUniformi("texHeightmap", 1, shader);

	glActiveTexture(GL_TEXTURE16);
	glBindTexture(GL_TEXTURE_2D, hLevelTex);
	setUniformi("texHLevel", 16, shader);

	
	glActiveTexture(GL_TEXTURE17);
	glBindTexture(GL_TEXTURE_2D, hLevelTex1);
	setUniformi("texHLevel1", 17, shader);

	glActiveTexture(GL_TEXTURE19);
	glBindTexture(GL_TEXTURE_2D, tmpTex);
	setUniformi("terrainTex", 19, shader);
}


glm::vec4 space_xy[BLOCKNUMBER + 1][BLOCKNUMBER + 1][2];

bool frustumCull(int x, int y) {
	int xs, xb, ys, yb, zs, zb;
	xs = xb = ys = yb = zs = zb = 0;
	for (int i = 0; i <= 1; i++)
		for (int j = 0; j <= 1; j++)
			for (int k = 0; k < 2; k++) {
				glm::vec4 pos = space_xy[x+i][y+j][k];
				xs = (pos.x < -pos.w) ? xs + 1 : xs;
				xb = (pos.x > pos.w) ? xb + 1 : xb;
				ys = (pos.y < -pos.w) ? ys + 1 : ys;
				yb = (pos.y > pos.w) ? yb + 1 : yb;
				zs = (pos.z < -pos.w) ? zs + 1 : zs;
				zb = (pos.z > pos.w) ? zb + 1 : zb;
			}
	return xs == 8 || xb == 8 || ys == 8 || yb == 8 || zs == 8 || zb == 8;
}

void blockCull(glm::mat4 mat) {
	glm::vec4 scale;
	scale.x = scale.z = MAXSCALE / BLOCKNUMBER * 2;
	scale.y = scale.w = 1;

	for (int i = 0; i < BLOCKNUMBER + 1; i++)
		for (int j = 0; j < BLOCKNUMBER + 1; j++) {
			float cx, cy;
			int ti, tj;
			ti = (i == BLOCKNUMBER) ? i - 1 : i;
			tj = (j == BLOCKNUMBER) ? j - 1 : j;
			cx = (blocks[ti][tj].cX - blocks[ti][tj].size) * MAXSCALE;
			cy = (blocks[ti][tj].cY - blocks[ti][tj].size) * MAXSCALE;
			for (int k = -1; k < 2; k += 2) {
				glm::vec4 position(0, k * 4000, 0, 1);
				position.x = (i == BLOCKNUMBER) ? 1 : 0;
				position.z = (j == BLOCKNUMBER) ? 1 : 0;
				glm::vec4 pos = scale * position;
				pos.x += cx;
				pos.z += cy;
				pos = mat * pos;
				int t = (k == -1) ? 0 : 1;
				space_xy[i][j][t] = pos;
			}
		}

	memset(needUpdate, 0, sizeof(needUpdate));
	for (int i = 0; i < BLOCKNUMBER; i++)
		for (int j = 0; j < BLOCKNUMBER; j++) {
			blocks[i][j].vis = !frustumCull(i, j);
			int lx = blocks[i][j].lx / CHUNKSIZE;
			int rx = blocks[i][j].rx / CHUNKSIZE;
			int ly = blocks[i][j].ly / CHUNKSIZE;
			int ry = blocks[i][j].ry / CHUNKSIZE;
			for (int k1 = lx; k1 <= rx; k1++)
				for (int k2 = ly; k2 <= ry; k2++) {
					needUpdate[k1][k2] |= blocks[i][j].vis;
				}
		}
}

void  updateBlocks(glm::mat4 matrix) {
	for (int i = 0; i < BLOCKNUMBER; i++)
		for (int j = 0; j < BLOCKNUMBER; j++) {
			if (!blocks[i][j].vis)
				continue;
			int lx, ly, rx, ry;
			lx = blocks[i][j].lx;
			ly = blocks[i][j].ly;
			rx = blocks[i][j].rx;
			ry = blocks[i][j].ry;

			float size = blocks[i][j].size * VIEWCHUNKNUMBER / float(CHUNKNUMBER) * 0.5f;

			tuple<double, int, float, float> result = htex.getSatMax(lx, ly, rx, ry, hLevel);

			double ret = get<0>(result);
			ret /= 10;

			float minHval = get<2>(result);
			minHval = minHval / 10;

			float maxHval = get<3>(result);
			maxHval = maxHval / 10;

			float currentH;
			if (minHval <= camera.getPos().y && maxHval >= camera.getPos().y)
				currentH = camera.getPos().y;
			else if (minHval > camera.getPos().y)
				currentH = minHval;
			else
				currentH = maxHval;

			blocks[i][j].level = -1;
			for (int k = 0; k < LEVEL + FINER; k++) {
				int grid;
				if (k > LEVEL - 1)
					grid = GRID << (k - LEVEL + 1);
				else
					grid = GRID >> (LEVEL - 1 - k);
				double ma = ret / BLOCKNUMBER / grid * VIEWCHUNKNUMBER * CHUNKREALSIZE;

				glm::vec4 p0, p1;
				p0.x = blocks[i][j].cX * MAXSCALE;
				p0.z = blocks[i][j].cY * MAXSCALE;
				float Dp = blocks[i][j].dis;
				float s = size * 2 * MAXSCALE;
				float d = 1 - 1.41421356 * s / 2 / Dp; //1-sqrt(2)*s/2/Dp
				p0.y = -currentH;
				p0 = p0 * d;
				p0.y += currentH;
				p0.w = 1;
				p1 = p0;
				p1.y = p0.y + ma / 2;
				p0 = matrix * p0;
				p0 /= p0.w;
				p1 = matrix * p1;
				p1 /= p1.w;
				double xPixel = (fabs(p0.x - p1.x) / 2) * WIDTH;
				double yPixel = (fabs(p0.y - p1.y) / 2) * HEIGHT;
				double pixel = xPixel * xPixel + yPixel * yPixel;
				pixel = sqrt(pixel);
				if (pixel <= 1) {
					blocks[i][j].level = k;
					break;
				}
			}

            if (blocks[i][j].level == -1)
                blocks[i][j].level = LEVEL + FINER - 1;

			lx /= CHUNKSIZE;
			ly /= CHUNKSIZE;
			rx /= CHUNKSIZE;
			ry /= CHUNKSIZE;
			for (int k1 = lx; k1 <= rx; k1++)
				for (int k2 = ly; k2 <= ry; k2++) {
					int p = floor(hLevel[k2 * CHUNKNUMBER + k1]);
					float r = float(VIEWCHUNKNUMBER) / BLOCKNUMBER;
					int dp = (CHUNKSIZE >> p) * r;
					r = float(GRID) / dp;
					if (r > 1) {
						p = -floor(log2(r));
						p += LEVEL - 1;
						p = Max(0, p);
					}
					else {
						p = ceil(log2(1 / r));
						p += LEVEL - 1;
						p = Min(p, LEVEL + FINER - 1); 
					}
					blocks[i][j].level = Max(blocks[i][j].level, p);
				}
		}
}

void calcBlockScope(float currentX, float currentZ) {
	for (int i = 0; i < BLOCKNUMBER; i++)
		for (int j = 0; j < BLOCKNUMBER; j++) {
			float r = VIEWCHUNKNUMBER / float(CHUNKNUMBER);
			float cx, cy, size;
			cx = currentX + blocks[i][j].cX * r * 0.5f;
			cy = currentZ + blocks[i][j].cY * r * 0.5f;
			size = blocks[i][j].size * r * 0.5f;

			int lx, rx, ly, ry;
			lx = (cx - size) * CHUNKNUMBER * CHUNKSIZE;
			rx = (cx + size) * CHUNKNUMBER * CHUNKSIZE;
			ly = (cy - size) * CHUNKNUMBER * CHUNKSIZE;
			ry = (cy + size) * CHUNKNUMBER * CHUNKSIZE;

			int maxCoor = CHUNKNUMBER * CHUNKSIZE - 1;
			clamp(lx, 0, maxCoor);
			clamp(ly, 0, maxCoor);
			clamp(rx, 0, maxCoor);
			clamp(ry, 0, maxCoor);

			blocks[i][j].lx = lx;
			blocks[i][j].ly = ly;
			blocks[i][j].rx = rx;
			blocks[i][j].ry = ry;
		}
}

void updateHTex() {
	float currentX, currentZ, currentH;
	currentX = camera.getPos().x / CHUNKNUMBER / CHUNKREALSIZE;
	currentZ = camera.getPos().z / CHUNKNUMBER / CHUNKREALSIZE;
	currentH = camera.getPos().y;

	static glm::mat4 matrix;
	matrix = projection * view * model;

	calcBlockScope(currentX, currentZ);

	blockCull(matrix);

	//printf("After Cull\n");
	
	htex.clear();
	htex.checkThreadState();
	htex.update(currentX, currentZ, currentH, hLevel, hLevel1, matrix, geoLevel, geometryclipmap, needUpdate);
	//printf("After update\n");
	htex.generateTex();

	//sat.loadSATData(currentX, currentZ);

	glBindTexture(GL_TEXTURE_2D, hLevelTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_FLOAT, hLevel);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(GL_TEXTURE_2D, hLevelTex1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNKNUMBER, CHUNKNUMBER, 0, GL_RED, GL_UNSIGNED_BYTE, hLevel1);
	glBindTexture(GL_TEXTURE_2D, 0);

	updateBlocks(matrix);
	//printf("After Blocks\n");
}



void drawBlocks(Shader &shader, bool isFeedback) {
	static const int dx[4] = { -1, 0, 1, 0 };
	static const int dy[4] = { 0, -1, 0, 1 };


	for (int i = 0; i < BLOCKNUMBER; i++)
		for (int j = 0; j < BLOCKNUMBER; j++) {
			if (!blocks[i][j].vis)
				continue;
			int l = blocks[i][j].level;

			glBindVertexArray(vao[l]);
			setUniform4f("testcolor", blocks[i][j].color.x, blocks[i][j].color.y, blocks[i][j].color.z, 1, shader);
			setUniform2f("center", blocks[i][j].cX - blocks[i][j].size, blocks[i][j].cY - blocks[i][j].size, shader);
			setUniform4f("testcolor2", float(l) / (LEVEL + FINER - 1), 0, 0, 1, shader);
			float scale = 2 * MAXSCALE / BLOCKNUMBER;
			setUniformi("xzratio", 1, shader);

			setUniformi("signbitX", 1, shader);
			setUniformi("complementX", 0, shader);
			setUniformi("signbitZ", 1, shader);
			setUniformi("complementZ", 0, shader);

			setUniformi("id", i * BLOCKNUMBER + j, shader);

			setUniform4f("scale", scale, 1, scale, 1, shader);

			if (!lineMode)
				glDrawArrays(GL_TRIANGLE_STRIP, 0, vertices[l].size() / 3);
			else
				glDrawArrays(GL_LINE_LOOP, 0, vertices[l].size() / 3);

			glBindVertexArray(0);

			setUniform4f("testcolor2", 0, 0, 1, 1, shader);
			for (int k = 0; k < 4; k++) {
				int x, y;
				x = i + dx[k];
				y = j + dy[k];

				int level1, level2;
				level1 = blocks[i][j].level;
				if (x < 0 || y < 0 || x >= BLOCKNUMBER || y >= BLOCKNUMBER)
					level2 = level1;
				else
					level2 = blocks[x][y].level;

				if (level1 > level2) {
					level2 = level1;
				}
				
				switch (k) {
				case 0:
					break;
				case 1:
					setUniformi("xzratio", 0, shader);
					setUniformi("signbitZ", -1, shader);
					setUniformi("complementZ", 1, shader);
					break;
				case 2:
					setUniformi("xzratio", 1, shader);
					setUniformi("signbitZ", -1, shader);
					setUniformi("complementZ", 1, shader);
					setUniformi("signbitX", -1, shader);
					setUniformi("complementX", 1, shader);
					break;
				case 3:
					setUniformi("xzratio", 0, shader);
					setUniformi("signbitZ", 1, shader);
					setUniformi("complementZ", 0, shader);
					setUniformi("signbitX", -1, shader);
					setUniformi("complementX", 1, shader);
					break;
				}
        		border.draw(level1, level2, lineMode);
			}
		}
}

void drawGrid(Shader &shader, bool isFeedback) {
	float currentX, currentZ;
	currentX = camera.getPos().x / CHUNKNUMBER / CHUNKREALSIZE;
	currentZ = camera.getPos().z / CHUNKNUMBER / CHUNKREALSIZE;
	setUniform4f("currentPos", currentX, currentZ, camera.getPos().y, 0, shader);

	drawBlocks(shader, isFeedback);
}



void drawFrame() {
	glClearDepth(1.0f);
	glClearColor(0.51f, 0.83f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);

	//AA
	glEnable(GL_MULTISAMPLE);

	projection = glm::perspective(glm::radians(fov), (float)WIDTH / (float)HEIGHT,
		znear, zfar);

	glm::vec3 dir = camera.getDir();
	glm::vec3 up = camera.getUp();
	glm::vec3 pos = camera.getPos();
	pos.x = 0;
	pos.z = 0;

	view = glm::lookAt(pos, pos + dir, up);

	model = glm::mat4(1.0f);


	updateHTex();

	glViewport(0, 0, gWidth, gHeight);
	shader.begin();
	setUniformMatrix4f("projection", projection, shader);
	setUniformMatrix4f("view", view, shader);
	setUniformMatrix4f("model", model, shader);

	setUniformi("showBlend", showBlend, shader);
	setHTex(shader);
	drawGrid(shader, false);

	shader.end();
}

int main() {
	if (!glfwInit()){
		exit(EXIT_FAILURE);
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_DECORATED, GL_FALSE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	//Anti-Aliasing
	glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT,
		"Terrain SAT", nullptr, nullptr);

	if (window == nullptr) {
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

#ifndef __APPLE__
	glewExperimental = (GLboolean)true;
	if (glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
#endif

	glfwGetFramebufferSize(window, &gWidth, &gHeight);
	glViewport(0, 0, gWidth, gHeight);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//vsync
	typedef BOOL(APIENTRY *PFNWGLSWAPINTERVALFARPROC)(int);
	PFNWGLSWAPINTERVALFARPROC wglSwapIntervalEXT = 0;
	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALFARPROC)wglGetProcAddress("wglSwapIntervalEXT");

	//wglSwapIntervalEXT(1);
	wglSwapIntervalEXT(0);

	init();

	GLfloat lastTime = glfwGetTime();
	GLfloat currentTime;



	//GLint maxUnit;
	//glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxUnit);

	VTex::initThread();
	htex.clear();
	htex.loadCoarsest();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		currentTime = glfwGetTime();
		camera.revise();
		camera.move(keys, currentTime - lastTime, CHUNKNUMBER * CHUNKREALSIZE,
			CHUNKNUMBER * CHUNKREALSIZE);
		lastTime = currentTime;

		
		drawFrame();


		glfwSwapBuffers(window);
	}

	//glDeleteVertexArrays(1, &vao);
	//glDeleteBuffers(1, &vbo);
	glfwTerminate();
	VTex::free();
	return 0;
}
