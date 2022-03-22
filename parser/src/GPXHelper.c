#include "LinkedListAPI.h"
#include "GPXParser.h"
#include "GPXParser.h"
#define MAX 1024

/**
*link: 
*function name
*
*/




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



void dummyDelete() {
    
}
