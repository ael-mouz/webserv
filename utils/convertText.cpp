#include "../include/server.hpp"

std::string convertText(std::string a)
{
	std::string::size_type i = 0;
	std::string newbuff;
	while (i < a.length())
	{
		if (a[i] == '\r' && i + 1 < a.length() && a[i + 1] == '\n')
		{
			newbuff += "\e[41m\\r\\n\e[49m\n";
			i++;
		}
		else if (a[i] == '\t')
			newbuff += "\\t\t";
		else if (a[i] == '\v')
			newbuff += "\\v\v";
		else if (a[i] == '\f')
			newbuff += "\\f\f";
		else
			newbuff += a[i];
		i++;
	}
	return newbuff;
}
