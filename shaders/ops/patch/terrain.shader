/*
    :copyright: 2010 by Florian Boesch <pyalot@gmail.com>.
    :license: GNU AGPL v3 or later, see LICENSE for more details.
*/

fragment:
    uniform sampler2D normal;
    uniform sampler2D height;
    uniform float patch_size;

    void main(void){
        vec2 pos = gl_TexCoord[0].st; 
        gl_FragColor.rgb = texture2D(normal, pos).rgb;
        gl_FragColor.a = texture2D(height, pos).r/patch_size;
    }
