/*
# MIT License

# Copyright(c) 2018-2019 NovusCore

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files(the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions :

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
*/
#pragma once
#include <Utils/DebugHandler.h>
#include <sstream>
#include "../MPQ/MPQHandler.h"
#include "DBCReader.h"
#include "DBCStructures.h"

namespace DBCLoader
{
	std::string EscapeString(std::string const& s)
	{
		std::size_t n = s.length();
		std::string escaped;
		escaped.reserve(n * 2);        // pessimistic preallocation

		for (std::size_t i = 0; i < n; ++i) {
			if (s[i] == '\\' || s[i] == '\'')
				escaped += '\\';
			escaped += s[i];
		}
		return escaped;
	}

	bool LoadMap(MPQHandler& handler, std::string& output)
	{
		MPQFile file;
		if (handler.GetFile("DBFilesClient\\Map.dbc", file))
		{
			if (DBCReader* dbcReader = DBCReader::GetReader())
			{
				if (dbcReader->Load(file.Buffer) == 0)
				{
					u32 rows = dbcReader->GetNumRows();
					if (rows == 0) return false;

					std::stringstream ss;
					ss << "DELETE FROM map; INSERT INTO map(id, internalName, instanceType, flags, name, expansion, maxPlayers) VALUES";

					for (u32 i = 0; i < rows; i++)
					{
						auto& row = dbcReader->GetRow(i);
						u32 fieldCount = dbcReader->GetNumFields();
						DBCMap map;
						map.Id = row.GetUInt32(0);
						map.InternalName = EscapeString(row.GetString(row.GetUInt32(1)));
						map.InstanceType = row.GetUInt32(2);
						map.Flags = row.GetUInt32(3);
						map.Name = EscapeString(row.GetString(row.GetUInt32(5)));
						map.Expansion = row.GetUInt32(63);
						map.MaxPlayers = row.GetUInt32(65);

						if (i != 0)
							ss << ", ";
						
						ss << "(" << map.Id << ", '" << map.InternalName << "', " << map.InstanceType << ", " << map.Flags << ", '" << map.Name << "', " << map.Expansion << ", " << map.MaxPlayers << ")";
					}

					ss << ";";
					output += ss.str();
				}
			}
		}

		return true;
	}
}