/*
 * Copyright (c) 2024 Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <chrono>
#include <ctime>
#include <inttypes.h>
namespace modm
{

/// @ingroup modm_io
/// @{

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::year year)
{
	return s.printf("%04" PRIi16, int16_t(int(year)));
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::month month)
{
	static constexpr std::string_view map = "???JanFebMarAprMayJunJulAugSepOctNovDec";
	return s << map.substr((unsigned(month) <= 12 ? unsigned(month) : 0u) * 3, 3);
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::day day)
{
	return s.printf("%02" PRIu8, uint8_t(unsigned(day)));
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::weekday wd)
{
	static constexpr std::string_view map = "SunMonTueWedThuFriSat???";
	return s << map.substr((wd.c_encoding() < 7u ? wd.c_encoding() : 7u) * 3, 3);
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::weekday_indexed wdi)
{
	s << wdi.weekday();
	const auto index = wdi.index();
	if (1 <= index and index <= 5) return s << '[' << index << ']';
	return s << "[?]";
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::weekday_last wdl)
{
	return s << wdl.weekday() << "[last]";
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::month_day md)
{
	return s << md.month() << '/' << md.day();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::month_day_last mdl)
{
	return s << mdl.month() << "/last";
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::month_weekday mwd)
{
	return s << mwd.month() << '/' << mwd.weekday_indexed();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::month_weekday_last mwdl)
{
	return s << mwdl.month() << '/' << mwdl.weekday_last();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::year_month ym)
{
	return s << ym.year() << '/' << ym.month();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::year_month_day& ymd)
{
	return s.printf("%04" PRIi16 "-%02" PRIu8 "-%02" PRIu8,
		int16_t(int(ymd.year())), uint8_t(unsigned(ymd.month())), uint8_t(unsigned(ymd.day())));
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::year_month_day_last& ymdl)
{
	return s << ymdl.year() << '/' << ymdl.month_day_last();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::year_month_weekday& ymwd)
{
	return s << ymwd.year() << '/' << ymwd.month() << '/' << ymwd.weekday_indexed();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::year_month_weekday_last& ymwdl)
{
	return s << ymwdl.year() << '/' << ymwdl.month() << '/' << ymwdl.weekday_last();
}

template< class Duration >
modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::hh_mm_ss<Duration>& hms)
{
	s.printf("%02" PRIu8 ":%02" PRIu8 ":%02" PRIu8 ".%03" PRIu16,
			 uint8_t(hms.hours().count()), uint8_t(hms.minutes().count()), uint8_t(hms.seconds().count()),
			 uint16_t(std::chrono::duration_cast<std::chrono::milliseconds>(hms.subseconds()).count()));
	return s;
}
template< class Rep, class Period >
inline modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::duration<Rep, Period>& d)
{
	s << d.count();

	if constexpr (std::is_same_v<typename Period::type, std::atto>)			s << "as";
	if constexpr (std::is_same_v<typename Period::type, std::femto>)		s << "fs";
	if constexpr (std::is_same_v<typename Period::type, std::pico>)			s << "ps";
	if constexpr (std::is_same_v<typename Period::type, std::nano>)			s << "ns";
	if constexpr (std::is_same_v<typename Period::type, std::micro>)		s << "us";
	if constexpr (std::is_same_v<typename Period::type, std::milli>)		s << "ms";
	if constexpr (std::is_same_v<typename Period::type, std::centi>)		s << "cs";
	if constexpr (std::is_same_v<typename Period::type, std::deci>)			s << "ds";
	if constexpr (std::is_same_v<typename Period::type, std::ratio<1>>)		s << 's';
	if constexpr (std::is_same_v<typename Period::type, std::deca>)			s << "as";
	if constexpr (std::is_same_v<typename Period::type, std::hecto>)		s << "hs";
	if constexpr (std::is_same_v<typename Period::type, std::kilo>)			s << "ks";
	if constexpr (std::is_same_v<typename Period::type, std::mega>)			s << "Ms";
	if constexpr (std::is_same_v<typename Period::type, std::giga>)			s << "Gs";
	if constexpr (std::is_same_v<typename Period::type, std::tera>)			s << "Ts";
	if constexpr (std::is_same_v<typename Period::type, std::peta>)			s << "Ps";
	if constexpr (std::is_same_v<typename Period::type, std::exa>)			s << "Es";

	if constexpr (std::is_same_v<typename Period::type, std::ratio<60>>)	s << "min";
	if constexpr (std::is_same_v<typename Period::type, std::ratio<3600>>)	s << 'h';
	if constexpr (std::is_same_v<typename Period::type, std::ratio<86400>>)	s << 'd';

	return s;
}

template< class Clock, class Duration >
modm::IOStream&
operator << (modm::IOStream& s, const std::chrono::time_point<Clock, Duration>& tp)
{
	return s << tp.time_since_epoch();
}

inline modm::IOStream&
operator << (modm::IOStream& s, const std::tm& tm)
{
	s.printf("%04" PRIu16 "-%02" PRIu8 "-%02" PRIu8 " %02" PRIu8 ":%02" PRIu8 ":%02" PRIu8,
			 uint16_t(tm.tm_year + 1900u), uint8_t(tm.tm_mon), uint8_t(tm.tm_mday),
			 uint8_t(tm.tm_hour), uint8_t(tm.tm_min), uint8_t(tm.tm_sec));
	return s;
}

/// @}

}


