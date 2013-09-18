/*
    :copyright: 2010 by Florian Boesch <pyalot@gmail.com>.
    :license: GNU AGPL v3 or later, see LICENSE for more details.
*/

//#version 120 core

uniform sampler2D heightfield;
uniform sampler2D blurred;
uniform float scale;

void main(void){
    vec2 pos = gl_TexCoord[0].st; 

    gl_FragColor = ((texture2D(heightfield, pos) - texture2D(blurred, pos)) * scale)+1.0;
}
