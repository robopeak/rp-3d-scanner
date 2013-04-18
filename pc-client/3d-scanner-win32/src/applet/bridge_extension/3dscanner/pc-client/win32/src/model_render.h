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

#pragma once

#include "irrlicht/irr.h"

struct CloudPoint {
    int col, row;
    float X, Y, Z;

    int height;
    CloudPoint(int c, int r, float x, float y, float z, int h) : col(c), row(r), X(x), Y(y), Z(z), height(h) {}
    CloudPoint() {}
};

using namespace irr::scene;
using namespace irr::core;

struct ModelRender: public ISimpleApp
{
    virtual bool init_scene();
	virtual void update_scene();
	virtual void render_scene();

    void appendData(const CloudPoint * data, size_t cnt);
    void do_render();

    void setCloudMaterialFlag(video::E_MATERIAL_FLAG flag, bool newvalue);
    void fin();

    void setCloudMaterial(video::SMaterial & m);
	~ModelRender();

    ICameraSceneNode* camera;
	IMeshSceneNode* node;
    SMesh* mesh;
};