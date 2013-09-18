uniform float height;
uniform vec2 offset;
uniform float zoom;

/*
void main(){
    vec2 uv = gl_TexCoord[0].xy;
    float x=0.0;
    float y=0.0;
    float xtemp=0.0;
    float x0=uv.x*3.5 - 2.5;
    float y0=uv.y*2.0 - 1.0;
    int i=0, iterations=256;

    while(x*x + y*y <= 4.0 && i<iterations){
        xtemp = x*x - y*y + x0;
        y = 2*x*y + y0;
        x = xtemp;
        i = i+1;
    }

    if(i==iterations){
        gl_FragColor = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else{
        float color = (float(i)/float(iterations)) / x;
        //float color = float(i)/float(iterations);
        gl_FragColor = vec4(color, color, color, 1.0);
    }
}
*/

void main(){
    vec2 uv = (gl_TexCoord[0].xy-0.5)*zoom+offset;
    float max_iter = 1024;
    float x = (uv.x+0.5)*3.5 - 2.5;
    float y = (uv.y+0.5)*2.0 - 1.0;

    float real = x;
    float imaginary = y;
    
    float tmp;
    float i=0;

    for(;i<max_iter && pow(real, 2) + pow(imaginary, 2) < 4; i++){
        tmp = pow(real, 2) - pow(imaginary, 2) + x;
        imaginary = imaginary * real * 2 + y;
        real = tmp;
    }
    
    float result;
    if(i>=max_iter){
        result = 0;
    }
    else{
        result = i - log(log(sqrt(real*real + imaginary*imaginary))) / log(2);
        //result = i/max_iter;
    }
    result = max(result/max_iter, 0.0);
    result *= height;
    gl_FragColor = vec4(result, result, result, 1.0);
}
