#if defined _inc_impl
	#undef _inc_impl
#endif

stock bool:IsFlagged(flags, flagValue) {
    if ((flags & flagValue) == flagValue) {
        return true;
    }
    return false;
}

stock Float:FloatOffset(Float:f1, Float:f2)
{   
    if(f1 >= f2) return f1 - f2;
    else return (f2 - f1);
}

stock Float:GetPointDistanceToPoint(Float:x1,Float:y1,Float:x2,Float:y2)
{
	new Float:x, Float:y;
	x = x1-x2;
	y = y1-y2;
	return floatsqroot(x*x+y*y);
}


stock Float:Distance(Float:x1, Float:y1, Float:z1, Float:x2, Float:y2, Float:z2) {
    return VectorSize(x1 - x2, y1 - y2, z1 - z2);
}


stock StripColorEmbedding(string[], len = sizeof string)
{
 	new i, tmp[7];
  	while (i < len - 7)
	{
	    if (string[i] == '{' && string[i + 7] == '}')
		{
		    strmid(tmp, string, i + 1, i + 7);
			if (ishex(tmp))
			{
				strdel(string, i, i + 8);
				i = 0;
				continue;
			}
		}
		i++;
  	}
}