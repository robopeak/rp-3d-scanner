/*
 *    RoboPeak Project
 *    http://www.robopeak.com
 *
 *    3D Scanner
 *    An applet of RoboticPeak Project
 *
 *    By Shikai Chen (shikai.chn@gmail.com)
 *
 *
 *    Cloud Point render
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include "stdafx.h"
#include "common.h"

#include "model_render.h"




bool ModelRender::init_scene()
{
    SMeshBuffer* buf = new SMeshBuffer;
    mesh = new SMesh;
    setCloudMaterial(buf->Material);
    mesh->addMeshBuffer(buf);
    buf->drop();
    node = smgr->addMeshSceneNode(mesh);
    node->setName("scanning surface");
    node->setReadOnlyMaterials(true);
    node->setAutomaticCulling(EAC_OFF);

    camera = smgr->addCameraSceneNodeFPS(NULL, 100, 0.1,-1, g_KeyMap, 4);
    camera->setNearValue(0.5);
    camera->setFOV(core::PI/4);
    camera->setPosition(vector3df(-50,10,0));
    camera->setTarget(vector3df(00,0,0));
    ILightSceneNode* lit = smgr->addLightSceneNode(camera,vector3df(0,30,-20),video::SColorf(0.3,0.3,0.3),10);
    lit->setRadius(500);
    return true;

}

void ModelRender::setCloudMaterial(video::SMaterial & m)
{
    m.setFlag(video::EMF_POINTCLOUD, true);
    m.DiffuseColor = video::SColor(255,0,0,0);
}

void ModelRender::setCloudMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue)
{
    for (int pos =0; pos < mesh->getMeshBufferCount(); ++pos)
    {
        SMeshBuffer* buf = (SMeshBuffer*)mesh->getMeshBuffer(pos);
        buf->Material.setFlag(flag, newvalue);
    }
}

void ModelRender::update_scene()
{
    static bool wireframe = true;
    static int  disptype = 0;
    if (isKeyToggle(VK_SPACE, &wireframe))
    {
        
        if (disptype == 0) {
            //cloud
            setCloudMaterialFlag(video::EMF_POINTCLOUD, true);
            setCloudMaterialFlag(video::EMF_WIREFRAME, false);
            setCloudMaterialFlag(video::EMF_GOURAUD_SHADING, false);
        } else if (disptype == 1) {
            // wire
            setCloudMaterialFlag(video::EMF_POINTCLOUD, false);
            setCloudMaterialFlag(video::EMF_WIREFRAME, true);
            setCloudMaterialFlag(video::EMF_GOURAUD_SHADING, false);
        }else {
            // surface
            setCloudMaterialFlag(video::EMF_POINTCLOUD, false);
            setCloudMaterialFlag(video::EMF_WIREFRAME, false);
            setCloudMaterialFlag(video::EMF_GOURAUD_SHADING, true);
        }
        
        ++disptype;
        if (disptype>2) disptype=0;
    }
    if (KEY_DOWN(VK_ESCAPE))
        device->closeDevice();

    ISimpleApp::update_scene();
}

void ModelRender::render_scene()
{
    
}


static bool checkValidSurface( const array<video::S3DVertex> & arr, int a, int b ,int c)
{
    const float MAX_ALLOWED_DIST = 500/50;//50 cm

    if (arr[a].Pos.X==0 && arr[a].Pos.Y==0 && arr[a].Pos.Z==0 )
    {
        return false;
    }
    if (arr[b].Pos.X==0 && arr[b].Pos.Y==0 && arr[b].Pos.Z==0 )
    {
        return false;
    }
    if (arr[c].Pos.X==0 && arr[c].Pos.Y==0 && arr[c].Pos.Z==0 )
    {
        return false;
    }
    
    if (arr[a].Pos.getDistanceFrom(arr[b].Pos) > MAX_ALLOWED_DIST
        || arr[a].Pos.getDistanceFrom(arr[c].Pos) > MAX_ALLOWED_DIST
        || arr[b].Pos.getDistanceFrom(arr[c].Pos) > MAX_ALLOWED_DIST)
    {
        return false;
    }
    
    return true;

}


static void smoothVertex( array<video::S3DVertex> & arr, int a, int b, int c, int output)
{
    vector3df & A = arr[a].Pos;
    vector3df & B = arr[b].Pos;
    vector3df & C = arr[c].Pos;
    vector3df & Output = arr[output].Pos;

    Output.setLength( (A.getLength()*.6 + B.getLength()*.2 + C.getLength()*.2));
}

void ModelRender::appendData(const CloudPoint * data, size_t cnt)
{
    SMeshBuffer* buf = (SMeshBuffer*)mesh->getMeshBuffer(mesh->getMeshBufferCount()-1);
    int pos = 0;

    while (pos < cnt) {
        video::S3DVertex v;
        v.Pos.X = data[pos].X;
        v.Pos.Y = data[pos].Z;
        v.Pos.Z = data[pos].Y;
        v.Pos.setLength(v.Pos.getLength() / 50);
        v.Color = video::SColor(100,255,100,255);
        v.Normal = v.Pos;
        v.Normal.invert();
        v.Normal.normalize();

        buf->Vertices.push_back(v);

        if (data[pos].row && buf->Vertices.size()>data[pos].height ) {

            size_t currentpos = buf->Vertices.size()-1;

            if (checkValidSurface(buf->Vertices, 
                currentpos - data[pos].height - 1 ,
                currentpos - 1,
                currentpos )) {

                buf->Indices.push_back(currentpos - data[pos].height - 1);
                buf->Indices.push_back(currentpos - 1);
                buf->Indices.push_back(currentpos );

                //smooth
                smoothVertex(buf->Vertices, currentpos, currentpos - data[pos].height - 1, currentpos - 1
                                          , currentpos);

            } else {
                buf->Indices.push_back(currentpos - data[pos].height - 1);
                buf->Indices.push_back(currentpos - data[pos].height - 1);
                buf->Indices.push_back(currentpos - data[pos].height - 1);
            }

            if (checkValidSurface(buf->Vertices, 
                currentpos - data[pos].height - 1 ,
                currentpos - data[pos].height,
                currentpos )) {
                buf->Indices.push_back(currentpos - data[pos].height - 1);
                buf->Indices.push_back(currentpos );
                buf->Indices.push_back(currentpos - data[pos].height);


                //smooth
                smoothVertex(buf->Vertices, currentpos, currentpos - data[pos].height - 1, currentpos - data[pos].height
                                          , currentpos);

            } else {
                buf->Indices.push_back(currentpos - data[pos].height - 1);
                buf->Indices.push_back(currentpos - data[pos].height - 1);
                buf->Indices.push_back(currentpos - data[pos].height - 1);
 
                buf->Indices.push_back(currentpos - data[pos].height);
                buf->Indices.push_back(currentpos - data[pos].height);
                buf->Indices.push_back(currentpos - data[pos].height);
            }
        }
  
        pos++;
    }

    mm->recalculateNormals(buf);
    if (buf->Indices.size() > 60000 ) {
        SMeshBuffer* newbuf = new SMeshBuffer;
        setCloudMaterial(newbuf->Material);
        mesh->addMeshBuffer(newbuf);
        newbuf->drop();

        // dup the current list
        appendData(data, cnt);
    }

}

void ModelRender::fin(){


    for (int pos = 0; pos < mesh->getMeshBufferCount(); ++pos)
    {
        SMeshBuffer* buf = (SMeshBuffer*)mesh->getMeshBuffer(pos);
        buf->recalculateBoundingBox();
    //    mm->recalculateNormals(buf);
    }

}

void ModelRender::do_render()
{
	if (device->run())
	{
		if (device->isWindowActive())
		{
			driver->beginScene(true, true, _bg_clr);
			
			update_scene();
			smgr->drawAll();			
			render_scene();
			env->drawAll();

            driver->endScene();
		}
        
		else
		{
		    // if the window is inactive, we should pause the device and release the mouse
		    device->yield();
		}
        
 	}
}

ModelRender::~ModelRender()
{
    mesh->drop();
}
