uniform sampler2D texture;
uniform float time;
uniform vec3 glowColor;
uniform float intensity;

void main()
{
    vec4 pixel = texture2D(texture, gl_TexCoord[0].xy);
    float glow = sin(time * 3.0) * 0.5 + 0.5;
    vec3 color = pixel.rgb + glowColor * glow * intensity;
    gl_FragColor = vec4(color, pixel.a);
}