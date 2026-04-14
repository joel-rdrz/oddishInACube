#version 330 core

layout(location=0) out vec4 color;

in vec3 normalToCamera;
in vec3 cameraSpacePos;
in vec2 texCoord;
in vec4 lightView_Position;

uniform vec3 lightPos; 
uniform vec3 lightPoints[4];

uniform sampler2D shadow;
uniform sampler2D tex;
uniform sampler2D ltc1;
uniform sampler2D ltc2;

const float roughness = 0.45; 
const float intensity = 200.0;
const vec3 lightColor = vec3(0.0, 1.0, 0.8);

const float LUT_SCALE = (64.0 - 1.0) / 64.0;
const float LUT_BIAS  = 0.5 / 64.0;

// This function directly integrates the "meat" equation 11 from the paper. Our area light is located in the upper hemisphere in order to match the assumption in the paper.
vec3 IntegrateEdgeVector(vec3 v1, vec3 v2) {
    float cosTheta = dot(v1, v2);
    cosTheta = clamp(cosTheta, -0.9999, 0.9999); 
    
    float theta = acos(cosTheta);
    vec3 crossV = cross(v1, v2);
    float lengthCrossV = length(crossV);
    
    if (lengthCrossV < 0.0001) return vec3(0.0);
    
    return (theta / lengthCrossV) * crossV;
}

float LTC_Evaluate(vec3 N, vec3 V, vec3 P, mat3 Minv, vec3 points[4], bool twoSided) {
    // From Section 5.4. We first construct an orthonormal basis (T1, T2, N) where T1 is the projection of the view vector V onto the plane of the surface. If V is parallel to N, we just pick an arbitrary T1 that is perpendicular to N. We move from world space to the LTC coordinates.
    vec3 T1, T2;
    vec3 T1_dir = V - N * dot(V, N);
    
    if (length(T1_dir) > 1e-5) {
        T1 = normalize(T1_dir);
    } else {
        vec3 up;
        if (abs(N.z) < 0.999) {
            up = vec3(0.0, 0.0, 1.0);
        } else {
            up = vec3(1.0, 0.0, 0.0);
        }
        T1 = normalize(cross(up, N));
    }
    
    T2 = cross(N, T1);
    mat3 B = mat3(T1, T2, N);

    // This math integrates Section 3.2 of the paper: "The integral of an LSTD D over a polygon P is the integral of the original distribution D_o over the transformed polygon P_o = M^-1 * P"
    // We transform the 4 vertices of our area light into the LTC's local space by applying the inverse matrix M^-1 and the change of basis B^-1 = transpose(B). We move each point relative to the pixel
    vec3 L[4];
    L[0] = Minv * (transpose(B) * (points[0] - P));
    L[1] = Minv * (transpose(B) * (points[1] - P));
    L[2] = Minv * (transpose(B) * (points[2] - P));
    L[3] = Minv * (transpose(B) * (points[3] - P));

    
    // This math does the summation part of equation (11) in the paper. We loop through each edge of the polygon and integrate the contribution of that edge to the final result using the IntegrateEdgeVector function we defined above.
    int n = 4;
    vec3 m = vec3(0.0);
    vec3 prev = normalize(L[n - 1]);
    
    for (int i = 0; i < n; i++) {
        vec3 curr = normalize(L[i]);
        m += IntegrateEdgeVector(prev, curr);
        prev = curr;
    }

    float res = m.z;
    if (!twoSided && res < 0.0) return 0.0;
    return abs(res) / (2.0 * 3.14159); // Finally divide by 2 * pi
}

void main() {
    vec3 N = normalize(normalToCamera);
    vec3 V = normalize(-cameraSpacePos);
    vec3 P = cameraSpacePos;
    float NdotV = clamp(dot(N, V), 0.0, 1.0);

    vec2 index = vec2(roughness, sqrt(1.0 - NdotV));
    index = index * LUT_SCALE + LUT_BIAS;

    vec4 t1 = texture(ltc1, index);
    vec4 t2 = texture(ltc2, index);

    // Construct Matrix M similar to how the paper did in Section 4.
    mat3 Minv = mat3(
        vec3(t1.x, 0, t1.y),
        vec3(  0,  1,    0),
        vec3(t1.z, 0, t1.w)
    );

    // We flip vertices so light points down at oddish instead of up
    vec3 correctLightPoints[4];
    correctLightPoints[0] = lightPoints[0];
    correctLightPoints[1] = lightPoints[3];
    correctLightPoints[2] = lightPoints[2];
    correctLightPoints[3] = lightPoints[1];

    
    vec3 baseReflectivity = vec3(0.04); 
    float specEval = LTC_Evaluate(N, V, P, Minv, correctLightPoints, false);
    vec3 specular = vec3(specEval) * (baseReflectivity * t2.x + (1.0 - baseReflectivity) * t2.y); 

    mat3 identity = mat3(1.0);
    float diffEval = LTC_Evaluate(N, V, P, identity, correctLightPoints, false);
    vec3 diffuse = vec3(diffEval);

    vec3 textureColor = pow(texture(tex, texCoord).rgb, vec3(2.2));
    vec3 lightDir = normalize(lightPos - P);
    float bias = max(0.05 * (1.0 - dot(N, lightDir)), 0.005);

    vec3 projCoords = lightView_Position.xyz / lightView_Position.w;
    float shadowDepth = texture(shadow, projCoords.xy).r; 
    float shadowTerm = (projCoords.z - bias <= shadowDepth) ? 1.0 : 0.0;

    vec3 ambient = textureColor * 0.05; 
    
    vec3 directLight = intensity * lightColor * ((textureColor * diffuse) + specular);    
    vec3 finalColor = ambient + (shadowTerm * directLight);
    
    finalColor = finalColor / (finalColor + vec3(1.0)); 

    finalColor = pow(finalColor, vec3(1.0 / 2.2)); 

    color = vec4(finalColor, 1.0);
}