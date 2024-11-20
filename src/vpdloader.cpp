#include "vpdloader.hpp"
#include "shiftjisconvert.hpp"

namespace democollection
{
	static std::string GetNextLine(std::ifstream& infile)
	{
		while (!infile.eof())
		{
			std::string line;
			for (int ch = infile.get(); ch != '\r' && ch != '\n' && !infile.eof(); ch = infile.get())
				line.push_back(ch);
			if (line.empty())
				continue;
			bool hasContent = false;
			const size_t lastCharToCheck = std::min(line.length(), line.find_first_of("//"));
			for (size_t i = 0; i < lastCharToCheck; ++i)
			{
				if (!std::isspace(line[i]))
				{
					hasContent = true;
					break;
				}
			}
			if (hasContent)
				return line;
		}
		return {};
	}

	VpdLoader::VpdLoader()
	{}

	VpdLoader::VpdLoader(const char filename[])
	{
		Read(filename);
	}

	VpdLoader::Status VpdLoader::Read(const char filename[])
	{
		std::ifstream infile;
		infile.open(filename, std::ios::binary);
		if (!infile.is_open())
			return FileNotFound;
		std::string line;

		GetNextLine(infile);	// header
		GetNextLine(infile);	// parent file
		line = GetNextLine(infile);
		size_t boneCount = 0;
		for (const char* ch = &line[0]; *ch != ';'; ++ch)
		{
			if (!std::isdigit(*ch))
				return BoneCountError;
			boneCount = boneCount * 10 + *ch - '0';
		}
		m_bones.resize(boneCount);
		for (size_t i = 0; i < boneCount; ++i)
		{
			line = GetNextLine(infile);
			if (line.size() < 4 || 0 != memcmp(line.c_str(), "Bone", 4))
				return BoneListError;
			size_t boneNamePos = line.find('{');
			if (boneNamePos == std::string::npos)
				return BoneListError;
			Bone& bone = m_bones[i];
			bone.boneName = ConvertShiftJisToUtf8(std::string(line.cbegin() + boneNamePos + 1, line.cend()));
			{
				line = GetNextLine(infile);
				size_t index = 0, end = 0;
				bone.translation(0) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ',')
					return BoneValueError;
				bone.translation(1) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ',')
					return BoneValueError;
				bone.translation(2) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ';')
					return BoneValueError;
			}
			{
				line = GetNextLine(infile);
				size_t index = 0, end = 0;
				bone.rotation(0) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ',')
					return BoneValueError;
				bone.rotation(1) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ',')
					return BoneValueError;
				bone.rotation(2) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ',')
					return BoneValueError;
				bone.rotation(3) = std::stof(line.c_str() + index, &end);
				if (line[(index += end)++] != ';')
					return BoneValueError;
			}
			line = GetNextLine(infile);
			if (line.size() < 1 || line[0] != '}')
				return BoneListError;
		}

		return Ok;
	}
}