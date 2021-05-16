#if defined _inc_impl
	#undef _inc_impl
#endif

stock FormatMoney(number, name[], len = sizeof(name))
{
	format(name, len, "$%d", number >= 0 ? number : -number); //If the number is positive - we write it in the usual way. Otherwise, convert it to a positive number, adding a minus
	
	new value = strlen(name); // Write in "i" length of the string with the our number
 
	switch(value)
	{
		case 4..6: // If the number has passed from 4 up to 6 characters, add a delimiter
					strins(name, ",", value-3, len);
		case 7..9: // From 7 to 9 characters - two delimiters
					strins(name, ",", value-3, len), 
					strins(name, ",", value-6, len);
		case 10..12: // From 10 to 12 characters - three delimiters
					strins(name, ",", value-3, len), 
					strins(name, ",", value-6, len), 
					strins(name, ",", value-9, len);
	}	
	if (number < 0) strins(name, "-", 0, len); // If the number is negative, it return him minus

	return 0;
}