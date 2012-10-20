// Copyright © 2008-2012 Pioneer Developers. See AUTHORS.txt for details
// Licensed under the terms of the GPL v3. See licenses/GPL-3.txt

#ifndef __SHADER_HEIGHTMAP_H__
#define __SHADER_HEIGHTMAP_H__

// Include GLM
#include <glm/glm.hpp>

float shader_heightmap_frag(const glm::vec2 &gl_FragCoord);
void setUniforms(const glm::vec3 &v0_, const glm::vec3 &v1_, const glm::vec3 &v2_, const glm::vec3 &v3_, const float fracStep_);

#endif // __SHADER_HEIGHTMAP_H__