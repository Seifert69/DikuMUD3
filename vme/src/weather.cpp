/*
 $Author: All $
 $RCSfile: weather.cpp,v $
 $Date: 2004/09/18 19:52:56 $
 $Revision: 2.5 $
 */

#include "weather.h"

#include "comm.h"
#include "db.h"
#include "interpreter.h"
#include "json_helper.h"
#include "main_functions.h"
#include "slog.h"
#include "utils.h"

#include <ctime>

int g_sunlight = SUN_SET;                     /* And how much sun. */
const time_t g_beginning_of_time = 650336715; /* Sat Aug 11 01:05:15 1990 */
time_t g_tBootTime = 0;                       // time(0) when server boots

/* What a coincidence!
 * I actually (REALLY!) converted the time_t on:  11-Aug-94 12:38:58.
 *								/gnort
 */

/* Calculate the REAL time passed over the last t2-t1 centuries (secs) */
time_info_data real_time_passed(time_t t2, time_t t1)
{
    long secs = 0;
    time_info_data now;

    secs = (long)difftime(t2, t1);

    now.setHours((secs / SECS_PER_REAL_HOUR) % 24);
    secs -= SECS_PER_REAL_HOUR * now.getHours();

    now.setDay((secs / SECS_PER_REAL_DAY) % 365); /* 0.. days  */
    secs -= SECS_PER_REAL_DAY * now.getDay();

    now.setMonth(-1);

    now.setYear((secs / SECS_PER_REAL_YEAR)); /* 0.. days  */

    return now;
}

/* Calculate the MUD date */
time_info_data mud_date(time_t t)
{
    time_info_data mdate;
    long p = 0;

    p = (long)difftime(t, g_beginning_of_time);

    mdate.setHours((p / SECS_PER_MUD_HOUR) % 24); /* 0..23 hours */
    p -= SECS_PER_MUD_HOUR * mdate.getHours();

    mdate.setDay((p / SECS_PER_MUD_DAY) % MUD_MONTH); /* 0..20 days  */
    p -= SECS_PER_MUD_DAY * mdate.getDay();

    mdate.setMonth((p / SECS_PER_MUD_MONTH) % MUD_YEAR); /* 0..8 months */
    p -= SECS_PER_MUD_MONTH * mdate.getMonth();

    mdate.setYear((p / SECS_PER_MUD_YEAR)); /* [0..[ years */

    return mdate;
}

time_info_data mud_date()
{
    // By doing this, essentially we allow timewarping to also warp the MUD time and date
    // MUD time can skew a tiny bit this way if ticks generally take longer than 250ms.
    // Before it simply used time(0).  I like this for debugging complicated zones, e.g. ships.

    time_t t = g_tBootTime + (g_tics / PULSE_SEC);

    return mud_date(t);
}

/* Calculate the MUD time passed over the last t2-t1 centuries (secs) */
time_info_data mud_time_passed(time_t t2, time_t t1)
{
    long secs = 0;
    time_info_data now;

    secs = (long)difftime(t2, t1);

    now.setHours((secs / SECS_PER_MUD_HOUR) % 24); /* 0..23 hours */
    secs -= SECS_PER_MUD_HOUR * now.getHours();

    now.setDay((secs / SECS_PER_MUD_DAY) % MUD_MONTH); /* 0..20 days  */
    secs -= SECS_PER_MUD_DAY * now.getDay();

    now.setMonth((secs / SECS_PER_MUD_MONTH) % MUD_YEAR); /* 0..8 months */
    secs -= SECS_PER_MUD_MONTH * now.getMonth();

    now.setYear((secs / SECS_PER_MUD_YEAR)); /* 0..XX? years */

    return now;
}

time_info_data age(unit_data *ch)
{
    static time_info_data player_age;

    if (ch->isPC())
    {
        player_age = mud_time_passed(time(nullptr), PC_TIME(ch).getPlayerBirthday());
    }
    else
    {
        player_age = mud_time_passed(0, 0);
    }

    return player_age;
}

