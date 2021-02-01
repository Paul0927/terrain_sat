#include "border.h"

void Border::init() {
	int grid1, grid2, level1, level2, d;
	for (int i = -(LEVEL - 1); i <= FINER; i++)
		for (int j = i; j <= FINER; j++) {
			grid1 = (i < 0) ? GRID >> -i : GRID << i;
			grid2 = (j < 0) ? GRID >> -j : GRID << j;
			level1 = i + LEVEL - 1;
			level2 = j + LEVEL - 1;
			d = level2 - level1;
			bVertices[level1][level2].clear();
			int p1, p2;
			for (int k = 1; k < grid2 + 1; k++) {
				p1 = (k - 1) >> d;
				if (p1 == 0)
					p1++;
				if (p1 >= grid1)
					p1 = grid1 - 1;
				p2 = k >> d;
				if (p2 == 0)
					p2++;
				if (p2 >= grid1)
					p2 = grid1 - 1;
				if (p1 != p2) {	
					bVertices[level1][level2].push_back(float(k-1) / grid2);
					bVertices[level1][level2].push_back(0);
					bVertices[level1][level2].push_back(0);
					bVertices[level1][level2].push_back(float(p1) / grid1);
					bVertices[level1][level2].push_back(0);
					bVertices[level1][level2].push_back(float(1) / grid1);	
					bVertices[level1][level2].push_back(float(p2) / grid1);
					bVertices[level1][level2].push_back(0);
					bVertices[level1][level2].push_back(float(1) / grid1);
				}	
				bVertices[level1][level2].push_back(float(p2) / grid1);
				bVertices[level1][level2].push_back(0);
				bVertices[level1][level2].push_back(float(1) / grid1);
				bVertices[level1][level2].push_back(float(k) / grid2);
				bVertices[level1][level2].push_back(0);
				bVertices[level1][level2].push_back(0);
				bVertices[level1][level2].push_back(float(k-1) / grid2);
				bVertices[level1][level2].push_back(0);
				bVertices[level1][level2].push_back(0);
			}
			//if (level1 == 0 && level2 == 3)
			//	for (int k = 0; k < bVertices[level1][level2].size(); k++) {
			//		printf("%.2f ", bVertices[level1][level2][k]);
			//		if ((k % 3) == 2)
			//			printf("\n");
			//	}
			glGenVertexArrays(1, &bVAO[level1][level2]);
			glGenBuffers(1, &bVBO[level1][level2]);
			glBindVertexArray(bVAO[level1][level2]);

			glBindBuffer(GL_ARRAY_BUFFER, bVBO[level1][level2]);
			glBufferData(GL_ARRAY_BUFFER, bVertices[level1][level2].size() * sizeof(GLfloat),
				&bVertices[level1][level2][0], GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
				(GLvoid*)0);
			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindVertexArray(0);
		}
}

void Border::draw(int level1, int level2, bool linemode) {
	assert(level1 <= level2);
	glBindVertexArray(bVAO[level1][level2]);
	if (!linemode)
		glDrawArrays(GL_TRIANGLES, 0, bVertices[level1][level2].size() / 3);
	else
		glDrawArrays(GL_LINE_STRIP, 0, bVertices[level1][level2].size() / 3);
	glBindVertexArray(0);
}
