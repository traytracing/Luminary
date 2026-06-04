#version 460 core

in vec2 f_UV; 
in flat vec4 f_color;


out vec4 FragColor;

void main() {
    float dist = length(f_UV);
    if (dist > 1) discard; 
    
    //FragColor = glm::vec4(f_color.xyz *  (dist - 0.5) * f_color.q, (dist - 0.5) * f_color.q); 
    FragColor = f_color * (dist - 0.5); 
}