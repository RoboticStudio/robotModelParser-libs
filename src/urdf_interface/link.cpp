/*********************************************************************
* Software License Agreement (BSD License)
* 
*  Copyright (c) 2008, Willow Garage, Inc.
*  All rights reserved.
* 
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
* 
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Willow Garage nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
* 
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Wim Meeussen */


#include "../../include/urdf_interface/link.h"
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <algorithm>
#include <iostream>

namespace urdf{

boost::shared_ptr<Geometry> parseGeometry(TiXmlElement *g)
{
  boost::shared_ptr<Geometry> geom;
  if (!g) return geom;

  TiXmlElement *shape = g->FirstChildElement();
  if (!shape)
  {
      std::cout << "Geometry tag contains no child element." << std::endl;
   // ROS_ERROR("Geometry tag contains no child element.");
    return geom;
  }

  std::string type_name = shape->Value();
  if (type_name == "sphere")
    geom.reset(new Sphere);
  else if (type_name == "box")
    geom.reset(new Box);
  else if (type_name == "cylinder")
    geom.reset(new Cylinder);
  else if (type_name == "mesh")
    geom.reset(new Mesh);
  else
  {
    std::cout << "Unknown geometry type " <<  type_name.c_str() << std::endl;
    //ROS_ERROR("Unknown geometry type '%s'", type_name.c_str());
    return geom;
  }

  // clear geom object when fails to initialize
  if (!geom->initXml(shape)){
      std::cout << "Geometry failed to parse" << std::endl;
      //ROS_ERROR("Geometry failed to parse");
      geom.reset();
  }

  return geom;
}

bool Material::initXml(TiXmlElement *config)
{
  bool has_rgb = false;
  bool has_filename = false;

  this->clear();

  if (!config->Attribute("name"))
  {
	  //ROS_ERROR("Material must contain a name attribute");
    return false;
  }

  this->name = config->Attribute("name");

  // texture
  TiXmlElement *t = config->FirstChildElement("texture");
  if (t)
  {
    if (t->Attribute("filename"))
    {
      this->texture_filename = t->Attribute("filename");
      has_filename = true;
    }
    else
    {
    	//ROS_ERROR("texture has no filename for Material %s",this->name.c_str());
    }
  }

  // color
  TiXmlElement *c = config->FirstChildElement("color");
  if (c)
  {
    if (c->Attribute("rgba"))
    {
      if (!this->color.init(c->Attribute("rgba")))
      {
        // ROS_ERROR("Material %s has malformed color rgba values.",this->name.c_str());
        this->color.clear();
        return false;
      }
      else
        has_rgb = true;
    }
    else
    {
    	// ROS_ERROR("Material %s color has no rgba",this->name.c_str());
    }
  }

  return (has_rgb || has_filename);
}

bool Inertial::initXml(TiXmlElement *config)
{
  this->clear();

  // Origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (!o)
  {
	  // ROS_DEBUG("Origin tag not present for inertial element, using default (Identity)");
    this->origin.clear();
  }
  else
  {
    if (!this->origin.initXml(o))
    {
    	//   ROS_ERROR("Inertial has a malformed origin tag");
      this->origin.clear();
      return false;
    }
  }

  TiXmlElement *mass_xml = config->FirstChildElement("mass");
  if (!mass_xml)
  {
	  // ROS_ERROR("Inertial element must have mass element");
    return false;
  }
  if (!mass_xml->Attribute("value"))
  {
	  // ROS_ERROR("Inertial: mass element must have value attributes");
    return false;
  }

  try
  {
    mass = boost::lexical_cast<double>(mass_xml->Attribute("value"));
  }
  catch (boost::bad_lexical_cast &e)
  {
	  // ROS_ERROR("mass (%s) is not a float",mass_xml->Attribute("value"));
    return false;
  }

  TiXmlElement *inertia_xml = config->FirstChildElement("inertia");
  if (!inertia_xml)
  {
	  // ROS_ERROR("Inertial element must have inertia element");
    return false;
  }
  if (!(inertia_xml->Attribute("ixx") && inertia_xml->Attribute("ixy") && inertia_xml->Attribute("ixz") &&
        inertia_xml->Attribute("iyy") && inertia_xml->Attribute("iyz") &&
        inertia_xml->Attribute("izz")))
  {
	  //ROS_ERROR("Inertial: inertia element must have ixx,ixy,ixz,iyy,iyz,izz attributes");
    return false;
  }
  try
  {
    ixx  = boost::lexical_cast<double>(inertia_xml->Attribute("ixx"));
    ixy  = boost::lexical_cast<double>(inertia_xml->Attribute("ixy"));
    ixz  = boost::lexical_cast<double>(inertia_xml->Attribute("ixz"));
    iyy  = boost::lexical_cast<double>(inertia_xml->Attribute("iyy"));
    iyz  = boost::lexical_cast<double>(inertia_xml->Attribute("iyz"));
    izz  = boost::lexical_cast<double>(inertia_xml->Attribute("izz"));
  }
  catch (boost::bad_lexical_cast &e)
  {
	  //ROS_ERROR("one of the inertia elements: ixx (%s) ixy (%s) ixz (%s) iyy (%s) iyz (%s) izz (%s) is not a valid double",
//              inertia_xml->Attribute("ixx"),
//              inertia_xml->Attribute("ixy"),
//              inertia_xml->Attribute("ixz"),
//              inertia_xml->Attribute("iyy"),
//              inertia_xml->Attribute("iyz"),
//              inertia_xml->Attribute("izz"));
    return false;
  }

  return true;
}

bool Visual::initXml(TiXmlElement *config)
{
  this->clear();

  // Origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (!o)
  {
	  //ROS_DEBUG("Origin tag not present for visual element, using default (Identity)");
    this->origin.clear();
  }
  else if (!this->origin.initXml(o))
  {
	  // ROS_ERROR("Visual has a malformed origin tag");
    this->origin.clear();
    return false;
  }

  // Geometry
  TiXmlElement *geom = config->FirstChildElement("geometry");
  geometry = parseGeometry(geom);
  if (!geometry)
  {
	  // ROS_ERROR("Malformed geometry for Visual element");
    return false;
  }

  // Material
  TiXmlElement *mat = config->FirstChildElement("material");
  if (!mat)
  {
	  //  ROS_DEBUG("visual element has no material tag.");
  }
  else
  {
    // get material name
    if (!mat->Attribute("name"))
    {
    	//   ROS_ERROR("Visual material must contain a name attribute");
      return false;
    }
    this->material_name = mat->Attribute("name");

    // try to parse material element in place
    this->material.reset(new Material);
    if (!this->material->initXml(mat))
    {
    	//   ROS_DEBUG("Could not parse material element in Visual block, maybe defined outside.");
      this->material.reset();
    }
    else
    {
    	//   ROS_DEBUG("Parsed material element in Visual block.");
    }
  }

  // Group Tag (optional)
  // collision blocks without a group tag are designated to the "default" group
  const char *group_name_char = config->Attribute("group");
  if (!group_name_char)
    group_name = std::string("default");
  else
    group_name = std::string(group_name_char);

  return true;
}

bool Collision::initXml(TiXmlElement* config)
{
  this->clear();

  // Origin
  TiXmlElement *o = config->FirstChildElement("origin");
  if (!o)
  {
	  //  ROS_DEBUG("Origin tag not present for collision element, using default (Identity)");
    this->origin.clear();
  }
  else if (!this->origin.initXml(o))
  {
	  //  ROS_ERROR("Collision has a malformed origin tag");
    this->origin.clear();
    return false;
  }

  // Geometry
  TiXmlElement *geom = config->FirstChildElement("geometry");
  geometry = parseGeometry(geom);
  if (!geometry)
  {
	  //  ROS_ERROR("Malformed geometry for Collision element");
    return false;
  }

  // Group Tag (optional)
  // collision blocks without a group tag are designated to the "default" group
  const char *group_name_char = config->Attribute("group");
  if (!group_name_char)
    group_name = std::string("default");
  else
    group_name = std::string(group_name_char);

  return true;
}

bool Sphere::initXml(TiXmlElement *c)
{
  this->clear();

  this->type = SPHERE;
  if (!c->Attribute("radius"))
  {
	  //  ROS_ERROR("Sphere shape must have a radius attribute");
    return false;
  }

  try
  {
    radius = boost::lexical_cast<double>(c->Attribute("radius"));
  }
  catch (boost::bad_lexical_cast &e)
  {
	  //  ROS_ERROR("radius (%s) is not a valid float",c->Attribute("radius"));
    return false;
  }

  return true;
}

bool Box::initXml(TiXmlElement *c)
{
  this->clear();

  this->type = BOX;
  if (!c->Attribute("size"))
  {
	  //  ROS_ERROR("Box shape has no size attribute");
    return false;
  }
  if (!dim.init(c->Attribute("size")))
  {
	  //   ROS_ERROR("Box shape has malformed size attribute");
    dim.clear();
    return false;
  }
  return true;
}

bool Cylinder::initXml(TiXmlElement *c)
{
  this->clear();

  this->type = CYLINDER;
  if (!c->Attribute("length") ||
      !c->Attribute("radius"))
  {
	  //   ROS_ERROR("Cylinder shape must have both length and radius attributes");
    return false;
  }

  try
  {
    length = boost::lexical_cast<double>(c->Attribute("length"));
  }
  catch (boost::bad_lexical_cast &e)
  {
	  //  ROS_ERROR("length (%s) is not a valid float",c->Attribute("length"));
    return false;
  }

  try
  {
    radius = boost::lexical_cast<double>(c->Attribute("radius"));
  }
  catch (boost::bad_lexical_cast &e)
  {
	  //   ROS_ERROR("radius (%s) is not a valid float",c->Attribute("radius"));
    return false;
  }

  return true;
}

bool Mesh::fileExists(std::string filename)
{
//  std::string fullname = filename;
//  if (fullname.find("package://") == 0)
//  {
//    fullname.erase(0, strlen("package://"));
//    size_t pos = fullname.find("/");
//    if (pos == std::string::npos)
//    {
//    	//   ROS_FATAL("Could not parse package:// format for [%s]",filename.c_str());
//    }
//
//    std::string package = fullname.substr(0, pos);
//    fullname.erase(0, pos);
//    //std::string package_path = ros::package::getPath(package);
//
//    if (package_path.empty())
//    {
//    	//    ROS_FATAL("%s Package[%s] does not exist",filename.c_str(),package.c_str());
//      return false;
//    }
//
//    fullname = package_path + fullname;
//  }
//  std::ifstream fin; fin.open(fullname.c_str(), std::ios::in); fin.close();
//  if (fin.fail()) {
//	  //  ROS_FATAL("Mesh [%s] does not exist",filename.c_str());
//    return false;
//  }


  return true;
}

bool Mesh::initXml(TiXmlElement *c)
{
  this->clear();

  this->type = MESH;
  if (!c->Attribute("filename"))
  {
	  //  ROS_ERROR("Mesh must contain a filename attribute");
    return false;
  }

  filename = c->Attribute("filename");

  // check if filename exists, is this really necessary?
  if (!fileExists(filename))
  {
    printf("filename referred by mesh [%s] does not appear to exist. \n");
    //ROS_WARN("filename referred by mesh [%s] does not appear to exist.",filename.c_str());
  }
  if (c->Attribute("scale"))
  {
    if (!this->scale.init(c->Attribute("scale")))
    {
    	//   ROS_ERROR("Mesh scale was specified, but could not be parsed");
      this->scale.clear();
      return false;
    }
  }
  else
	  printf("Mesh scale was not specified, default to (1,1,1)");
    //ROS_DEBUG("Mesh scale was not specified, default to (1,1,1)");

  return true;
}


bool Link::initXml(TiXmlElement* config)
{
  this->clear();

  const char *name_char = config->Attribute("name");
  if (!name_char)
  {
	  //  ROS_ERROR("No name given for the link.");
    return false;
  }
  name = std::string(name_char);

  // Inertial (optional)
  TiXmlElement *i = config->FirstChildElement("inertial");
  if (i)
  {
    inertial.reset(new Inertial);
    if (!inertial->initXml(i))
    {
    	//    ROS_ERROR("Could not parse inertial element for Link '%s'", this->name.c_str());
      return false;
    }
  }

  // Multiple Visuals (optional)
  for (TiXmlElement* vis_xml = config->FirstChildElement("visual"); vis_xml; vis_xml = vis_xml->NextSiblingElement("visual"))
  {
    boost::shared_ptr<Visual> vis;
    vis.reset(new Visual);

    if (vis->initXml(vis_xml))
    {
      boost::shared_ptr<std::vector<boost::shared_ptr<Visual > > > viss = this->getVisuals(vis->group_name);
      if (!viss)
      {
        // group does not exist, create one and add to map
        viss.reset(new std::vector<boost::shared_ptr<Visual > >);
        // new group name, create vector, add vector to map and add Visual to the vector
        this->visual_groups.insert(make_pair(vis->group_name,viss));
        //     ROS_DEBUG("successfully added a new visual group name '%s'",vis->group_name.c_str());
      }

      // group exists, add Visual to the vector in the map
      viss->push_back(vis);
      //   ROS_DEBUG("successfully added a new visual under group name '%s'",vis->group_name.c_str());
    }
    else
    {
    	//  ROS_ERROR("Could not parse visual element for Link '%s'", this->name.c_str());
      vis.reset();
      return false;
    }
  }
  // Visual (optional)
  // Assign one single default visual pointer from the visual_groups map
  this->visual.reset();
  boost::shared_ptr<std::vector<boost::shared_ptr<Visual > > > default_visual = this->getVisuals("default");
  if (!default_visual)
  {
	  //  ROS_DEBUG("No 'default' visual group for Link '%s'", this->name.c_str());
  }
  else if (default_visual->empty())
  {
	  //   ROS_DEBUG("'default' visual group is empty for Link '%s'", this->name.c_str());
  }
  else
  {
    if (default_visual->size() > 1)
      printf("'default' visual group has %d visuals for Link '%s', taking the first one as default",(int)default_visual->size(), this->name.c_str());
    	//    ROS_WARN("'default' visual group has %d visuals for Link '%s', taking the first one as default",(int)default_visual->size(), this->name.c_str());
    this->visual = (*default_visual->begin());
  }



  // Multiple Collisions (optional)
  for (TiXmlElement* col_xml = config->FirstChildElement("collision"); col_xml; col_xml = col_xml->NextSiblingElement("collision"))
  {
    boost::shared_ptr<Collision> col;
    col.reset(new Collision);

    if (col->initXml(col_xml))
    {
      boost::shared_ptr<std::vector<boost::shared_ptr<Collision > > > cols = this->getCollisions(col->group_name);
      if (!cols)
      {
        // group does not exist, create one and add to map
        cols.reset(new std::vector<boost::shared_ptr<Collision > >);
        // new group name, create vector, add vector to map and add Collision to the vector
        this->collision_groups.insert(make_pair(col->group_name,cols));
        //     ROS_DEBUG("successfully added a new collision group name '%s'",col->group_name.c_str());
      }

      // group exists, add Collision to the vector in the map
      cols->push_back(col);
      //   ROS_DEBUG("successfully added a new collision under group name '%s'",col->group_name.c_str());
    }
    else
    {
    	//    ROS_ERROR("Could not parse collision element for Link '%s'", this->name.c_str());
      col.reset();
      return false;
    }
  }

  // Collision (optional)
  // Assign one single default collision pointer from the collision_groups map
  this->collision.reset();
  boost::shared_ptr<std::vector<boost::shared_ptr<Collision > > > default_collision = this->getCollisions("default");
  if (!default_collision)
  {
	  //   ROS_DEBUG("No 'default' collision group for Link '%s'", this->name.c_str());
  }
  else if (default_collision->empty())
  {
	  //   ROS_DEBUG("'default' collision group is empty for Link '%s'", this->name.c_str());
  }
  else
  {
    if (default_collision->size() > 1)
      printf("'default' collision group has %d collisions for Link '%s', taking the first one as default",(int)default_collision->size(), this->name.c_str());
    	//     ROS_WARN("'default' collision group has %d collisions for Link '%s', taking the first one as default",(int)default_collision->size(), this->name.c_str());
    this->collision = (*default_collision->begin());
  }

  return true;
}

void Link::addVisual(std::string group_name, boost::shared_ptr<Visual> visual)
{
  boost::shared_ptr<std::vector<boost::shared_ptr<Visual > > > viss = this->getVisuals(group_name);
  if (!viss)
  {
    // group does not exist, create one and add to map
    viss.reset(new std::vector<boost::shared_ptr<Visual > >);
    // new group name, create vector, add vector to map and add Visual to the vector
    this->visual_groups.insert(make_pair(group_name,viss));
    //   ROS_DEBUG("successfully added a new visual group name '%s'",group_name.c_str());
  }

  // group exists, add Visual to the vector in the map
  std::vector<boost::shared_ptr<Visual > >::iterator vis_it = find(viss->begin(),viss->end(),visual);
  if (vis_it != viss->end())
    printf("attempted to add a visual that already exists under group name '%s', skipping.",group_name.c_str());
    //   ROS_WARN("attempted to add a visual that already exists under group name '%s', skipping.",group_name.c_str());
  else
    viss->push_back(visual);
  // ROS_DEBUG("successfully added a new visual under group name '%s'",group_name.c_str());

}

boost::shared_ptr<std::vector<boost::shared_ptr<Visual > > > Link::getVisuals(const std::string& group_name) const
{
  boost::shared_ptr<std::vector<boost::shared_ptr<Visual > > > ptr;
  if (this->visual_groups.find(group_name) == this->visual_groups.end())
    ptr.reset();
  else
    ptr = this->visual_groups.find(group_name)->second;
  return ptr;
}


void Link::addCollision(std::string group_name, boost::shared_ptr<Collision> collision)
{
  boost::shared_ptr<std::vector<boost::shared_ptr<Collision > > > viss = this->getCollisions(group_name);
  if (!viss)
  {
    // group does not exist, create one and add to map
    viss.reset(new std::vector<boost::shared_ptr<Collision > >);
    // new group name, create vector, add vector to map and add Collision to the vector
    this->collision_groups.insert(make_pair(group_name,viss));
    //   ROS_DEBUG("successfully added a new collision group name '%s'",group_name.c_str());
  }

  // group exists, add Collision to the vector in the map
  std::vector<boost::shared_ptr<Collision > >::iterator vis_it = find(viss->begin(),viss->end(),collision);
  if (vis_it != viss->end())
    printf("attempted to add a collision that already exists under group name '%s', skipping.",group_name.c_str());
    //   ROS_WARN("attempted to add a collision that already exists under group name '%s', skipping.",group_name.c_str());
  else
    viss->push_back(collision);
  // ROS_DEBUG("successfully added a new collision under group name '%s'",group_name.c_str());

}

boost::shared_ptr<std::vector<boost::shared_ptr<Collision > > > Link::getCollisions(const std::string& group_name) const
{
  boost::shared_ptr<std::vector<boost::shared_ptr<Collision > > > ptr;
  if (this->collision_groups.find(group_name) == this->collision_groups.end())
    ptr.reset();
  else
    ptr = this->collision_groups.find(group_name)->second;
  return ptr;
}

void Link::setParent(boost::shared_ptr<Link> parent)
{
  this->parent_link_ = parent;
  //ROS_DEBUG("set parent Link '%s' for Link '%s'", parent->name.c_str(), this->name.c_str());
}

void Link::setParentJoint(boost::shared_ptr<Joint> parent)
{
  this->parent_joint = parent;
  //ROS_DEBUG("set parent joint '%s' to Link '%s'",  parent->name.c_str(), this->name.c_str());
}

void Link::addChild(boost::shared_ptr<Link> child)
{
  this->child_links.push_back(child);
  //ROS_DEBUG("added child Link '%s' to Link '%s'",  child->name.c_str(), this->name.c_str());
}

void Link::addChildJoint(boost::shared_ptr<Joint> child)
{
  this->child_joints.push_back(child);
  //ROS_DEBUG("added child Joint '%s' to Link '%s'", child->name.c_str(), this->name.c_str());
}



}

