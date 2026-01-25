struct VSInput {
    float2 position;
    float3 color;
    float2 textureCoordinates;
};

struct UniformBuffer {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};
ConstantBuffer<UniformBuffer> ubo;

struct VSOutput {
    float4 position : SV_Position;
    float3 color;
    float2 textureCoordinates;
};

[shader("vertex")]
VSOutput vertMain(VSInput input) {
    VSOutput output;
    output.position = mul(ubo.projection, mul(ubo.view, mul(ubo.model, float4(input.position, 0.0, 1.0))));
    output.color = input.color;
    output.textureCoordinates = input.textureCoordinates;

    return output;
}

Sampler2D texture;

[shader("fragment")]
float4 fragMain(VSOutput vertIn) : SV_Target {
    return texture.Sample(vertIn.textureCoordinates);
}