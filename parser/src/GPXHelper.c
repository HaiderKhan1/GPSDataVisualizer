#include "LinkedListAPI.h"
#include "GPXParser.h"
#include "GPXParser.h"
#define MAX 1024

/**
*link: 
*function name
*
*/


Waypoint *parseWayPoint(xmlNode *cur_node) {
    Waypoint *wpt = malloc(sizeof(Waypoint)); 
    xmlAttr *attr;
    for (attr = cur_node->properties; attr != NULL; attr = attr->next) {
        xmlNode *val = attr->children;
        char *attrName = (char *)attr->name;
        if(strcmp(attrName, "lat") == 0){
            double parsed_double = atof((char *) val->content);
            wpt->latitude = parsed_double;
        } else if (strcmp(attrName, "lon") == 0) {
            double parsed_double = atof((char *) val->content);
            wpt->longitude = parsed_double; 
        }
    }

    //other data
    wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *node = NULL;
    wpt->name = malloc(sizeof(char *) * MAX);
    if(wpt->name == NULL) {
        return NULL;
    }
    strcpy(wpt->name, "\0");
    for (node = cur_node->children; node!= NULL; node = node->next){
        if (node->type == XML_ELEMENT_NODE) {  
            char *eleName = (char *)node->name;
            xmlNode *childNode = node->children;
            char *content;
            int size;
            if(childNode != NULL) {
               content = (char *)childNode->content;
               size = strlen(content)+MAX;
            } else {
                strcpy(content,"\0");
                size = strlen(content)+MAX;
            }
            if(strcmp(eleName, "name") == 0) {
                strcpy(wpt->name, content);
            }   else {
                GPXData *data = malloc(sizeof(GPXData) + size * sizeof(char));
                if(data == NULL) {
                    return NULL;
                }
                strcpy(data->name, eleName);
                strcpy(data->value, content); 
                insertBack(wpt->otherData, (void *)data);
            }
        }
         
    }
   
    return wpt;
}

void createWayPointsList(xmlNode *rootNode, List *wpts){
    xmlNode *cur_node = NULL;
    for (cur_node = rootNode; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *name = (char *)cur_node->name; 
            if(strcmp(name, "wpt") == 0){
              Waypoint *wpt = parseWayPoint(cur_node);
              insertBack(wpts, (void *)wpt);
            }
        }
        createWayPointsList(cur_node->children, wpts);
    }
}

Route *parseRoute(xmlNode *cur_node) {
    Route *rte = malloc(sizeof(Route)); 
    rte->waypoints = initializeList(&waypointToString, &deleteWaypoint,&compareWaypoints);
    rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *node = NULL;
   
    rte->name = malloc(sizeof(char *) * MAX);
    if(rte->name == NULL) {
        return NULL;
    }
    strcpy(rte->name, "\0");
     
    for (node = cur_node->children; node!= NULL; node = node->next){
        if (node->type == XML_ELEMENT_NODE) {  
            char *eleName = (char *)node->name;
            xmlNode *childNode = node->children;
            char *content;
            int size;
            
            if(childNode != NULL) {
               content = (char *)childNode->content;
               size = strlen(content)+MAX;
            } else {
                strcpy(content,"\0");
                size = strlen(content)+MAX;
            }
            
            if(strcmp(eleName, "name") == 0) {
                strcpy(rte->name, content);
            } else if(strcmp(eleName, "rtept") == 0) {
                 Waypoint *wpt = parseWayPoint(node);
                 insertBack(rte->waypoints, (void *)wpt);
            } else {
                GPXData *data = malloc(sizeof(GPXData) + size * sizeof(char));
                if(data == NULL) {
                    return NULL;
                }
                strcpy(data->name, eleName);
                strcpy(data->value, content); 
                insertBack(rte->otherData, (void *)data);
            }
        }
         

    }

    return rte;
}

void createRoutesList(xmlNode *rootNode, List *rtes) {
    xmlNode *cur_node = NULL;
    for (cur_node = rootNode; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *name = (char *)cur_node->name; 
            if(strcmp(name, "rte") == 0){
                Route *rte = parseRoute(cur_node);
                insertBack(rtes, (void *)rte);
            }
        }
        createRoutesList(cur_node->children, rtes);
    }
}

