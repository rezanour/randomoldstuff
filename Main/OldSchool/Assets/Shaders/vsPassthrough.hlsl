//==============================================================================
//  Passthrough vertex shader that just passes the post-projection position on.
//==============================================================================

float4 main(float4 position : POSITION) : SV_POSITION
{
    return position;
}