/* Here comes the code */
static void another_hour(time_info_data time_data)
{
    switch (time_data.getHours())
    {
        case 5:
            g_sunlight = SUN_RISE;
            send_to_outdoor("The sun rises in the east.<br/>");
            break;

        case 6:
            g_sunlight = SUN_LIGHT;
            send_to_outdoor("The day has begun.<br/>");
            break;

        case 21:
            g_sunlight = SUN_SET;
            send_to_outdoor("The sun slowly disappears in the west.<br/>");
            break;

        case 22:
            g_sunlight = SUN_DARK;
            send_to_outdoor("The night has begun.<br/>");
            break;

        default:
            break;
    }
}

std::optional<std::string> Weather::updateWeather(const time_info_data &time_data)
{
    int diff = 0;

    if (time_data.getMonth() <= 2)
    {
        /* Winter */
        diff = (m_pressure <= (m_base - 25) ? 2 : -2);
    }
    else if (time_data.getMonth() <= 4)
    {
        /* Spring */
        diff = (m_pressure <= (m_base + 5) ? 2 : -2);
    }
    else if (time_data.getMonth() <= 6)
    {
        /* Summer */
        diff = (m_pressure <= (m_base + 20) ? 2 : -2);
    }
    else
    {
        /* Fall   */
        diff = (m_pressure <= (m_base - 5) ? 2 : -2);
    }

    setChange(dice(1, 4) * diff + dice(2, 6) - dice(2, 6));
    modifyPressureBy(m_change);

    int change = 0;
    switch (m_sky)
    {
        case SKY_CLOUDLESS:
        {
            if (m_pressure < 990)
            {
                change = 1;
            }
            else if (m_pressure < 1010)
            {
                if (dice(1, 4) == 1)
                {
                    change = 1;
                }
            }
            break;
        }
        case SKY_CLOUDY:
        {
            if (m_pressure < 970)
            {
                change = 2;
            }
            else if (m_pressure < 990)
            {
                if (dice(1, 4) == 1)
                {
                    change = 2;
                }
                else
                {
                    change = 0;
                }
            }
            else if (m_pressure > 1030)
            {
                if (dice(1, 4) == 1)
                {
                    change = 3;
                }
            }

            break;
        }
        case SKY_RAINING:
        {
            if (m_pressure < 970)
            {
                if (dice(1, 4) == 1)
                {
                    change = 4;
                }
                else
                {
                    change = 0;
                }
            }
            else if (m_pressure > 1030)
            {
                change = 5;
            }
            else if (m_pressure > 1010)
            {
                if (dice(1, 4) == 1)
                {
                    change = 5;
                }
            }

            break;
        }
        case SKY_LIGHTNING:
        {
            if (m_pressure > 1010)
            {
                change = 6;
            }
            else if (m_pressure > 990)
            {
                if (dice(1, 4) == 1)
                {
                    change = 6;
                }
            }
            break;
        }
        default:
        {
            change = 0;
            m_sky = SKY_CLOUDLESS;
            break;
        }
    }

    std::optional<std::string> zone_message;
    switch (change)
    {
        case 0:
        default:
            break;

        case 1:
            zone_message = "The sky is getting cloudy.<br/>";
            m_sky = SKY_CLOUDY;
            break;

        case 2:
            zone_message = "It starts to rain.<br/>";
            m_sky = SKY_RAINING;
            break;

        case 3:
            zone_message = "The clouds disappear.<br/>";
            m_sky = SKY_CLOUDLESS;
            break;

        case 4:
            zone_message = "Lightning starts to show in the sky.<br/>";
            m_sky = SKY_LIGHTNING;
            break;

        case 5:
            zone_message = "The rain stopped.<br/>";
            m_sky = SKY_CLOUDY;
            break;

        case 6:
            zone_message = "The lightning has stopped.<br/>";
            m_sky = SKY_RAINING;
            break;
    }
    return zone_message;
}

