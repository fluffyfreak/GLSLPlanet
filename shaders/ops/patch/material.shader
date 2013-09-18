/*
    :copyright: 2010 by Florian Boesch <pyalot@gmail.com>.
    :license: GNU AGPL v3 or later, see LICENSE for more details.
*/

fragment:
    uniform sampler2D color;
    uniform sampler2D light;

    void main(void){
        vec2 pos = gl_TexCoord[0].st; 
        gl_FragColor = texture2D(color, pos) * texture2D(light, pos);
    }
