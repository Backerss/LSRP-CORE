#if defined _inc_impl
	#undef _inc_impl
#endif

stock GetVehicleNameByModel(modelid)
{
    new output[24];
    if (0 <= (modelid -= 400) < sizeof(gVehicleNames))
    strunpack(output, gVehicleNames[modelid], sizeof(output));
    return output;
}