/*
 * YaST server side endpoint plugin
 *
 * Copyright (c) 2007 Novell Inc. All rights reserved.
 * Written by Klaus Kaempf <kkaempf@novell.com>
 *
 * Based on 'cim_data' plugin. See copyright below.
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
 * @author Eugene Yarmosh
 * @author Klaus Kaempf
 */

#ifdef HAVE_CONFIG_H
#include "wsman_yast_config.h"
#endif

#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#ifdef HAVE_CTYPE_H
#include "ctype.h"
#endif

#include <wsman-xml-api.h>

#include "yast.h"

//
// ************ Serialization type information for resource ************
//
// It creates an array of items with name YaST_TypeInfo 
// It can be used in calls to WsSerialize and WsDeserialize 
//
SER_START_ITEMS( YaST )
SER_END_ITEMS( YaST );


// ************** Array of end points for resource ****************
//
// Must follow general convention xxx_EndPoints
//

START_END_POINTS( YaST )
  END_POINT_IDENTIFY( YaST, XML_NS_WSMAN_YAST ),			// Identify
//  END_POINT_TRANSFER_DIRECT_CREATE( YaST, XML_NS_WSMAN_YAST ),	// Create
//  END_POINT_TRANSFER_DIRECT_GET( YaST, XML_NS_WSMAN_YAST ),		// Get
//  END_POINT_TRANSFER_DIRECT_PUT( YaST, XML_NS_WSMAN_YAST ),		// Put
//  END_POINT_TRANSFER_DIRECT_DELETE( YaST, XML_NS_WSMAN_YAST ),	// Delete
//  END_POINT_ENUMERATE( YaST, XML_NS_WSMAN_YAST ),			// Enumerate
//  END_POINT_DIRECT_PULL( YaST, XML_NS_WSMAN_YAST ),			// Pull
//  END_POINT_RELEASE( YaST, XML_NS_WSMAN_YAST ),			// Release
  END_POINT_CUSTOM_METHOD( YaST, XML_NS_WSMAN_YAST ),			// Custom
FINISH_END_POINTS( YaST );


// ************** Array of name spaces for resource ****************
//

START_NAMESPACES( YaST )
    ADD_NAMESPACE( XML_NS_WSMAN_YAST, "YaST" ),
FINISH_NAMESPACES( YaST );


// register namespaces with server
//

static list_t *
set_namespaces(void) 
{

  int i;

  list_t *l = list_create(LISTCOUNT_T_MAX);
  for (i = 0; YaST_Namespaces[i].ns != NULL; i++)
  {
    lnode_t *node;
    WsSupportedNamespaces *ns = (WsSupportedNamespaces *)u_malloc(sizeof(WsSupportedNamespaces));
    ns->class_prefix = YaST_Namespaces[i].class_prefix;
    ns->ns = (char*) YaST_Namespaces[i].ns;
    debug( "YaST prefix:namespace [%s:%s]", ns->class_prefix, ns->ns );
    node = lnode_create(ns);
    list_append(l, node);
  }


  return l;
}


// register end points with server
//

void get_endpoints(void *self, void **data)
{
    WsDispatchInterfaceInfo *ifc = (WsDispatchInterfaceInfo *)data;	
    ifc->flags = 0;
    ifc->actionUriBase = NULL;
    ifc->version = WSMAN_VERSION;
    ifc->vendor = "Novell, Inc.";
    ifc->displayName = "YaST";
    ifc->notes = "Return value from call";
    ifc->compliance = XML_NS_WS_MAN;
    ifc->wsmanResourceUri = NULL;
    ifc->extraData = NULL;
    ifc->namespaces = set_namespaces();
    ifc->endPoints = YaST_EndPoints;			
}


// initialize plugin
//

int init( void *self, void **data )
{
    yast_initialize();
    return 1;
}

// cleanup plugin
//

void
cleanup( void  *self, void *data )
{
    yast_finish();
    return;
}


// ??
//

void set_config( void *self, dictionary *config )
{
    return;
}

