#ifndef __AMY_DETAIL_VALUE_CAST_HPP__
#define __AMY_DETAIL_VALUE_CAST_HPP__

#include <amy/sql_types.hpp>

#include <iomanip>
#include <string>
#include <sstream>
#include <string>

namespace amy {
namespace detail {

template<typename ValueType>
ValueType value_cast(const char* s, unsigned long l) {
    ValueType v{};
    std::istringstream is({s, l});
    is >> v;
    return v;
}

template<>
inline std::string value_cast(const char* s, unsigned long l) {
    return {s, l};
}

template<>
inline long long value_cast(const char* s, unsigned long) {
    return std::strtoll(s, NULL, 10);
}

template<>
inline long value_cast(const char* s, unsigned long) {
    return std::strtol(s, NULL, 10);
}

template<>
inline int32_t value_cast(const char* s, unsigned long) {
    return static_cast<int32_t>(std::strtol(s, NULL, 10));
}

template<>
inline int16_t value_cast(const char* s, unsigned long) {
    return static_cast<int16_t>(std::strtol(s, NULL, 10));
}

template<>
inline int8_t value_cast(const char* s, unsigned long) {
    return static_cast<int8_t>(std::strtol(s, NULL, 10));
}

template<>
inline bool value_cast(const char* s, unsigned long) {
    return static_cast<bool>(std::strtol(s, NULL, 10));
}

template<>
inline unsigned long long value_cast(const char* s, unsigned long) {
    return std::strtoull(s, NULL, 10);
}

template<>
inline unsigned long value_cast(const char* s, unsigned long) {
    return std::strtoul(s, NULL, 10);
}

template<>
inline uint32_t value_cast(const char* s, unsigned long) {
    return static_cast<uint32_t>(std::strtoul(s, NULL, 10));
}

template<>
inline uint16_t value_cast(const char* s, unsigned long) {
    return static_cast<int16_t>(std::strtoul(s, NULL, 10));
}

template<>
inline uint8_t value_cast(const char* s, unsigned long) {
    return static_cast<uint8_t>(std::strtoul(s, NULL, 10));
}

template<>
inline double value_cast(const char* s, unsigned long) {
    return std::strtod(s, NULL);
}

template<>
inline float value_cast(const char* s, unsigned long) {
    return std::strtof(s, NULL);
}

inline std::vector<std::string> split(const std::string &s, char delim) {
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;
	while (std::getline(ss, item, delim)) {
		elems.push_back(std::move(item)); // if C++11 (based on comment from @mchiasson)
	}
	return elems;
}

template<>
inline sql_datetime value_cast(const char* s, unsigned long l) {
	std::string str = { s, l };

	std::vector<std::string> dateTime = split(str, ' ');
	std::string date = dateTime[0];
	std::string time = dateTime[1];

	std::vector<std::string> dateSplit = split(date, '-');
	std::vector<std::string> timeSplit = split(time, ':');

	std::tm tm;
	tm.tm_year = stoi(dateSplit[0]) - 1900; // Year since 1900
	tm.tm_mon = stoi(dateSplit[1]); // Month of the year
	tm.tm_mday = stoi(dateSplit[2]); // Day of the month

	tm.tm_hour = stoi(timeSplit[0]); // Hour of the day
	tm.tm_min = stoi(timeSplit[1]); // Minute of the hour
	tm.tm_sec = stoi(timeSplit[2]); // Second of the minute

    return sql_datetime(mktime(&tm));
}

template<>
inline sql_time value_cast(const char* str, unsigned long len) {
    /*using namespace boost::posix_time;

    // It's a pity that Boost.Date_Time cannot correctly parse negative time
    // durations, so we have to deal with the sign here.
    assert(len > 0);
    bool negative = (str[0] == '-') ? true : false;
    if (negative) {
        ++str;
        --len;
    }

    time_input_facet* input_facet = new time_input_facet();
    input_facet->time_duration_format("%H:%M:%S:%F");

    std::istringstream in({str, len});
    in.unsetf(std::ios::skipws);
    in.imbue(std::locale(std::locale::classic(), input_facet));

    sql_time v;
    in >> v;

    if (negative) {
        v = hours(0) - v;
    }*/
	assert(0, "NOT IMPLEMENTED YET");
	sql_time* v = new sql_time();
    return *v;
}

} // namespace detail
} // namespace amy

#endif // __AMY_DETAIL_VALUE_CAST_HPP__

// vim:ft=cpp sw=4 ts=4 tw=80 et
