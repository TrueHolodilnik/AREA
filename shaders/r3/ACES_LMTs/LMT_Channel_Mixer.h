//=================================================================================================
//LMT - Channel Mixer
//For complex color effects
//=================================================================================================

void Channel_Mixer(inout float3 aces)
{
	float3 Channel_Mixer_r = float3(1.000, 0.000, 0.000);
	float3 Channel_Mixer_g = float3(0.000, 1.000, 0.000);
	float3 Channel_Mixer_b = float3(0.000, 0.000, 1.000);

	aces = float3(
	dot(aces, Channel_Mixer_r),
	dot(aces, Channel_Mixer_g),
	dot(aces, Channel_Mixer_b));
	
	aces = max(0.0, aces);
}
