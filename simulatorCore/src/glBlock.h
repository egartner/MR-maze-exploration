/*
 * glBlock.h
 *
 *  Created on: 23 avr. 2013
 *      Author: ben
 */

#ifndef GLBLOCK_H_
#define GLBLOCK_H_
#include <string>
#include "shaders.h"
#include "vector3D.h"
#include "color.h"
#include "tDefs.h"

namespace ObjLoader {
class ObjLoader;
}

using namespace std;

class GlObject {
protected :
    bool isHighlighted;
public :
    bID blockId;
    GLfloat position[3];
    GLfloat color[4];

    GlObject(bID id);
    GlObject(bID id,const Vector3D &pos, const Vector3D &col);
    virtual ~GlObject(){};

    virtual void setPosition(const Vector3D &p);
    virtual void setColor(const Color &c);
    virtual void setColor(const Vector3D &c);
    virtual void setVisible(bool visible);
    virtual void toggleHighlight();
    virtual string getInfo();
    virtual string getPopupInfo();
    virtual const Vector3D getPosition() { return Vector3D(position[0],position[1],position[2],1); };

    virtual void glDraw(ObjLoader::ObjLoader *ptrObj) {};
    virtual void glDrawId(ObjLoader::ObjLoader *ptrObj,int &n);
    virtual void glDrawIdByMaterial(ObjLoader::ObjLoader *ptrObj,int &n);

};

class GlObstacle : public GlObject {
public :
    GlObstacle(bID id) : GlObject(id){};
    GlObstacle(bID id, const Vector3D &pos, const Vector3D &col) : GlObject(id, pos, col){};
    virtual ~GlObstacle(){};
/*
    virtual void glDraw(ObjLoader::ObjLoader *ptrObj) {};
    virtual void glDrawId(ObjLoader::ObjLoader *ptrObj,int &n);
    virtual void glDrawIdByMaterial(ObjLoader::ObjLoader *ptrObj,int &n);
*/
};

class GlBlock : public GlObject {
public :
    GlBlock(bID id) : GlObject(id){};
    GlBlock(bID id,const Vector3D &pos, const Vector3D &col) : GlObject(id, pos, col){};
    virtual ~GlBlock(){};
/*
    virtual void glDraw(ObjLoader::ObjLoader *ptrObj) {};
    virtual void glDrawId(ObjLoader::ObjLoader *ptrObj,int &n);
    virtual void glDrawIdByMaterial(ObjLoader::ObjLoader *ptrObj,int &n);
*/
};

#endif /* GLBLOCK_H_ */
