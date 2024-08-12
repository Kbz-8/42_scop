#include <Graphics/Loaders/OBJ.h>
#include <Core/Logs.h>

#include <fstream>
#include <set>

namespace Scop
{
	ObjData LoadOBJFromFile(const std::filesystem::path& path)
	{
		if(!std::filesystem::exists(path))
			Error();
		char line[1024];
		std::string op;
		std::istringstream line_in;
		std::set<std::string> groups;
		groups.insert("default");

		ObjData data;

		while(in.good()){
			in.getline(line, 1023);
			line_in.clear();
			line_in.str(line);

			if(!(line_in >> op))
				continue;
			if(op == "v")
				line_in >> data.vertex >> data.vertex >> data.vertex;
			else if(op == "vt")
				line_in >> data.texCoord >> data.texCoord >> data.texCoord;
			else if(op == "vn")
				line_in >> data.normal >> data.normal >> data.normal;
			else if(op == "g"){
				groups.clear();
			while(line_in >> groups) ;
				groups.insert("default");
			}
			else if(op == "f") {
				std::vector<ObjModel::FaceVertex> list;
				while(line_in >> list) ;

				for(std::set<std::string>::const_iterator g = groups.begin(); g != groups.end(); ++g){
					ObjModel::FaceList & fl = data.faces[*g];
					fl.second.push_back(fl.first.size());
					fl.first.insert(fl.first.end(), list.begin(), list.end());
				}
			}
		}
		for(std::map<std::string, ObjModel::FaceList>::iterator g = data.faces.begin(); g != data.faces.end(); ++g){
			ObjModel::FaceList & fl = g->second;
			fl.second.push_back(fl.first.size());
		}
		return data;
	}

	void TesselateOBJData(ObjData& data) noexcept
	{
	}
}
