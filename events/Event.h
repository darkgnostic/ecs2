#pragma once

namespace wcs
{
	struct Event
	{
		explicit Event(const EventType et) : type(et) {}
		virtual ~Event() = default;
		EventType type;
	};
}
