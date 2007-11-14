/*
 * YaST server side endpoint plugin stub for 'Custom'
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
#include <wsman-xml-serializer.h>

#include "yast.h"


static void
debug_hash( hash_t *hash, const char *header )
{
    if (header) debug( header );
    debug( " -- @ %p", hash );

    if (hash == NULL) return;

    hscan_t ptr;
    hash_scan_begin( &ptr, hash );

    hnode_t *node;
    while ((node = hash_scan_next( &ptr )) ) {
	debug ( "YaST:   hash [%s] = [%s]", hnode_getkey( node ), hnode_get( node ) );
    }
    return;
}


/*
 * Custom end point
 *
 */

int
YaST_Custom_EP( SoapOpH op,
		void* appData,
		void *opaqueData )
{
	debug( "YaST_Custom_EP Called");
	WsXmlDocH in_doc = NULL;		// the incoming body
	WsXmlDocH out_doc = NULL;		// the outgoing body

	WsmanStatus status;
	wsman_status_init( &status );

	// retrieve the incoming SOAP message, the body and the context
	SoapH soap = soap_get_op_soap( op );
	in_doc = soap_get_op_doc( op, 1 );

	if (in_doc) {
	    char *buf = NULL; int size;

	    WsXmlNodeH node = ws_xml_get_doc_root( in_doc );

	    ws_xml_dump_memory_node_tree( node, &buf, &size );
	}

	WsContextH cntx = ws_create_ep_context( soap, in_doc );
	if (!cntx) {
	    error( "Cannot create context" );
	    return 1;
	}

	// retrieve the custom action

	WsmanMessage *msg = wsman_get_msg_from_op( op );
	if (!msg) {
	    status.fault_code = WSMAN_INTERNAL_ERROR;
	    status.fault_detail_code = WSMAN_DETAIL_FORMAT_MISMATCH;
	    debug( "YaST: No message in SOAP ?!" );
	    goto cleanup;
	}

	debug( "YaST: method '%s'", msg->method );
	debug( "YaST: request '%s'", (char *)u_buf_ptr( msg->request ) );

	// retrieve action
	char *action = wsman_get_action( cntx, in_doc );
	debug( "YaST: action '%s'", action );

	// retrieve selectors
	hash_t *selectors = wsman_get_selector_list( cntx, NULL );
	debug_hash( selectors, "YaST: selectors" );

	// retrieve class name
	const char *classname = wsman_get_class_name( cntx );
	debug( "YaST: classname '%s'", classname );

	if ( strcmp( classname, "YCP" ) ) {					// class YCP ?
	    status.fault_code = WSA_DESTINATION_UNREACHABLE;
	    status.fault_detail_code = WSMAN_DETAIL_INVALID_RESOURCEURI;
	    debug( "YaST: unknown class '%s'", classname );
	    goto cleanup;
	}

	// retrieve resoure URI
	char *resource_uri = wsman_get_resource_uri( cntx, NULL );
	debug( "YaST: resource_uri %s", resource_uri );

	// retrieve custom method name and arguments
	const char *methodname = wsman_get_method_name( cntx );
	debug( "YaST: method name '%s'", methodname );

	hash_t *method_args = wsman_get_method_args( cntx, resource_uri );
	debug_hash( method_args, "YaST: method args" );

	if ( !method_args ) {					// args given ?
	    status.fault_code = WSMAN_INVALID_PARAMETER;
	    status.fault_detail_code = WSMAN_DETAIL_MISSING_VALUES;
	    debug( "YaST: no method arguments given" );
	    goto cleanup;
	}

	// check for 'eval'
	if ( strcmp( methodname, "eval" ) ) {
	    status.fault_code = WSA_ACTION_NOT_SUPPORTED;
	    status.fault_detail_code = OWSMAN_NO_DETAILS;
	    debug( "YaST: action '%s' not supported", methodname );
	    goto cleanup;
	}

	debug ("YaST: YCP/eval called" );
	// property for 'eval' must be 'ycp'
	hnode_t *ycpnode = hash_lookup( method_args, "ycp" );
	debug ("YaST: 'ycp' node @ %p", ycpnode );
	if (!ycpnode) {
	    status.fault_code = WSMAN_SCHEMA_VALIDATION_ERROR;
	    status.fault_detail_code = WSMAN_DETAIL_INVALID_VALUE;
	    debug( "YaST: no 'ycp' property found for 'eval'" );
	    goto cleanup;
	}

	// get value of "ycp"
	char *ycp = hnode_get( ycpnode );

	// create response
	out_doc = wsman_create_response_envelope( in_doc , NULL );

	WsXmlNodeH out_body = ws_xml_get_soap_body( out_doc );

	debug( "YaST: calling evaluate_ycp(%s)", ycp );
	int result = evaluate_ycp( ycp, out_body );
	debug( "YaST: result from evaluate_ycp(): '%d'", result );

	if (result != 0) {
	    status.fault_code = WSMAN_INVALID_PARAMETER;
	    status.fault_detail_code = WSMAN_DETAIL_INVALID_VALUE;
	    debug( "YaST: invalid ycp code passed" );
	    goto cleanup;
	}

cleanup:

	if (status.fault_code != WSMAN_RC_OK) {
	    if (out_doc) {
		ws_xml_destroy_doc( out_doc );
	    }
	    out_doc = wsman_generate_fault( in_doc, status.fault_code,
				status.fault_detail_code, NULL );
	}

	// return value handling

	if (out_doc) {
		soap_set_op_doc( op, out_doc, 0 );
	} else {
		error("Invalid result doc");
	}

	ws_destroy_context( cntx );

	return 0;
}

