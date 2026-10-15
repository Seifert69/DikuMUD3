#include "color.h"
#include "comm.h"
#include "textutil.h"

#include <string>

#include <boost/test/unit_test.hpp>

/**********************************************************************************
 * Html_Attr_Suite - attributes on the comm divs, e.g.
 *     <div class='tell_other' from='Moira'>
 * The telnet colour path must still find the class, and names put into
 * attribute values must not be able to break out of them. The telnet path
 * also drops <data> packets.
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

// <data> packets are for the web client: telnet showed what was inside them,
// such as the whole JSON of a wstat or an $alias list
BOOST_AUTO_TEST_CASE(telnet_drops_data_packets)
{
    color_type colours;
    std::string dest;

    substHTMLcolor(dest, "<data type='$json'>{\"name\":\"bob\"}</data>Sending json edit info to your browser.<br/>", colours);
    BOOST_TEST(dest == "Sending json edit info to your browser.<br/>");

    substHTMLcolor(dest, "a<data type='$alias'><li>n north</li><li>s &amp; south<br/></li></data>b<data type='$combat'>{}</data>c", colours);
    BOOST_TEST(dest == "abc");

    substHTMLcolor(dest, "a<data type='$json'>{\"name\":", colours); // Never closed: the rest is data
    BOOST_TEST(dest == "a");

    substHTMLcolor(dest, "a<data type='$json'/>b<database>c</database>", colours);
    BOOST_TEST(dest == "ab<database>c</database>");
}

// A message that is only packets leaves nothing to send
BOOST_AUTO_TEST_CASE(telnet_message_of_only_data_is_empty)
{
    color_type colours;
    std::string dest = "x";

    substHTMLcolor(dest, "<data type='$alias'><li>n north</li></data>", colours);
    BOOST_TEST(dest == "");
    substHTMLcolor(dest, "<data type='$trigger'></data><data type='$var'></data>", colours);
    BOOST_TEST(dest == "");
}

// Tags are read as a browser reads them. Before, these left the rest of the
// text hidden, or showed the data.
BOOST_AUTO_TEST_CASE(telnet_data_tags_in_any_form)
{
    color_type colours;
    std::string dest;

    substHTMLcolor(dest, "a<DATA type='$json'>{}</DATA>b", colours);
    BOOST_TEST(dest == "ab");
    substHTMLcolor(dest, "a<data type='$json'>{}</data >b", colours);
    BOOST_TEST(dest == "ab");
    substHTMLcolor(dest, "a<data type='$json'>{}</Data\t>b", colours);
    BOOST_TEST(dest == "ab");
    substHTMLcolor(dest, "a<data\ttype='$json'>{}</data>b", colours);
    BOOST_TEST(dest == "ab");
    substHTMLcolor(dest, "a<data>{}</data>b", colours);
    BOOST_TEST(dest == "ab");
}

BOOST_AUTO_TEST_CASE(telnet_self_closed_data_in_any_form)
{
    color_type colours;
    std::string dest;

    substHTMLcolor(dest, "a<data type='$json' / >b", colours);
    BOOST_TEST(dest == "ab");
    substHTMLcolor(dest, "a<data/>b", colours);
    BOOST_TEST(dest == "ab");
    std::string html = "a<data type='" + std::string(300, 'x') + "'/>b"; // Longer than getHTMLTag() reads
    substHTMLcolor(dest, html.c_str(), colours);
    BOOST_TEST(dest == "ab");
}

// What isn't a <data> tag is left alone, and a tag without its '>' or
// </data> holds the rest of the text
BOOST_AUTO_TEST_CASE(telnet_data_edge_cases)
{
    color_type colours;
    std::string dest;

    substHTMLcolor(dest, "a<datum>b</datum><datalist>c</datalist>", colours);
    BOOST_TEST(dest == "a<datum>b</datum><datalist>c</datalist>");
    substHTMLcolor(dest, "a<data", colours);
    BOOST_TEST(dest == "a<data");
    substHTMLcolor(dest, "a<data type='$json'", colours);
    BOOST_TEST(dest == "a");
    substHTMLcolor(dest, "a<data>{}</datum>b", colours);
    BOOST_TEST(dest == "a");
    substHTMLcolor(dest, "a<data>1</data>b<data>2</data>c", colours);
    BOOST_TEST(dest == "abc");
}

// The server sends text over 32 KB in pieces, which mplex reads one by one;
// a packet is dropped whole before that
BOOST_AUTO_TEST_CASE(telnet_drops_data_packets_longer_than_a_piece)
{
    color_type colours;
    std::string dest;

    std::string html = "Before<data type='$json'>[" + std::string(40000, '1') + "]</data>After";
    substHTMLcolor(dest, html.c_str(), colours);
    BOOST_TEST(dest == "BeforeAfter");
}

// skipHTMLData() gives the text after the packet that p starts, or p
BOOST_AUTO_TEST_CASE(skip_data_gives_the_text_after_the_packet)
{
    BOOST_TEST(std::string(skipHTMLData("<data type='$json'>{}</data>after")) == "after");
    BOOST_TEST(std::string(skipHTMLData("<data/>after</data>")) == "after</data>");
    BOOST_TEST(std::string(skipHTMLData("<data>{}")) == "");
    BOOST_TEST(std::string(skipHTMLData("<data")) == "<data");
    BOOST_TEST(std::string(skipHTMLData("<div>x</div>")) == "<div>x</div>");
    BOOST_TEST(std::string(skipHTMLData("x<data>{}</data>")) == "x<data>{}</data>");
    BOOST_TEST(std::string(skipHTMLData("")) == "");
}

// For a snooping god: the packets go, the HTML stays for the god's client
BOOST_AUTO_TEST_CASE(remove_data_keeps_the_html)
{
    BOOST_TEST(str_remove_html_data("<div class='cpw'>a</div><data type='$alias'><li>n</li></data>b<br/>") ==
               "<div class='cpw'>a</div>b<br/>");
    BOOST_TEST(str_remove_html_data("<data type='$trigger'></data><data type='$var'></data>") == "");
    BOOST_TEST(str_remove_html_data("a<DATA>{}</data >b") == "ab");
    BOOST_TEST(str_remove_html_data("") == "");
}

// jstat's JSON: a packet for the web client, text for telnet
BOOST_AUTO_TEST_CASE(json_packet_for_each_client)
{
    BOOST_TEST(json_packet("{\"a\":\"<b>\"}", true) == "<data type='$json'>{\"a\":\"<b>\"}</data>");
    BOOST_TEST(json_packet("{\"a\":\"Dain's <b> & co\"}", false) ==
               "{&quot;a&quot;:&quot;Dain&#39;s &lt;b&gt; &amp; co&quot;}<br/>");

    // Not even text that looks like a packet is dropped from it
    color_type colours;
    std::string telnet = json_packet("{\"t\":\"<data>x</data>\"}", false);
    std::string dest;
    substHTMLcolor(dest, telnet.c_str(), colours);
    BOOST_TEST(dest == telnet);
}

BOOST_AUTO_TEST_SUITE_END()
