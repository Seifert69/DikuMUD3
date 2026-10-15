#include "color.h"
#include "comm.h"
#include "textutil.h"

#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Html_Attr_Suite - attributes on the comm divs, e.g.
 *     <div class='tell_other' from='Moira'>
 * The telnet colour path must still find the class, and names put into
 * attribute values must not be able to break out of them.
 */
BOOST_AUTO_TEST_SUITE(Html_Attr_Suite)

BOOST_AUTO_TEST_CASE(attr_escape)
{
    BOOST_TEST(str_html_attr_escape("Moira") == "Moira");
    BOOST_TEST(str_html_attr_escape("the mangy mongrel") == "the mangy mongrel");
    BOOST_TEST(str_html_attr_escape("Dain's <b>\"x\"</b> & co") == "Dain&#39;s &lt;b&gt;&quot;x&quot;&lt;/b&gt; &amp; co");
    BOOST_TEST(str_html_attr_escape("") == "");
    BOOST_TEST(str_html_attr_escape(nullptr) == "");
}

BOOST_AUTO_TEST_CASE(get_class_with_extra_attributes)
{
    char tag[256];
    char value[256];

    getHTMLTag("<div class='tell_other' from='Moira'>Moira tells you 'hi'</div>", tag, sizeof(tag));
    BOOST_TEST(std::string(tag) == "div class='tell_other' from='moira'");
    BOOST_TEST(getHTMLValue("class", tag, value, sizeof(value)) == 10);
    BOOST_TEST(std::string(value) == "tell_other");
    getHTMLValue("from", tag, value, sizeof(value));
    BOOST_TEST(std::string(value) == "moira");

    getHTMLTag("<div class='comm_say' chan='gossip' from='Moira'>", tag, sizeof(tag));
    getHTMLValue("class", tag, value, sizeof(value));
    BOOST_TEST(std::string(value) == "comm_say");
    getHTMLValue("chan", tag, value, sizeof(value));
    BOOST_TEST(std::string(value) == "gossip");
}

BOOST_AUTO_TEST_CASE(telnet_colour_with_extra_attributes)
{
    color_type colours;
    colours.create("tell_other:cpw bn:say_other:cw bn");

    std::string dest;
    substHTMLcolor(dest, "<div class='tell_other' from='Moira'>Moira tells you 'hi'</div>", colours);
    BOOST_TEST(dest == "<div class='cpw bn' from='moira'>Moira tells you 'hi'</div>");

    substHTMLcolor(dest, "<div class='say_other' from='Dain&#39;s ghost'>x</div>", colours);
    BOOST_TEST(dest == "<div class='cw bn' from='dain&#39;s ghost'>x</div>");

    // Unchanged without the extra attribute
    substHTMLcolor(dest, "<div class='tell_other'>hi</div>", colours);
    BOOST_TEST(dest == "<div class='cpw bn'>hi</div>");
}

BOOST_AUTO_TEST_SUITE_END()
