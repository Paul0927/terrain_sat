#include "ddsloader.h"
#include "utils.h"


DDSLoader::DDSLoader() {
	data = nullptr;
}


DDSLoader::~DDSLoader() {
}

void DDSLoader::getTotalBlock(const char * filename, int mipmap, int &width, 
	int & height, int *& pixels) {
	FILE *file = fopen(filename, "rb");
	if (!file) {
		printf("Cannot open dds file.\n");
		exit(1);
	}
	int size;
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	unsigned char *buffer = new unsigned char[size];
	fread(buffer, 1, size, file);
	fclose(file);

	pixels = ddsRead(buffer, DDS_READER_ABGR, mipmap);
	width = ddsGetWidth(buffer) >> mipmap;
	height = ddsGetWidth(buffer) >> mipmap;

	delete buffer;
}

int readMipmap(FILE* fp) {
	int ret;
	seekfile(fp, 28, SEEK_SET);
	fread(&ret, 4, 1, fp);
	return ret;
}

void fixOff(int &off, int mipmap) {
	if (mipmap == LEVELOFBLENDTEX)
		off += 32768;
}

void restoreOff(int &off, int mipmap) {
	if (mipmap == LEVELOFBLENDTEX)
		off -= 32768;
}

void DDSLoader::loadBorderData(int level, int index, int x, int y, 
	string dataname) {

	width = height = BLENDSIZE;
	int off = 128;
	for (int i = 0; i < level; i++) {
		off += 8 * ((width + 3) / 4) * ((height + 3) / 4);
		width >>= 1;
		height >>= 1;
	}
	int col = (width + 3) / 4;
	int row = (height + 3) / 4;

	int bx, by;
	bx = Min((x + 1) << level, CHUNKNUMBER) - (x << level);
	by = Min((y + 1) << level, CHUNKNUMBER) - (y << level);

	string filename;
	FILE *fp;

    int tx, ty;

    assert(width >= 4 && height >= 4);

	int mipmap, st;

    switch (index) {
    case TOP:
        imageSize = bx * col * 8;
        data = new unsigned char[imageSize];
        for (int i = 0; i < bx; i++) {
            tx = i + (x << level);
            ty = y << level;

            filename = "data/" + getChunkName(tx, ty) + "/" + dataname;
            openfile(filename.c_str(), fp);
			mipmap = readMipmap(fp);
			fixOff(off, mipmap);
            seekfile(fp, off, SEEK_SET);
			restoreOff(off, mipmap);

            fread(data + i * col * 8, col * 8, 1, fp);
            fclose(fp);
        }
        width = bx * width;
        height = BORDERSIZE;
        break;
    case BOTTOM:
        imageSize = bx * col * 8;
        data = new unsigned char[imageSize];
        off += row * col * 8;
        off -= col * 8;
        for (int i = 0; i < bx; i++) {
            tx = i + (x << level);
            ty = by - 1 + (y << level);

            filename = "data/" + getChunkName(tx, ty) + "/" + dataname;
            openfile(filename.c_str(), fp);
			mipmap = readMipmap(fp);
			fixOff(off, mipmap);
            seekfile(fp, off, SEEK_SET);
			restoreOff(off, mipmap);

            fread(data + i * col * 8, col * 8, 1, fp);
            fclose(fp);
        }
        width = bx * width;
        height = BORDERSIZE;
        break;
    case LEFT:
        imageSize = by * row * 8;
        data = new unsigned char[imageSize];
		st = off;
        for (int i = 0; i < by; i++) {
            tx = (x << level);
            ty = i + (y << level);

            filename = "data/" + getChunkName(tx, ty) + "/" + dataname;
            openfile(filename.c_str(), fp);
			off = st;
			mipmap = readMipmap(fp);
			fixOff(off, mipmap);
            seekfile(fp, off, SEEK_SET);

            for (int j = 0; j < row; j++) {
                fread(data + i * row * 8 + j * 8, 8, 1, fp);
                if (j != row - 1) {
                    off += col * 8;
                    seekfile(fp, off, SEEK_SET);
                }
            }
            fclose(fp);
        }
        width = BORDERSIZE;
        height = height * by;
        break;
    case RIGHT:
        imageSize = by * row * 8;
        data = new unsigned char[imageSize];
        off += col * 8 - 8;
		st = off;
        for (int i = 0; i < by; i++) {
            tx = bx - 1 + (x << level);
            ty = i + (y << level);

            filename = "data/" + getChunkName(tx, ty) + "/" + dataname;
            openfile(filename.c_str(), fp);
			off = st;
			mipmap = readMipmap(fp);
			fixOff(off, mipmap);
            seekfile(fp, off, SEEK_SET);

            for (int j = 0; j < row; j++) {
                fread(data + i * row * 8 + j * 8, 8, 1, fp);
                if (j != row - 1) {
                    off += col * 8;
                    seekfile(fp, off, SEEK_CUR);
                }
            }
            fclose(fp);
        }
        width = BORDERSIZE;
        height = by * height;
        break;
    case LEFTTOP:
        imageSize = 8;
        data = new unsigned char[imageSize];
        filename = "data/" + getChunkName(x << level, y << level) + "/" + dataname;
        openfile(filename.c_str(), fp);
        mipmap = readMipmap(fp);
        fixOff(off, mipmap);
        seekfile(fp, off, SEEK_SET);
		restoreOff(off, mipmap);
        fread(data, 8, 1, fp);
        fclose(fp);
        width = height = BORDERSIZE;
        break;
    case RIGHTTOP:
        imageSize = 8;
        data = new unsigned char[imageSize];
        off += col * 8 - 8;
        filename = "data/" + getChunkName(bx - 1 + (x << level), y << level) + "/" +
            dataname;
        openfile(filename.c_str(), fp);
        mipmap = readMipmap(fp);
        fixOff(off, mipmap);
        seekfile(fp, off, SEEK_SET);
		restoreOff(off, mipmap);
        fread(data, 8, 1, fp);
        fclose(fp);
        width = height = BORDERSIZE;
        break;
    case LEFTBOTTOM:
        imageSize = 8;
        data = new unsigned char[imageSize];
        off += row * col * 8 - col * 8;
        filename = "data/" + getChunkName(x << level, by - 1 + (y << level)) + "/" +
            dataname;
        openfile(filename.c_str(), fp);
        mipmap = readMipmap(fp);
        fixOff(off, mipmap);
        seekfile(fp, off, SEEK_SET);
		restoreOff(off, mipmap);
        fread(data, 8, 1, fp);
        fclose(fp);
        width = height = BORDERSIZE;
        break;
    case RIGHTBOTTOM:
        imageSize = 8;
        data = new unsigned char[imageSize];
        off += row * col * 8 - 8;
        filename = "data/" + getChunkName(bx - 1 + (x << level), by - 1 + (y << level))
            + "/" + dataname;
        openfile(filename.c_str(), fp);
        mipmap = readMipmap(fp);
        fixOff(off, mipmap);
        seekfile(fp, off, SEEK_SET);
		restoreOff(off, mipmap);
        fread(data, 8, 1, fp);
        fclose(fp);
        width = height = BORDERSIZE;
        break;
    }
}