void getTrkSegWpts(xmlNode *node, List *list) {
    xmlNode *cur_node = NULL;
    for (cur_node = node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *name = (char *)cur_node->name; 
            if(strcmp(name, "trkpt") == 0) {
              Waypoint *wpt = parseWayPoint(cur_node);
              insertBack(list, (void *)wpt);
            }
            
        }
        getTrkSegWpts(cur_node->children, list);
    }
}

Track *parseTrack(xmlNode *cur_node) {
    Track *trk = malloc(sizeof(Track));
    trk->segments = initializeList(&trackSegmentToString, &deleteTrackSegment, &compareTrackSegments);
    trk->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);
    xmlNode *node = NULL;
    trk->name = malloc(sizeof(char *) * MAX);
    if(trk->name == NULL) {
        return NULL;
    }
    strcpy(trk->name, "\0");
    for (node = cur_node->children; node!= NULL; node = node->next){
        if (node->type == XML_ELEMENT_NODE) {  
            char *eleName = (char *)node->name;
            xmlNode *childNode = node->children;
            char *content;
            int size;
            if(childNode != NULL) {
               content = (char *)childNode->content;
               size = strlen(content)+MAX;
            } else {
                strcpy(content,"\0");
                size = strlen(content)+MAX;
            }
            if(strcmp(eleName, "name") == 0) {
                strcpy(trk->name, content);
            } else if(strcmp(eleName, "trkseg") == 0) {
                 TrackSegment *trkseg = malloc(sizeof(TrackSegment));
                 if(trkseg == NULL) {
                     return NULL;
                 }
                 trkseg->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
                 getTrkSegWpts(node->children, trkseg->waypoints);
                 insertBack(trk->segments, (void *)trkseg);
            } else {
                GPXData *data = malloc(sizeof(GPXData) + size * sizeof(char));
                if(data == NULL) {
                    return NULL;
                }
                strcpy(data->name, eleName);
                strcpy(data->value, content); 
                insertBack(trk->otherData, (void *)data);
            }

        }
         
    }

    return trk;
}

void createTrackskList(xmlNode *rootNode, List *tracks){
    xmlNode *cur_node = NULL;
    for (cur_node = rootNode; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            char *name = (char *)cur_node->name; 
            if(strcmp(name, "trk") == 0){
                Track *trk = parseTrack(cur_node);
                insertBack(tracks, (void *)trk);
            }
        }
        createTrackskList(cur_node->children, tracks);
    }
}

int validateXMLTree(xmlDoc *doc, char* gpxSchemaFile) {
    int result = 0;
    if(doc == NULL || gpxSchemaFile == NULL) {
        return -1;
    }
    xmlSchemaPtr schema = NULL; 
    xmlSchemaParserCtxtPtr ctxt; 
    xmlLineNumbersDefault(1);
    ctxt = xmlSchemaNewParserCtxt(gpxSchemaFile);

    if(ctxt == NULL ) {
    xmlSchemaFree(schema);
    xmlSchemaCleanupTypes();
    xmlMemoryDump();
    }

    xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
    schema = xmlSchemaParse(ctxt);
    xmlSchemaFreeParserCtxt(ctxt);
    
    if (doc == NULL) {
        return 0;
    } else {

        xmlSchemaValidCtxtPtr ctxt;
		int ret;

		ctxt = xmlSchemaNewValidCtxt(schema);
		xmlSchemaSetValidErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
		ret = xmlSchemaValidateDoc(ctxt, doc);
        if (ret == 0) {
			result = 1;
		} else if (ret > 0) {
			result = 0;
		} else {
		    result = 0;
		}

		xmlSchemaFreeValidCtxt(ctxt);
    }

    // free the resource
    if(schema != NULL) {
        xmlSchemaFree(schema);
    }

    xmlSchemaCleanupTypes();
    xmlMemoryDump();

    return result;
     
}

void attachOtherData(List *data, xmlNodePtr node) {
    //the other elements
    ListIterator odIter = createIterator(data);
    void *otherData;
    while ((otherData = nextElement(&odIter)) != NULL) {
        GPXData *gpxData = (GPXData *)otherData;
        xmlNewChild(node, NULL, BAD_CAST gpxData->name, BAD_CAST gpxData->value);
    }
}

