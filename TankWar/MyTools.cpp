#include "MyTools.h"

void SplitString(const std::string& source, std::vector<std::string>& output, const std::string& splitBy)
{
	output.clear();
	std::string::size_type pos1, pos2;
	pos2 = source.find(splitBy);
	pos1 = 0;
	while (std::string::npos != pos2)
	{
		output.push_back(source.substr(pos1, pos2 - pos1));

		pos1 = pos2 + splitBy.size();
		pos2 = source.find(splitBy, pos1);
	}
	if (pos1 != source.length())
		output.push_back(source.substr(pos1));
}