void DDSLoader::loadCompressedData(int level, int index, int x, int y, 
	string dataname) {
	if (data) {
		delete data;
		data = nullptr;
	}

	assert(index >= 0 && index < 9);

	if (index != CENTER) {
		loadBorderData(level, index, x, y, dataname);
		return;
	}

	int off = 128;
	width = height = BLENDSIZE;

	for (int i = 0; i < level; i++) {
		off += 8 * ((width + 3) / 4) * ((height + 3) / 4);
		width >>= 1;
		height >>= 1;
	}

	int bx, by;
	bx = Min((x + 1) << level, CHUNKNUMBER) - (x << level);
	by = Min((y + 1) << level, CHUNKNUMBER) - (y << level);

	int bSize = ((width + 3) / 4) * ((height + 3) / 4) * 8;
	imageSize = bSize * bx * by;
	data = new unsigned char[imageSize];

	FILE *fp;

	int col = (width + 3) / 4;
	int row = (height + 3) / 4;

	int tx, ty;
	string filename;
	for (int j = 0; j < by; j++)
		for (int i = 0; i < bx; i++) {
			tx = i + (x << level);
			ty = j + (y << level);

			filename = "data/" + getChunkName(tx, ty) + "/" + dataname;
			openfile(filename.c_str(), fp);

			int mipmap = readMipmap(fp);
			fixOff(off, mipmap);
			seekfile(fp, off, SEEK_SET);
			restoreOff(off, mipmap);

			for (int k = 0; k < row; k++) {
				fread(data +  (j * row  + k)* bx * col * 8 + i * col * 8,
					col * 8, 1, fp);
			}
			fclose(fp);
		}

	width *= bx;
	height *= by;
}

int DDSLoader::getWidth() const {
	return width;
}

int DDSLoader::getHeight() const {
	return height;
}

unsigned char * DDSLoader::getData() const {
	return data;
}

int DDSLoader::getImageSize() const {
	return imageSize;
}

int DDSLoader::getWidth(FILE *fp) {
	int ret;
	seekfile(fp, 16, SEEK_SET);
	fread(&ret, 4, 1, fp);
	return ret;
}

int DDSLoader::getHeight(FILE *fp) {
	int ret;
	seekfile(fp, 12, SEEK_SET);
	fread(&ret, 4, 1, fp);
	return ret;
}