void update_time_and_weather()
{
    time_info_data time_info = mud_date();

    another_hour(time_info);

    for (auto &[zone_name, zone] : g_zone_info.mmp)
    {
        auto message = zone->getWeather().updateWeather(time_info);
        if (message)
        {
            send_to_zone_outdoor(zone, message->c_str());
        }
    }
}

/* Convert 'time' into text, and copy it into str */

void weather_and_time_event(void *p1, void *p2)
{
    update_time_and_weather();
    g_events.add(SECS_PER_MUD_HOUR * 4, weather_and_time_event, nullptr, nullptr);
}

/* reset the time in the game from file */
void boot_time_and_weather()
{
    g_tBootTime = time(nullptr);
    g_world_boottime = ctime(&g_tBootTime);
    g_world_boottime.erase(g_world_boottime.length() - 1);

    time_info_data time_info = mud_time_passed(time(nullptr), g_beginning_of_time);

    if (time_info.getHours() == 5)
    {
        g_sunlight = SUN_RISE;
    }
    else if (6 <= time_info.getHours() && time_info.getHours() <= 20)
    {
        g_sunlight = SUN_LIGHT;
    }
    else if (time_info.getHours() == 21)
    {
        g_sunlight = SUN_SET;
    }
    else
    {
        g_sunlight = SUN_DARK;
    }

    slog(LOG_OFF,
         0,
         "   Current Gametime: %dH %dD %dM %dY.",
         time_info.getHours(),
         time_info.getDay(),
         time_info.getMonth(),
         time_info.getYear());

    for (auto &[zone_name, zone] : g_zone_info.mmp)
    {
        zone->getWeather().boot(time_info);
    }

    g_events.add(PULSE_SEC * SECS_PER_MUD_HOUR, weather_and_time_event, nullptr, nullptr);
}

void Weather::boot(const time_info_data &time_info)
{
    m_pressure = m_base;

    int value = number(-10, 10);
    if (time_info.getMonth() >= 7 && time_info.getMonth() <= 12)
    {
        value = number(-6, 6);
    }

    modifyPressureBy(value);
    setChange(m_change + number(-2, 2));

    updateSky();
}

void Weather::updateSky()
{
    if (m_pressure <= 980)
    {
        m_sky = SKY_LIGHTNING;
    }
    else if (m_pressure <= 1000)
    {
        m_sky = SKY_RAINING;
    }
    else if (m_pressure <= 1020)
    {
        m_sky = SKY_CLOUDY;
    }
    else
    {
        m_sky = SKY_CLOUDLESS;
    }
}

int Weather::getPressure() const
{
    return m_pressure;
}

int Weather::getChange() const
{
    return m_change;
}

int Weather::getSky() const
{
    return m_sky;
}

int Weather::getBase() const
{
    return m_base;
}

void Weather::setBase(int value)
{
    m_base = value;
}

void Weather::setChange(int value)
{
    if (value > 12)
    {
        value = 12;
    }

    if (value < -12)
    {
        value = -12;
    }
    m_change = value;
}

void Weather::incrementChangeBy(int value)
{
    setChange(m_change + value);
}

void Weather::decrementChangeBy(int value)
{
    setChange(m_change - value);
}

void Weather::modifyPressureBy(int value)
{
    setPressure(m_pressure + value);
}

void Weather::setPressure(int value)
{
    if (value < 950)
    {
        value = 950;
    }

    if (value > 1050)
    {
        value = 1050;
    }

    m_pressure = value;
}

void Weather::toJSON(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer) const
{
    writer.StartObject();
    {
        json::write_kvp("pressure", m_pressure, writer);
        json::write_kvp("change", m_change, writer);
        json::write_kvp("sky", m_sky, writer);
        json::write_kvp("base", m_base, writer);
    }
    writer.EndObject();
}
