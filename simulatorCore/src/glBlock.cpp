#include "glBlock.h"

#include <sstream>

#include "objLoader.h"

/************************ CLASS GlObject ************************/
GlObject::GlObject(bID id):blockId(id) {
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;
	color[0] = 0.5;
	color[1] = 0.5;
	color[2] = 0.5;
	color[3] = 1.0;
	isHighlighted = false;
}

GlObject::GlObject(bID id,const Vector3D &pos, const Vector3D &col) : blockId(id) {
	position[0] = pos[0];
	position[1] = pos[1];
	position[2] = pos[2];
	color[0] = col[0];
	color[1] = col[1];
	color[2] = col[2];
	color[3] = 1.0;
	isHighlighted = false;
}

void GlObject::setPosition(const Vector3D &pos) {
	position[0] = GLfloat(pos[0]);
	position[1] = GLfloat(pos[1]);
	position[2] = GLfloat(pos[2]);
}

void GlObject::setColor(const Vector3D &col) {
	color[0] = GLfloat(col[0]);
	color[1] = GLfloat(col[1]);
	color[2] = GLfloat(col[2]);
	color[3] = 1.0;
}

void GlObject::setColor(const Color &col) {
	color[0] = col[0];
	color[1] = col[1];
	color[2] = col[2];
	color[3] = 1.0;
}

void GlObject::setVisible(bool visible) {
    color[3] = visible;
}

void GlObject::toggleHighlight() {
	isHighlighted=!isHighlighted;
}

string GlObject::getInfo() {
    ostringstream out;
	out << blockId << "\n";
	out << fixed;
	out.precision(1);
	out << "Pos=(" << position[0] << "," << position[1] << "," << position[2] << ") ";
	out << "Col=(" << (int)(color[0] * 255) << "," << (int)(color[1] * 255) << "," << (int)(color[2] * 255) << ")";

	return out.str();
}

string GlObject::getPopupInfo() {
    ostringstream out;
	out << blockId << "\n";

	return out.str();
}


void GlObject::glDrawId(ObjLoader::ObjLoader *ptrObj,int &n) {
	glPushMatrix();
	glTranslatef(position[0],position[1],position[2]);
	ptrObj->glDrawId(n);
	glPopMatrix();
}

void GlObject::glDrawIdByMaterial(ObjLoader::ObjLoader *ptrObj,int &n) {
	glPushMatrix();
	glTranslatef(position[0],position[1],position[2]);
	ptrObj->glDrawIdByMaterial(n);
	glPopMatrix();
}

/************************ CLASS GlObject ************************/