void attachWpts(List *wpts, xmlNodePtr root, int type) {
    /**
    *type 0 = wpt
    *type 1 = trkpt
    *type 2 = rtept
    */

    char *wptType = malloc(sizeof(char)*MAX); 
    if(type == 0) {
        strcpy(wptType, "wpt\0");
    } else if (type == 1) {
        strcpy(wptType, "trkpt\0");
    } else {
        strcpy(wptType, "rtept\0");
    }

    //loop through each element and extract the data
    ListIterator iter = createIterator(wpts);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
       Waypoint *wpt = (Waypoint *)data;

       //extract the tag
       xmlNodePtr node = xmlNewChild(root, NULL, BAD_CAST wptType, NULL);
       
       //extract lat 
       char *lat = malloc(sizeof(double)*MAX);
       sprintf(lat, "%lf", wpt->latitude);
       xmlNewProp(node, BAD_CAST "lat", BAD_CAST lat);
       free(lat);
       
       //extract longitude
        char *lon = malloc(sizeof(double)*MAX);
        sprintf(lon, "%lf", wpt->longitude);
        xmlNewProp(node, BAD_CAST "lon", BAD_CAST lon);
        free(lon);

        //extract name
        if(strcmp(wpt->name, "\0") != 0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST wpt->name);
        }

        //attach other Data 
        attachOtherData(wpt->otherData, node);
    }

    free(wptType);
}



void attachRoutes(List *routes, xmlNodePtr root) {
    ListIterator iter = createIterator(routes);
    void *data; 

    while((data = nextElement(&iter)) != NULL) {
        Route *rte = (Route *)data;
        //get the tag "rte"
        xmlNodePtr node = xmlNewChild(root, NULL, BAD_CAST "rte", NULL);

        //extract name
        if(strcmp(rte->name, "\0") != 0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST rte->name);
        }

        //extract other data
        attachOtherData(rte->otherData, node);

        //extract the rtept
        attachWpts(rte->waypoints, node, 2);

        
    }
}

void attachTrkSegments(List *segments, xmlNodePtr node) {
    ListIterator iter = createIterator(segments);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
        TrackSegment *trkSeg = (TrackSegment *)data;
        xmlNodePtr node1 = xmlNewChild(node, NULL, BAD_CAST "trkseg", NULL);
        attachWpts(trkSeg->waypoints, node1, 1);
    }
}

void attachTracks(List *tracks, xmlNodePtr root) {
    ListIterator iter = createIterator(tracks);
    void *data; 

    while((data = nextElement(&iter)) != NULL) {
        Track *trk = (Track *)data;
        //get the tag "rte"
        xmlNodePtr node = xmlNewChild(root, NULL, BAD_CAST "trk", NULL);

        //extract name
        if(strcmp(trk->name, "\0") != 0) {
            xmlNewChild(node, NULL, BAD_CAST "name", BAD_CAST trk->name);
        }

        //extract other data
        attachOtherData(trk->otherData, node);

        //extract the rtept
        attachTrkSegments(trk->segments, node);

     
    }
}


bool GpxToXml(GPXdoc* doc, xmlNodePtr root) {
    if(doc == NULL || root == NULL) {
        return false;
    }
   
    //name space
    xmlNsPtr nameSpace = NULL;
    nameSpace = xmlNewNs(root, BAD_CAST doc->namespace, NULL);
    
    if(nameSpace == NULL) {
        return false;
    }

    xmlSetNs(root, nameSpace);

    //version
    char *temp = malloc(sizeof(double) + MAX);
    sprintf(temp, "%1.1f", doc->version);
    if(temp == NULL) {
        return false;
    }
    xmlNewProp(root, BAD_CAST "version", BAD_CAST temp);
    free(temp);

    //creator
    xmlNewProp(root, BAD_CAST "creator", BAD_CAST doc->creator);
    
    attachWpts(doc->waypoints, root, 0);
    attachRoutes(doc->routes, root);
    attachTracks(doc->tracks, root);

    return true;
}

