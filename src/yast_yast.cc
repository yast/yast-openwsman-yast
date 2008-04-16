/*
 * YaST runtime environment stub code
 *
 * Copyright (c) 2007 Novell Inc. All rights reserved.
 * Written by Klaus Kaempf <kkaempf@novell.com>
 *
 *
 */

#define y2log_component "openwsman"


#include <ycp/ExecutionEnvironment.h>
#include <y2/Y2Component.h>
#include <y2/Y2PluginComponent.h>
#include <y2/Y2ComponentBroker.h>

#include <YCP.h>
#include <ycp/Parser.h>
#include <ycp/pathsearch.h>

#include <scr/SCR.h>

#include "WFM.h"	// should be <wfm/WFM.h>

#include "yast.h"

#include <wsman-xml-api.h>

void
yast_initialize()
{
    static int initialized = 0;
    if (initialized) return;

    y2milestone( "yast_initialize()" );
    YCPPathSearch::initialize();


//    Y2Component *scr = Y2ComponentBroker::getNamespaceComponent( "SCR" );
    Y2Component *scr_server = Y2ComponentBroker::createServer( "scr" );

    if (scr_server) {
	SCRAgent *agent = scr_server->getSCRAgent();
	y2milestone( "scr agent at %p", agent );

	static SCR scr;
    }
    y2milestone( "scr component at %p", scr_server );

    Y2Component *wfm_server = Y2ComponentBroker::createServer( "wfm" );

    if (wfm_server) {
	SCRAgent *agent = wfm_server->getSCRAgent();
	y2milestone( "wfm agent at %p", agent );

	static WFM wfm;
    }
    y2milestone( "wfm component at %p", wfm_server );

}

void
yast_finish()
{
    y2milestone( "yast_finish()" );
    return;
}


//-------------------------------------------------------------------
static int xmlify( YCPValue result, WsXmlNodeH node );

// YCPList -> <list>...</list>
static int
list2xml ( YCPList l, WsXmlNodeH node )
{
    node = ws_xml_add_child( node, XML_NS_WSMAN_YAST, "list", NULL);

    int i = 0;
    while( i < l->size() ) {
	xmlify( l->value( i++ ), node );
    }
    return 0;
}


// YCPMap -> <map><key>...</key><value>...</value>...</map>

static int
map2xml ( YCPMap m, WsXmlNodeH node )
{
    node = ws_xml_add_child( node, XML_NS_WSMAN_YAST, "map", NULL);
    for (YCPMapIterator it = m->begin(); it != m->end(); it++) {
	xmlify( it.key( ), ws_xml_add_child( node, XML_NS_WSMAN_YAST, "key", NULL) );
	xmlify( it.value( ), ws_xml_add_child( node, XML_NS_WSMAN_YAST, "value", NULL) );
    }
    return 0;
}


// YCPTerm -> <term name="...">...</term>

static int
term2xml ( YCPTerm t, WsXmlNodeH node )
{
    node = ws_xml_add_child( node, XML_NS_WSMAN_YAST, "term", NULL);
    ws_xml_add_node_attr( node, XML_NS_WSMAN_YAST, "name", t->name().c_str() );

    YCPList l = t->args();
    int i = 0;
    while( i < l->size() ) {
	xmlify( l->value( i++ ), node );
    }
    return 0;
}


static int
xmlify( YCPValue result, WsXmlNodeH node )
{
    const char *name = NULL; const char *value;

    switch (result->valuetype()) {
	case YT_RETURN:
	case YT_BREAK:
	case YT_VOID:	name = "void"; value = NULL;
	break;
	case YT_BOOLEAN:name = "bool"; value = result->toString().c_str();
	break;
	case YT_INTEGER:name = "int"; value = result->toString().c_str();
	break;
	case YT_FLOAT:	name = "float"; value = result->toString().c_str();
	break;
	case YT_STRING:	name = "string"; value = result->asString()->value().c_str();
	break;
	case YT_PATH:	name = "path"; value = result->toString().c_str();
	break;
	case YT_SYMBOL:	name = "symbol"; value = result->asSymbol()->symbol().c_str();
	break;
	case YT_LIST:	return list2xml( result->asList(), node );
	break;
	case YT_TERM:	return term2xml( result->asTerm(), node );
	break;
	case YT_MAP:	return map2xml( result->asMap(), node );
	break;
	default:
	    y2error( "openwsman-yast: unsupported for XML: %s: %s", result->valuetype_str(), result->toString().c_str() );
	    return 1;
	break;
    }

    ws_xml_add_child( node, XML_NS_WSMAN_YAST, name, value );

    return 0;
}


// parse and evaluate YCP code
// return ycp result as string

int
evaluate_ycp( const char *ycp, WsXmlNodeH out_body )
{
    yast_initialize();

    y2milestone( "evaluate_ycp(%s)\n", ycp );

    Parser parser( ycp );	// set parser to option

    YCodePtr pc = parser.parse ();

    if (!pc ) {
	y2error( "Client option %s is not a valid YCP value", ycp );
	return 1;
    }

    y2milestone( "Parsed(%s)", pc->toString().c_str() );

    YCPValue result = pc->evaluate();
    if (result.isNull()) {
	y2error( "Evaluation failure" );
	return 1;
    }

    // add xml to response
    WsXmlNodeH node = ws_xml_add_child( out_body, XML_NS_WSMAN_YAST, "result", NULL );

    return xmlify( result, node );
}
