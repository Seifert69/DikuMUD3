#include "dil.h"
#include "formatter.h"
#include "json_helper.h"
#include "vmc.h"

#include <rapidjson/prettywriter.h>

#include <fstream>
#include <string>

extern zone_info g_zone;
void write_diltemplate_json(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer, diltemplate *tmpl);

void dump_json_zone(char *prefix)
{
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

    writer.StartObject();
    {
        writer.String("zone");
        writer.StartObject();
        {
            writer.String("name");
            writer.String(g_zone.z_zone.name);

            writer.String("title");
            if (g_zone.z_zone.title)
            {
                writer.String(g_zone.z_zone.title);
            }
            else
            {
                writer.Null();
            }

            writer.String("lifespan");
            writer.Uint(g_zone.z_zone.lifespan);

            writer.String("reset_mode");
            writer.Uint(g_zone.z_zone.reset_mode);

            writer.String("creators");
            if (g_zone.z_zone.creators)
            {
                writer.StartArray();
                for (auto creator = g_zone.z_zone.creators; *creator; ++creator)
                {
                    writer.String(*creator);
                }
                writer.EndArray();
            }
            else
            {
                writer.Null();
            }

            writer.String("notes");
            if (g_zone.z_zone.notes)
            {
                writer.String(g_zone.z_zone.notes);
            }
            else
            {
                writer.Null();
            }

            writer.String("help");
            if (g_zone.z_zone.help)
            {
                writer.String(g_zone.z_zone.help);
            }
            else
            {
                writer.Null();
            }

            writer.String("weather");
            writer.Int(g_zone.z_zone.weather);
        }
        writer.EndObject();

        writer.String("rooms");
        writer.StartArray();
        for (auto unit = g_zone.z_rooms; unit; unit = unit->getNext())
        {
            unit->toJSON(writer);
        }
        writer.EndArray();

        writer.String("mobiles");
        writer.StartArray();
        for (auto unit = g_zone.z_mobiles; unit; unit = unit->getNext())
        {
            unit->toJSON(writer);
        }
        writer.EndArray();

        writer.String("objects");
        writer.StartArray();
        for (auto unit = g_zone.z_objects; unit; unit = unit->getNext())
        {
            unit->toJSON(writer);
        }
        writer.EndArray();

        writer.String("table");
        writer.StartArray();
        for (auto reset = g_zone.z_table; reset; reset = reset->next)
        {
            writer.StartObject();
            {
                writer.String("cmd_no");
                writer.Int((int)reset->cmd_no);

                writer.String("ref1");
                if (reset->ref1)
                {
                    writer.String(reset->ref1);
                }
                else
                {
                    writer.Null();
                }

                writer.String("ref2");
                if (reset->ref2)
                {
                    writer.String(reset->ref2);
                }
                else
                {
                    writer.Null();
                }

                writer.String("num1");
                writer.Int(reset->num1);

                writer.String("num2");
                writer.Int(reset->num2);

                writer.String("num3");
                writer.Int(reset->num3);

                std::string c;
                c = reset->direction;
                writer.String("direction");
                writer.String(c.c_str());

                c = reset->cmpl;
                writer.String("cmpl");
                writer.String(c.c_str());
            }
            writer.EndObject();
        }
        writer.EndArray();

        writer.String("tmpl");
        writer.StartArray();
        for (auto tmpl = g_zone.z_tmpl; tmpl; tmpl = tmpl->vmcnext)
        {
            write_diltemplate_json(writer, tmpl);
        }
        writer.EndArray();
    }
    writer.EndObject();

    std::string filename(prefix);
    filename += ".json";

    std::ofstream out(filename);

    out << buffer.GetString();
}

void write_diltemplate_json(rapidjson::PrettyWriter<rapidjson::StringBuffer> &writer, diltemplate *tmpl)
{
    writer.StartObject();
    {
        writer.String("name");
        if (tmpl->zone)
        {
            auto str = diku::format_to_str("%s@%s", tmpl->prgname, tmpl->zone->getName());
            writer.String(str.c_str());
        }
        else
        {
            writer.String(tmpl->prgname);
        }

        writer.String("flags");
        writer.Int(tmpl->flags);

        writer.String("priority");
        writer.Int(tmpl->priority);

        writer.String("intrcount");
        writer.Int(tmpl->intrcount);

        writer.String("varcrc");
        writer.Int(tmpl->varcrc);

        writer.String("corecrc");
        writer.Int(tmpl->corecrc);

        writer.String("rtnt");
        writer.Int(tmpl->rtnt);

        writer.String("argc");
        writer.Int(tmpl->argc);

        writer.String("argument_types");
        writer.StartArray();
        for (auto i = 0; i < tmpl->argc; i++)
        {
            json::write_dil_var_type(tmpl->argt[i], writer);
        }
        writer.EndArray();

        writer.String("core_size");
        writer.Uint(tmpl->coresz);

        writer.String("core");
        writer.StartArray();
        for (auto i = 0u; i < tmpl->coresz; ++i)
        {
            writer.Uint(tmpl->core[i]);
        }
        writer.EndArray();

        writer.String("variable_count");
        writer.Uint(tmpl->varc);

        writer.String("variables");

        writer.StartArray();
        for (auto i = 0; i < tmpl->varc; ++i)
        {
            writer.StartObject();
            writer.String("variable_type");
            json::write_dil_var_type(tmpl->vart[i], writer);

            writer.String("variable_name");
            if (tmpl->varg[i])
            {
                writer.String(tmpl->varg[i]);
            }
            else
            {
                writer.Null();
            }
            writer.EndObject();
        }
        writer.EndArray();

        writer.String("xrefcount");
        writer.Uint(tmpl->xrefcount);

        writer.String("symbolic_references");
        writer.StartArray();
        for (auto i = 0; i < tmpl->xrefcount; ++i)
        {
            writer.StartObject();

            writer.String("name");
            writer.String(tmpl->xrefs[i].name);

            writer.String("return_type");
            json::write_dil_var_type(tmpl->xrefs[i].rtnt, writer);

            writer.String("no_of_args");
            writer.Uint(tmpl->xrefs[i].argc);

            writer.String("argument_types");
            writer.StartArray();
            for (auto j = 0; j < tmpl->xrefs[i].argc; ++j)
            {
                json::write_dil_var_type(tmpl->xrefs[i].argt[j], writer);
            }
            writer.EndArray();

            writer.EndObject();
        }
        writer.EndArray();
    }
    writer.EndObject();
}
