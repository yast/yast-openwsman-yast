/*
 * YaST server side endpoint plugin stub for 'Identify'
 *
 * Copyright (c) 2007 Novell Inc. All rights reserved.
 * Written by Klaus Kaempf <kkaempf@novell.com>
 *
 * Based on 'cim_data_stubs.c' plugin. See copyright below.
 *
 */

/*******************************************************************************
* Copyright (C) 2004-2006 Intel Corp. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*  - Redistributions of source code must retain the above copyright notice,
*    this list of conditions and the following disclaimer.
*
*  - Redistributions in binary form must reproduce the above copyright notice,
*    this list of conditions and the following disclaimer in the documentation
*    and/or other materials provided with the distribution.
*
*  - Neither the name of Intel Corp. nor the names of its
*    contributors may be used to endorse or promote products derived from this
*    software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS IS''
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED. IN NO EVENT SHALL Intel Corp. OR THE CONTRIBUTORS
* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
* POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**
 * @author Anas Nashif
 * @author Klaus Kaempf
 */
#ifdef HAVE_CONFIG_H
#include "wsman_yast_config.h"
#endif

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "ctype.h"

#include <wsman-soap.h>
#include <wsman-soap-envelope.h>
#include <wsman-xml-api.h>
#include <wsman-xml-serialize.h>

#include "yast.h"


/*
 * Identify end point
 *
 */

int
YaST_Identify_EP( WsContextH cntx )
{
    // The resource is defined by the WS-Man standard and modeled as a struct
    struct __wsmid_identify
    {
	XML_TYPE_STR ProtocolVersion;
	XML_TYPE_STR ProductVendor;
	XML_TYPE_STR ProductVersion;
    };
    typedef struct __wsmid_identify Wsmid_Identify;

    SER_START_ITEMS( Wsmid_Identify )
    SER_NS_STR( XML_NS_WSMAN_ID, "ProtocolVersion", 1 ), 
    SER_NS_STR( XML_NS_WSMAN_ID, "ProductVendor", 1 ),
    SER_NS_STR( XML_NS_WSMAN_ID, "ProductVersion", 1),
    SER_END_ITEMS(  Wsmid_Identify );

    Wsmid_Identify yast_identify = { XML_NS_WSMAN_YAST, "OpenWsman YaST plugin", PACKAGE_VERSION };

    debug( "YaST_Identify_EP Called");

    // build response envelope

    WsXmlDocH doc = wsman_create_response_envelope( cntx->indoc , NULL );
    WsXmlNodeH node = ws_xml_add_child( ws_xml_get_soap_body( doc ), XML_NS_WSMAN_ID, WSMID_IDENTIFY_RESPONSE, NULL );

    // build response body

//    <wsmid:IdentifyResponse>
//      <wsmid:ProtocolVersion>http://schema.opensuse.org/...</wsmid:ProtocolVersion>
//      <wsmid:ProductVendor>OpenWsman YaST plugin</wsmid:ProductVendor>
//      <wsmid:ProductVersion>1.1.0rc1</wsmid:ProductVersion>
//    </wsmid:IdentifyResponse>

    int result = ws_serialize( ws_serializer_init(),
			node,			// WsXmlNodeH xmlNode,
			&yast_identify,		// XML_TYPE_PTR dataPtr,
			Wsmid_Identify_TypeInfo,// XmlSerializerInfo * info,
			"YaST",			// const char *name,
			NULL,			// const char *ns
			NULL,			// XML_NODE_ATTR * attrs,
			0);			// int output)

    return result;
}