bool validateOtherData(List *data) {
    ListIterator odIter = createIterator(data);
    void *otherData;
    while ((otherData = nextElement(&odIter)) != NULL) {
        GPXData *gpxData = (GPXData *)otherData;
        if(gpxData != NULL) {
            if(strcmp(gpxData-> name, "\0") == 0 || strcmp(gpxData-> value, "\0") == 0) {
                return false;
            }
        }
    }

    return true;
}

bool validateWaypoints (List *wpts) {
    //loop through each element and extract the data
    ListIterator iter = createIterator(wpts);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
       Waypoint *wpt = (Waypoint *)data;
       if(wpt->name == NULL ) {
           return false; 
       }

       if(wpt->longitude < -180.0 || wpt->longitude > 80.0) {
           return false;
       }

       if(wpt->latitude < -90.0 || wpt->latitude > 90.0) {
           return false;
       }

        if(validateOtherData(wpt->otherData) == false) {
            return false;
        }
    }

    return true;
}

bool validateRoutes (List *routes) {
    //loop through each element and extract the data
    ListIterator iter = createIterator(routes);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
       Route *rte = (Route *)data;
       if(rte->name == NULL ) {
           return false; 
       }

        if(validateWaypoints(rte->waypoints) == false) {
            return false;
        }

        if(validateOtherData(rte->otherData) == false) {
            return false;
        }
    }

    return true;
}

bool validateSegments(List *segments) {
    //loop through each element and extract the data
    ListIterator iter = createIterator(segments);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
       TrackSegment *trkSeg = (TrackSegment *)data;
       if(validateWaypoints(trkSeg->waypoints) == false) {
           return false; 
        }
    }

    return true;
}

bool validateTracks (List *tracks) {
    //loop through each element and extract the data
    ListIterator iter = createIterator(tracks);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
       Track *trk = (Track *)data;
       if(trk->name == NULL ) {
           return false; 
       }

        if(validateSegments(trk->segments) == false) {
            return false;
        }

        if(validateOtherData(trk->otherData) == false) {
            return false;
        }
    }

    return true;
}

bool validateDocContents(GPXdoc *doc) {
    if(doc == NULL) {
        return false;
    }

    //validate GPXDoc information
    if(strcmp(doc->namespace, "\0") == 0  || doc->version < 0.0 || 
        doc->creator == NULL || strcmp(doc->creator, "\0") == 0) {
            return false;
    }

    //validate the Waypoints 
    if(validateWaypoints(doc->waypoints) == false) {
        return false;
    } 

    if(validateRoutes(doc->routes) == false) {
        return false;
    }

    if(validateTracks(doc->tracks) == false) {
        return false;
    }

    return true;
}

/**
This function is informed from GeeksForGeeks:
link: https://www.geeksforgeeks.org/haversine-formula-to-find-distance-between-two-points-on-a-sphere/
*/

float haversine(double lat1, double lon1, double lat2, double lon2) {
 
    double dLat = (lat2 - lat1) * M_PI / 180.0; 
    double dLon = (lon2 - lon1) * M_PI / 180.0; 
  
       // convert to radians 
    lat1 = (lat1) * M_PI / 180.0; 
    lat2 = (lat2) * M_PI / 180.0; 
  
        // apply formulae 
    double a = pow(sin(dLat / 2), 2) + pow(sin(dLon / 2), 2) * cos(lat1) * cos(lat2); 
    double rad = 6371000; 
    double c = 2 * asin(sqrt(a)); 
    return (float)rad * c; 
}

float getWptLength(List *waypoints) {
    void *firstNode = getFromFront(waypoints);
    Waypoint *firstWpt = (Waypoint *)firstNode;
    float total = 0.0;
    float lat1 = 0.0, lon1 = 0.0;
    lat1 = firstWpt->latitude;
    lon1 = firstWpt->longitude;
    int counter = 0;
    ListIterator iter = createIterator(waypoints);
    void *i;
    float lat2 = 0.0;
    float lon2 = 0.0;
    while((i = nextElement(&iter)) != NULL) {
        if(counter!=0){
            Waypoint *wpt = (Waypoint *)i;
            lat2 = wpt->latitude;
            lon2 = wpt->longitude;
            total+= haversine(lat1, lon1, lat2, lon2);
            lat1 = lat2;
            lon1 = lon2;
        }
        
        counter++;
    }
    return total;
}

void dummyDelete() {
    
}
