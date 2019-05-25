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
#include <Utils/StringUtils.h>
#include <sstream>
#include "../MPQ/MPQHandler.h"
#include "DBCReader.h"
#include "DBCStructures.h"

namespace DBCLoader
{
	bool LoadMap(MPQHandler& handler, std::string& sqlOutput, std::vector<std::string>& adtLocationOutput)
	{
		MPQFile file;
		if (handler.GetFile("DBFilesClient\\Map.dbc", file))
		{
			NC_LOG_MESSAGE("Loading Map.dbc...");
			if (DBCReader* dbcReader = DBCReader::GetReader())
			{
				if (dbcReader->Load(file.Buffer) == 0)
				{
					u32 rows = dbcReader->GetNumRows();
					if (rows == 0) return false;

					std::stringstream ss;
					ss << "DELETE FROM map;" << std::endl << "INSERT INTO map(id, internalName, instanceType, flags, name, expansion, maxPlayers) VALUES";

					for (u32 i = 0; i < rows; i++)
					{
						auto row = dbcReader->GetRow(i);
						//u32 fieldCount = dbcReader->GetNumFields();
						DBCMap map;
						map.Id = row.GetUInt32(0);
						map.InternalName = StringUtils::EscapeString(row.GetString(row.GetUInt32(1)));
						map.InstanceType = row.GetUInt32(2);
						map.Flags = row.GetUInt32(3);
						map.Name = StringUtils::EscapeString(row.GetString(row.GetUInt32(5)));
						map.Expansion = row.GetUInt32(63);
						map.MaxPlayers = row.GetUInt32(65);

						// MapFlag 2 & 16, seem to be exclusive to Test / Development Maps
						if ((map.Flags & 2) == 0 && (map.Flags & 16) == 0)
							adtLocationOutput.push_back(row.GetString(row.GetUInt32(1)));

						if (i != 0)
							ss << ", ";

						ss << "(" << map.Id << ", '" << map.InternalName << "', " << map.InstanceType << ", " << map.Flags << ", '" << map.Name << "', " << map.Expansion << ", " << map.MaxPlayers << ")";
					}

					ss << ";" << std::endl;
					sqlOutput += ss.str();
				}
			}
		}
		else
		{
			NC_LOG_ERROR("Failed to load Map.dbc");
		}

		return true;
	}

	bool LoadEmotes(MPQHandler& handler, std::string& sqlOutput)
	{
		MPQFile file;
		if (handler.GetFile("DBFilesClient\\Emotes.dbc", file))
		{
			NC_LOG_MESSAGE("Loading Emotes.dbc...");

			if (DBCReader* dbcReader = DBCReader::GetReader())
			{
                if (dbcReader->Load(file.Buffer) == 0)
                {
                    u32 rows = dbcReader->GetNumRows();
                    if (rows == 0) return false;

                    std::stringstream ss;
                    ss << "DELETE FROM emotes;" << std::endl << "INSERT INTO emotes(id, internalName, textEmoteId) VALUES";

                    for (u32 i = 0; i < rows; i++)
                    {
                        auto row = dbcReader->GetRow(i);

                        DBCEmote emote;
                        emote.Id = row.GetUInt32(0);
                        emote.InternalName = row.GetString(row.GetUInt32(1));
                        emote.TextEmoteId = row.GetUInt32(2);

                        if (i != 0)
                            ss << ", ";

                        ss << "(" << emote.Id << ", '" << emote.InternalName << "', " << emote.TextEmoteId << ")";
                    }

                    ss << ";" << std::endl;
                    sqlOutput += ss.str();
                }
			}
		}
		else
		{
			NC_LOG_ERROR("Failed to load Emotes.dbc");
		}

	return true;
	}
}
