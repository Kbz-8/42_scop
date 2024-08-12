#include <Graphics/Loaders/OBJ.h>
#include <Core/Logs.h>

#include <fstream>
#include <set>

namespace Scop
{
	std::optional<ObjData> LoadOBJFromFile(const std::filesystem::path& path)
	{
		if(!std::filesystem::exists(path))
		{
			Error("OBJ loader : OBJ file does not exists; %", path);
			return std::nullopt;
		}
		char line[1024];
		std::string op;
		std::istringstream line_in;
		std::set<std::string> groups;
		groups.insert("default");

		std::ifstream in(path);

		ObjData data;

		while(in.good())
		{
			in.getline(line, 1023);
			line_in.clear();
			line_in.str(line);

			if(!(line_in >> op))
				continue;
			if(op == "v")
				line_in >> data.vertex >> data.vertex >> data.vertex;
			else if(op == "vt")
				line_in >> data.tex_coord >> data.tex_coord >> data.tex_coord;
			else if(op == "vn")
				line_in >> data.normal >> data.normal >> data.normal;
			else if(op == "g")
			{
				groups.clear();
				while(line_in >> groups);
				groups.insert("default");
			}
			else if(op == "f")
			{
				std::vector<ObjData::FaceVertex> list;
				while(line_in >> list);
				for(auto g = groups.begin(); g != groups.end(); ++g)
				{
					ObjData::FaceList & fl = data.faces[*g];
					fl.second.push_back(fl.first.size());
					fl.first.insert(fl.first.end(), list.begin(), list.end());
				}
			}
		}
		for(auto g = data.faces.begin(); g != data.faces.end(); ++g)
		{
			ObjData::FaceList & fl = g->second;
			fl.second.push_back(fl.first.size());
		}
		return data;
	}

	static void TesselateOBJData(std::vector<ObjData::FaceVertex>& input, std::vector<std::uint32_t>& input_start) noexcept
	{
		std::vector<ObjData::FaceVertex> output;
		std::vector<unsigned> output_start;
		output.reserve(input.size());
		output_start.reserve(input_start.size());
		for(auto s = input_start.begin(); s != input_start.end() - 1; ++s)
		{
			const std::uint32_t size = *(s + 1) - *s;
			if(size > 3)
			{
				const ObjData::FaceVertex & start_vertex = input[*s];
				for(std::size_t i = 1; i < size - 1; i++)
				{
					output_start.push_back(output.size());
					output.push_back(start_vertex);
					output.push_back(input[*s + i]);
					output.push_back(input[*s + i + 1]);
				}
			} 
			else
			{
				output_start.push_back(output.size());
				output.insert(output.end(), input.begin() + *s, input.begin() + *(s + 1));
			}
		}
		output_start.push_back(output.size());
		input.swap(output);
		input_start.swap(output_start);
	}

	void TesselateOBJData(ObjData& obj)
	{
		for(std::map<std::string, ObjData::FaceList>::iterator g = obj.faces.begin(); g != obj.faces.end(); g++)
		{
			ObjData::FaceList & fl = g->second;
			TesselateOBJData(fl.first, fl.second);
		}
	}
}
