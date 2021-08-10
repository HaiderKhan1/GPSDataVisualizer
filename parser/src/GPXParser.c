#include "LinkedListAPI.h"
#include "GPXParser.h"
#include "GPXHelper.h"
#define MAX 20000
#define MAX_LEN 2048
/** Sources:
 * toString and delete functions were informed from the structDemoList.c example
 * Tree traversal was informed from xmlExample.c  
 */

void deleteGPXdoc(GPXdoc* doc) {

    if(doc == NULL) {
        return;
    }
   
    freeList(doc->tracks);
    freeList(doc->waypoints);
    freeList(doc->routes);
    free(doc->creator);
    free(doc);
}

void deleteGpxData( void* data) {
    GPXData *gpxdata;
   
    if(data == NULL) {
        return;
    }
    
    gpxdata = (GPXData *)data;
    free(gpxdata);
}

char* gpxDataToString( void* data) {
    char *tempStr;
    GPXData *gpxdata; 
    int len = MAX;

    if(data == NULL) {
        return NULL;
    } 

    gpxdata = (GPXData *)data;
    if(strcmp(gpxdata->value, "\0") != 0){
        len += strlen(gpxdata->value)+1;
    }
    
    tempStr = (char *)malloc(sizeof(char) * len);
    
    if(tempStr == NULL){
        return NULL;
    }
    sprintf(tempStr, "%s %s", gpxdata->name, gpxdata->value);
    return tempStr;
}

int compareGpxData(const void *first, const void *second){
    return 0;
}

void deleteWaypoint(void* data){
    if(data == NULL) {
        return;
    }

    Waypoint *wpt; 
    wpt = (Waypoint *)data;
    free(wpt->name);
    freeList(wpt->otherData);
    free(wpt);

}

char *waypointToString(void *data) {
  if (data == NULL) {
    return NULL;
  }

  Waypoint *wpt = (Waypoint *)data;
  char *tempStr;
  int len = MAX;

  if (strcmp(wpt->name, "\0") != 0) {
    len = strlen(wpt->name);
  }
  
  char *otherData = toString(wpt->otherData);
  len+=strlen(otherData);
  tempStr = (char *)malloc(sizeof(char) * len + MAX);
  
  if (tempStr == NULL){
    return NULL;
  }

  sprintf(tempStr, "wpt: name = %s lat = %f long = %f\n", wpt->name, wpt->latitude, wpt->longitude);
  strcat(tempStr, otherData);
  free(otherData);

  return tempStr;
}


int compareWaypoints(const void *first, const void *second) {
    return 0;
}


void deleteRoute(void* data){
        
    if(data == NULL) {
        return;
    }

    Route *rte; 
    rte = (Route *)data;
    free(rte->name);
    freeList(rte->waypoints);
    freeList(rte->otherData);
    free(rte);
}

char* routeToString(void* data) {
    if (data == NULL) {
        return NULL;
    }

  Route *rte = (Route *)data;
  char *tempStr;
  int len = MAX;

  if (strcmp(rte->name, "\0") != 0) {
    len += strlen(rte->name);
  }
 
  char *wptList = toString(rte->waypoints);
  char *otherData = toString(rte->otherData);
  len += strlen(wptList)+strlen(otherData);

  tempStr = (char *)malloc(sizeof(char) * len);
  if (tempStr == NULL){
    return NULL;
  }

  sprintf(tempStr, "rte: name = %s\n", rte->name);
  strcat(tempStr, wptList);
  strcat(tempStr, otherData);
  free(wptList);
  free(otherData);

  return tempStr;
}

int compareRoutes(const void *first, const void *second){
    return 0;
}

void deleteTrackSegment(void* data) {
    if(data == NULL) {
        return;
    }

    TrackSegment *trkseg; 
    trkseg = (TrackSegment *)data;
    freeList(trkseg->waypoints);
    free(trkseg);
}

char* trackSegmentToString(void* data){
    if(data == NULL) {
        return NULL;
    }

    TrackSegment *trkseg;
    trkseg = (TrackSegment *)data;
    char *tempStr;
    tempStr = toString(trkseg->waypoints);
    
    return tempStr;

}

int compareTrackSegments(const void *first, const void *second){
    return 0;
}

void deleteTrack(void* data) {
    if(data == NULL) {
        return; 

    }

    Track *trk; 
    trk = (Track *)data;
    free(trk->name);
    freeList(trk->otherData);
    freeList(trk->segments);
    free(trk);
}

char* trackToString(void* data){
    if(data == NULL) {
        return NULL;
    }

    Track *trk = (Track *)data;
    char *tempStr; 
    int len = MAX; 

    if(strcmp(trk->name, "\0") != 0) {
        len = strlen(trk->name) + MAX;
    }
    char *trksegList = toString(trk->segments);
    char *otherData = toString(trk->otherData);
    len+= strlen(trksegList)+strlen(otherData);

    tempStr = (char *) malloc(sizeof(char) * len);

    if(tempStr == NULL) {
        return NULL;
    }

    sprintf(tempStr, "Track: name = %s\n", trk->name);
    strcat(tempStr, trksegList);
    strcat(tempStr, otherData);
    
    free(trksegList);
    free(otherData);

    return tempStr;
}

int compareTracks(const void *first, const void *second){
    return 0;
}



GPXdoc* createGPXdoc(char *fileName) {
    if(fileName == NULL) {
        return NULL;
    }
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(fileName, NULL, 0);

    if(doc == NULL) {
       return NULL;
    }

    root_element = xmlDocGetRootElement(doc);
    GPXdoc *gpxdoc = malloc(sizeof(GPXdoc));

    if(gpxdoc == NULL) {
        return NULL;
    }
    
    // extract the name space 
    char *nsTemp = (char *)root_element->ns->href;
    if(nsTemp != NULL) {
        strcpy(gpxdoc->namespace, nsTemp);
    } else {
        strcpy(gpxdoc->namespace, "\0");
    }
    

    //extract the rest of values for GPX attr
    xmlAttr *attr; 

     for (attr = root_element->properties; attr != NULL; attr = attr->next) {
        char *name = (char *)attr->name;
        xmlNode *val = attr->children;

        //extract the version
        if(strcmp(name, "version\0") == 0) {
            double parsedVersion = atof((char *) val->content);
            gpxdoc->version = parsedVersion;
        } else if (strcmp(name, "creator\0") == 0) {
            char *tempCreator = (char *)(val->content);
            gpxdoc->creator = malloc(sizeof(char *)*strlen(tempCreator));
            strcpy(gpxdoc->creator, tempCreator);
        }
    }

    
    List *wpts = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    createWayPointsList(root_element, wpts);
    gpxdoc->waypoints = wpts;

   
    List *rte = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    createRoutesList(root_element, rte);
    gpxdoc->routes = rte;

    List *trks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    createTrackskList(root_element, trks);
    gpxdoc->tracks = trks; 
    
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return gpxdoc; 
}

char* GPXdocToString(GPXdoc* doc){
    
    if(doc == NULL) {
        return NULL;
    }
    int len = MAX; 
    char *tempStr;
    if (strcmp(doc->namespace, "\0") != 0) {
    len += strlen(doc->namespace);
  } else if(strcmp(doc->creator, "\0") != 0) {
    len+= strlen(doc->creator);
  }

  tempStr = (char *)malloc(sizeof(char) * len);

  sprintf(tempStr, "GPX Doc: \n %s %s %f", doc->namespace, doc->creator, doc->version);
  char *wpts = toString(doc->waypoints);
  strcat(tempStr, wpts);
  char *rtes = toString(doc->routes);
  strcat(tempStr, rtes);
  char *trks = toString(doc->tracks);
  strcat(tempStr, trks);
  
  free(wpts);
  free(rtes);
  free(trks);

  return tempStr;
}

//Total number of waypoints in the GPX file
int getNumWaypoints(const GPXdoc* doc){
     if(doc == NULL) {
        return 0;
    }
    return getLength(doc->waypoints);
}

//Total number of routes in the GPX file
int getNumRoutes(const GPXdoc* doc){
     if(doc == NULL) {
        return 0;
    }
    return getLength(doc->routes);
}

//Total number of tracks in the GPX file
int getNumTracks(const GPXdoc* doc){
    if(doc == NULL) {
        return 0;
    }

    return getLength(doc->tracks);
}

//Total number of segments in all tracks in the document
int getNumSegments(const GPXdoc* doc){
     if(doc == NULL) {
        return 0;
    }
    void *elem;
    int ret = 0;
    ListIterator iter = createIterator(doc->tracks);

    while((elem = nextElement(&iter)) != NULL) {
        Track *trk = (Track *)elem;
        ret += getLength(trk->segments);
    }

    return ret;
}   

//Total number of GPXData elements in the document
int getNumGPXData(const GPXdoc* doc) {
    if(doc == NULL) {
        return 0;
    }
    //get other other data waypts first
    void *elem;
    int ret = 0;
    ListIterator iterWpts = createIterator(doc->waypoints);

    while((elem = nextElement(&iterWpts)) != NULL) {
        Waypoint *wpt = (Waypoint *)elem;
        if(strcmp(wpt->name, "\0") != 0) {
            ret +=1; 
        }
        ret += getLength(wpt->otherData);
    }
    //get other data from routes, then the list of of -> rtept
    void *elemRoutes;
    ListIterator iterRoutes = createIterator(doc->routes);
    while((elemRoutes = nextElement(&iterRoutes)) != NULL) {
        Route *rte = (Route *)elemRoutes;
        ret += getLength(rte->otherData);
        if(strcmp(rte->name, "\0") != 0) {
            ret +=1; 
        }
        void *elemroutesWpts;
        ListIterator iterRoutesWpts = createIterator(rte->waypoints);
        while((elemroutesWpts = nextElement(&iterRoutesWpts)) != NULL) {
            Waypoint *wptRte = (Waypoint *)elemroutesWpts;
            ret += getLength(wptRte->otherData);
            if(strcmp(wptRte->name, "\0") != 0) {
                ret +=1; 
            }
        }
    }

    //get other data from tracks
    void *elemTracks;
    ListIterator iterTracks = createIterator(doc->tracks);
    //go through each Track struct, get it's other data
    while((elemTracks = nextElement(&iterTracks)) != NULL) {
        Track *trk = (Track *)elemTracks;
        ret += getLength(trk->otherData);
        if(strcmp(trk->name, "\0") != 0) {
            ret +=1; 
        }
        //no go through each track segment, each trkSeg has a list of wayPoints
        ListIterator trkSegIter = createIterator(trk->segments);
        void *trkSegData;
        while ((trkSegData = nextElement(&trkSegIter)) != NULL) {
            TrackSegment *trkSeg = (TrackSegment *)trkSegData;
            ListIterator trkSegWptItr = createIterator(trkSeg->waypoints);
            void *trkSegWpt;
            while((trkSegWpt = nextElement(&trkSegWptItr)) != NULL) {
                Waypoint *wptData = (Waypoint *)trkSegWpt;
                ret += getLength(wptData->otherData);  
                 if(strcmp(wptData->name, "\0") != 0) {
                     ret +=1; 
                 }
            } 
        }
    }
    
    return ret;
}

// Function that returns a waypoint with the given name.  If more than one exists, return the first one.  
// Return NULL if the waypoint does not exist
Waypoint* getWaypoint(const GPXdoc* doc, char* name) {
    if(doc == NULL) {
        return NULL; 
    }

    Waypoint *wpt; 
    ListIterator itr = createIterator(doc->waypoints);
    void *data;
    while((data = nextElement(&itr)) != NULL) {
        wpt = (Waypoint *)data;
        if(strcmp(wpt->name, name) == 0) {
            return wpt;
        }
    }

    return NULL;

}
// Function that returns a track with the given name.  If more than one exists, return the first one. 
// Return NULL if the track does not exist 
Track* getTrack(const GPXdoc* doc, char* name) {
    if(doc == NULL) {
        return NULL; 
    }
    
    Track *trk; 
    ListIterator itr = createIterator(doc->tracks);
    void *data;
    while((data = nextElement(&itr)) != NULL) {
        trk = (Track *)data;
        if(strcmp(trk->name, name) == 0) {
            return trk;
        }
    }
    return NULL;
}
// Function that returns a route with the given name.  If more than one exists, return the first one.  
// Return NULL if the route does not exist
Route* getRoute(const GPXdoc* doc, char* name) {
    if(doc == NULL) {
        return NULL; 
    }
    
    Route *rte; 
    ListIterator itr = createIterator(doc->routes);
    void *data;
    while((data = nextElement(&itr)) != NULL) {
        rte = (Route *)data;
        if(strcmp(rte->name, name) == 0) {
            return rte;
        }
    }
    return NULL;
}


/**Module 1 Assignment 2 **/

GPXdoc* createValidGPXdoc(char* fileName, char* gpxSchemaFile) {
    if(fileName == NULL || gpxSchemaFile == NULL) {
        return NULL;
    }

    char *ext = strrchr(fileName, '.');

    if(strcmp(ext, ".gpx") != 0) {
        return NULL;
    }

    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    doc = xmlReadFile(fileName, NULL, 0);
    
    if(doc == NULL) {
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }   
    

    if(validateXMLTree(doc, gpxSchemaFile) != 1){
        xmlFreeDoc(doc);
        xmlCleanupParser();
        return NULL;
    }

    root_element = xmlDocGetRootElement(doc);
    GPXdoc *gpxdoc = malloc(sizeof(GPXdoc));

    if(gpxdoc == NULL) {
        return NULL;
    }
    
    // extract the name space 
    char *nsTemp = (char *)root_element->ns->href;
    if(nsTemp != NULL) {
        strcpy(gpxdoc->namespace, nsTemp);
    } else {
        strcpy(gpxdoc->namespace, "\0");
    }
    

    //extract the rest of values for GPX attr
    xmlAttr *attr; 

     for (attr = root_element->properties; attr != NULL; attr = attr->next) {
        char *name = (char *)attr->name;
        xmlNode *val = attr->children;

        //extract the version
        if(strcmp(name, "version\0") == 0) {
            double parsedVersion = atof((char *) val->content);
            gpxdoc->version = parsedVersion;
        } else if (strcmp(name, "creator\0") == 0) {
            char *tempCreator = (char *)(val->content);
            gpxdoc->creator = malloc(sizeof(char *)*strlen(tempCreator));
            strcpy(gpxdoc->creator, tempCreator);
        }
    }

    
    List *wpts = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    createWayPointsList(root_element, wpts);
    gpxdoc->waypoints = wpts;

   
    List *rte = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    createRoutesList(root_element, rte);
    gpxdoc->routes = rte;

    List *trks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    createTrackskList(root_element, trks);
    gpxdoc->tracks = trks; 
    
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return gpxdoc; 
}


bool writeGPXdoc(GPXdoc* doc, char* fileName) {
    if(doc == NULL || fileName == NULL) {
        return false;
    }

    //create the doc and set it's root node
    xmlDocPtr xmlDoc = NULL;
    xmlNodePtr root_node = NULL;

    xmlDoc = xmlNewDoc(BAD_CAST "1.0");
    if(xmlDoc == NULL) {
        return false;
    }

    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(xmlDoc, root_node);
    bool parseState = GpxToXml(doc, root_node);

    if(parseState == false || xmlDoc == NULL || root_node == NULL) {
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return false;
    }

    if(xmlSaveFormatFileEnc(fileName, xmlDoc, "UTF-8", 1) == -1) {
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return false;
    }

    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();
    return true;
}


bool validateGPXDoc(GPXdoc* doc, char* gpxSchemaFile) {
    if(doc == NULL || gpxSchemaFile == NULL) {
        return false;
    }

    //create the doc and set it's root node
    xmlDocPtr xmlDoc = NULL;
    xmlNodePtr root_node = NULL;

    xmlDoc = xmlNewDoc(BAD_CAST "1.0");
    if(xmlDoc == NULL) {
        return false;
    }

    root_node = xmlNewNode(NULL, BAD_CAST "gpx");
    xmlDocSetRootElement(xmlDoc, root_node);
    bool parseState = GpxToXml(doc, root_node);

    if(parseState == false || xmlDoc == NULL || root_node == NULL) {
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return false;
    }

    if (validateXMLTree(xmlDoc, gpxSchemaFile) != 1){
        xmlFreeDoc(xmlDoc);
        xmlCleanupParser();
        return false;
    }

    //validate Doc Contents
    if(validateDocContents(doc) == false) {
        return false;
    }

    xmlFreeDoc(xmlDoc);
    xmlCleanupParser();
    return true;
}


float getRouteLen(const Route *rt) {
    if(rt == NULL || rt->waypoints == NULL || getLength(rt->waypoints) == 0) {
        return 0.0;
    }

    float total = getWptLength(rt->waypoints);
    //current element

    return total;
}

float getTrackLen(const Track *tr) {
    if(tr == NULL || tr->segments == NULL || getLength(tr->segments) == 0) {
        return 0.0;
    }
    
    ListIterator iter = createIterator(tr->segments);
    void *i;
    double lat1 = 0.0, lon1 = 0.0, lat2 = 0.0, lon2 = 0.0;
    int counter = 0;
    float total = 0.0;
    while((i = nextElement(&iter)) != NULL) {
        //get the trackSegment's length 
        TrackSegment *trkSeg = (TrackSegment *)i;
        if(getLength(trkSeg->waypoints) == 0) {
            continue;
        }
        total += getWptLength(trkSeg->waypoints);

        if(counter == 0) {
            void *w1 = getFromBack(trkSeg->waypoints);
            Waypoint *wpt1 = (Waypoint*)w1;
            lat1 = wpt1->latitude;
            lon1 = wpt1->longitude;
        }

        if (counter > 0) {
            void *w2 = getFromFront(trkSeg->waypoints);
            Waypoint *wpt2 = (Waypoint*)w2;
            lat2 = wpt2->latitude;
            lon2 = wpt2->longitude;
            total+=haversine(lat1, lon1, lat2, lon2);
            void *w1 = getFromBack(trkSeg->waypoints);
            Waypoint *wpt1 = (Waypoint *)w1;
            lat1 = wpt1->latitude;
            lon1 = wpt1->longitude;
        }

        counter++;
    }

    return total;
}


float round10(float len) {
   int ret = (int)len;
   ret = (ret+5)/10;
   ret = ret*10;
   return (float)ret;
}

int numRoutesWithLength(const GPXdoc* doc, float len, float delta) {
    if(doc == NULL || len < 0.0 || delta < 0.0) {
        return 0;
    }

    if(getLength(doc->routes)==0){
        return 0;
    }

    int counter = 0;
    ListIterator iter = createIterator(doc->routes);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
        Route *rte = (Route *)data;

        if(getLength(rte->waypoints) == 0) {
            continue;
        }

        float total = getRouteLen(rte);

        if(total == len) {
            counter++;
        } else if(fabs(len-total)<=delta) {
            counter++;
        }
    }

    return counter; 
}

int numTracksWithLength(const GPXdoc* doc, float len, float delta) {
    if(doc == NULL || len < 0.0 || delta < 0.0) {
        return 0;
    }

    if(getLength(doc->tracks) == 0) {
        return 0;
    }

    int counter = 0; 
    ListIterator iter = createIterator(doc->tracks);
    void *data1; 
    while((data1 = nextElement(&iter)) != NULL) {
        Track *trk = (Track *)data1;
        float total = getTrackLen(trk);
        if(total == len) {
            counter++;
        } else if(fabs(len-total)<=delta) {
            counter++;
        }
    }

    return counter;
}

bool isLoopRoute(const Route* route, float delta) {
    if(route == NULL || delta < 0.0) {
        return false;
    }

    if(getLength(route->waypoints) < 4) {
        return false;
    }

    double lat1 = 0.0, lon1 = 0.0,  lat2 = 0.0, lon2 = 0.0;
    void *wpt1 = getFromFront(route->waypoints);
    Waypoint *waypoint1 = (Waypoint *)wpt1;
    lat1 = waypoint1->latitude;
    lon1 = waypoint1->longitude;

    void *wpt2 = getFromBack(route->waypoints);
    Waypoint *waypoint2 = (Waypoint *)wpt2;
    lat2 = waypoint2->latitude;
    lon2 = waypoint2->longitude;

    float total = 0.0;
    total = haversine(lat1, lon1, lat2, lon2);

    if(total < delta) {
        return true;
    }

    return false;

}

bool isLoopTrack(const Track *tr, float delta) {
    if (tr == NULL || delta < 0.0) {
        return false;
    }

    //check weather atleast 4 wpts exist 
    ListIterator iter = createIterator(tr->segments);
    void *data;
    int counter = 0;
    while((data = nextElement(&iter)) != NULL)  {
        TrackSegment *trkSeg = (TrackSegment *)data;
        counter += getLength(trkSeg->waypoints);
    }

    if(counter < 4) {
        return false;
    }

    ListIterator segIter = createIterator(tr->segments);    
    TrackSegment *seg = NULL;
    TrackSegment *fSeg = NULL;
    TrackSegment *bSeg = NULL;
    bool set = false;
        
    while((seg = nextElement(&segIter)) != NULL) {
        if (getLength(seg->waypoints)!= 0) {
             if(set == false) {
                fSeg = seg;
                set = true;
            }
            bSeg = seg;
        }
    }

    if(fSeg == NULL || bSeg == NULL) {
        return false;
    }

    //check to see if the track forms a loop
    float lat1 = 0.0, lon1, lat2 = 0.0, lon2 = 0.0;
    
    //get wpt from front
    void *w1 = getFromFront(fSeg->waypoints);
    Waypoint *wpt1 = (Waypoint *)w1;
    lat1 = wpt1->latitude;
    lon1 = wpt1->longitude;

    //get wpt from back
    void *w2 = getFromBack(bSeg->waypoints);
    Waypoint *wpt2 = (Waypoint *)w2;
    lat2 = wpt2->latitude;
    lon2 = wpt2->longitude;

    float total = haversine(lat1, lon1, lat2, lon2);

    if(total < delta){
        return true;
    }


    return false;
}

List* getRoutesBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if(doc == NULL || delta < 0.0) {
        return NULL;
    }

    List *routeList = initializeList(&routeToString, &dummyDelete, &compareRoutes);

    int counter = 0;
    //go through each route
    ListIterator iter = createIterator(doc->routes);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
        float destDist = 0.0, sourceDist = 0.0;
        Route *rte = (Route *)data;
        void *w1 = getFromFront(rte->waypoints);

        Waypoint *wpt1 = (Waypoint *)w1;
        void *w2 = getFromBack(rte->waypoints);
        Waypoint *wpt2 = (Waypoint *)w2;

        //check destination
        sourceDist = haversine(sourceLat, sourceLong, (float)wpt1->latitude, (float)wpt1->longitude);
        destDist = haversine(destLat, destLong, (float)wpt2->latitude, (float)wpt2->longitude);

        if(destDist <= delta && sourceDist <= delta) {
            insertBack(routeList, (void *)rte);
            counter++;
        }
    }

    if(counter>0){
        return routeList;
    }
    
    freeList(routeList);
    return NULL;
}

List* getTracksBetween(const GPXdoc* doc, float sourceLat, float sourceLong, float destLat, float destLong, float delta) {
    if (doc == NULL || delta < 0.0) {
        return NULL;
    }

    List *trackList = initializeList(&trackToString, &dummyDelete, &compareTracks);
    int counter = 0;
    ListIterator iter = createIterator(doc->tracks);
    void *data;
    while((data = nextElement(&iter)) != NULL) {
        
        float destDist = 0.0, sourceDist = 0.0;
        Track *trk = (Track *)data;
        //if the track has no segments
        
        ListIterator segIter = createIterator(trk->segments);
        TrackSegment *seg = NULL;
        TrackSegment *fSeg = NULL;
        TrackSegment *bSeg = NULL;
        bool set = false;
        
        while((seg = nextElement(&segIter)) != NULL) {
            if (getLength(seg->waypoints)!= 0) {
                if(set == false) {
                    fSeg = seg;
                    set = true;
                }
                bSeg = seg;
            }
        }

        if(fSeg == NULL || bSeg == NULL) {
            continue;
        }

            //get first
            void *w1 = getFromFront(fSeg->waypoints);
            Waypoint *wpt1 = (Waypoint *)w1; 

            //get last
            void *w2 = getFromBack(bSeg->waypoints);
            Waypoint *wpt2 = (Waypoint *)w2;
      

        //calculate delta 
        sourceDist = haversine(sourceLat, sourceLong, (float)wpt1->latitude, (float)wpt1->longitude);
        destDist = haversine(destLat, destLong, (float)wpt2->latitude, (float)wpt2->longitude);

        if(destDist <= delta && sourceDist <= delta) {
            insertBack(trackList, (void *)trk);
            counter++;
        } 
    }

    if(counter>0){
        return trackList;
    }
    
    freeList(trackList);
    return NULL;

}

int numPoints(List *list) {
    int count = 0;
    ListIterator itr = createIterator(list);
    void *data;
    while((data = nextElement(&itr)) != NULL) {
        TrackSegment *trkseg = (TrackSegment *)data;
        count += getLength(trkseg->waypoints);
    }

    return count;
}

char* trackToJSON(const Track *tr) {
    //if null return empty string
    if(tr == NULL) {
        char *ret = malloc(sizeof(char)*256);
        strcpy(ret, "{}");
        return ret;
    }

    int len = MAX_LEN;
    //get the name
    char *tempName;
    if(strcmp(tr->name, "\0") == 0 || tr->name == NULL) {
        tempName = "None";
    } else {
        tempName = tr->name;
    }

    //get the  loopStat
    char *tempStat;
    bool isLoop = isLoopTrack(tr,10); 
    if(isLoop) {
       tempStat = "true";
    } else {
       tempStat = "false";
    }

    //get the trackLen
    float total = getTrackLen(tr);

    //track to JSON
    len+=strlen(tempName)+strlen(tempStat);
    char *str = (char *)malloc(sizeof(char) * len);
    sprintf(str, "{\"name\":\"%s\",\"len\":%0.1lf,\"numPoints\":%d,\"loop\":%s}", tempName, round10(total), numPoints(tr->segments),tempStat);

    return str;
}

char* routeToJSON(const Route *rt) {
        //if null return empty string
    if(rt == NULL) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"{}");
        return ret;
    }

    int len = MAX_LEN;
    //get the name
    char *tempName;
    if(strcmp(rt->name, "\0") == 0 || rt->name == NULL) {
        tempName = "None";
    } else {
        tempName = rt->name;
    }

    //get the loopStat
    char *tempStat;
    bool isLoop = isLoopRoute(rt,10); 
    if(isLoop) {
       tempStat = "true";
    } else {
       tempStat = "false";
    }

    //get the trackLen
    float total = getRouteLen(rt);

    //track to JSON
    len+=strlen(tempName)+strlen(tempStat);
    char *str = (char *)malloc(sizeof(char) * len);
    sprintf(str, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%0.1lf,\"loop\":%s}", tempName, getLength(rt->waypoints),round10(total), tempStat);

    return str;
}

char* routeListToJSON(const List *list) {
    //get the size of the new string to malloc
    if(list == NULL || getLength((List *)list) == 0) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"[]");
        return ret;
    }

    int len = MAX_LEN * getLength((List *)list);

    //create the return string
    char *str = (char *)malloc(sizeof(char)*len);
    strcpy(str, "[");
    ListIterator Jiter = createIterator((List*)list);
    void *elem;
    int max = getLength((List *)list);
    int c = 0;
    while ((elem = nextElement(&Jiter)) != NULL) {
        char *tempStr2 = routeToJSON((Route *)elem);
        strcat(str, tempStr2);
        if(c+1 != max) {
            strcat(str, ",");
        }
        free(tempStr2);
        c++;
    }

    strcat(str, "]");
    return str;
}

char* trackListToJSON(const List *list) {
        //get the size of the new string to malloc
    if(list == NULL || getLength((List *)list) == 0) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"[]");
        return ret;
    }

    int len = MAX_LEN * getLength((List *)list);
    //create the return string
    char *str = (char *)malloc(sizeof(char)*len);
    strcpy(str, "[");
    ListIterator Jiter = createIterator((List*)list);
    void *elem;
    int max = getLength((List *)list);
    int c = 0;
    while ((elem = nextElement(&Jiter)) != NULL) {
        char *tempStr2 = trackToJSON((Track *)elem);
        strcat(str, tempStr2);
        if(c+1 != max) {
            strcat(str, ",");
        }
        free(tempStr2);
        c++;
    }

    strcat(str, "]");
    return str;
}

char* GPXtoJSON(const GPXdoc* gpx) {
    if(gpx == NULL) {
        char *ret = malloc(sizeof(char)*256);
        strcpy(ret, "{}");
        return ret;
    }

    int n = strlen(gpx->namespace);
    int len = MAX_LEN+n;
    char *str = malloc(sizeof(char)*len);
    sprintf(str, "{\"version\":%0.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d}", gpx->version, gpx->creator, 
    getLength(gpx->waypoints), getLength(gpx->routes), getLength(gpx->tracks));

    return str;
}


void addWaypoint(Route *rt, Waypoint *pt) {
    if(rt == NULL || pt == NULL) {
        return;
    }

    insertBack(rt->waypoints, (void *)pt);
}

void addRoute(GPXdoc* doc, Route* rt) {
    if(doc == NULL || rt == NULL) {
        return;
    }

    insertBack(doc->routes, (void *)rt);
}

GPXdoc* JSONtoGPX(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }

    char *str = (char *)gpxString;
    //parse the input 
    const char s[MAX_LEN] = "{}:,\"";
    char *token = strtok(str, s);
    double version = 0.0;
    char *creator = NULL;
    int counter = 0; 

    while( token != NULL ) {
      if(counter == 1) {
          version = atof(token);
      } else if (counter == 3) {
        creator = token;
      }
      token = strtok(NULL, s);
      
      counter++;
   }

    if(version == 0.0 || creator == NULL ) {
        return NULL;
    } 

    GPXdoc *gpxdoc = malloc(sizeof(GPXdoc));
    strcpy(gpxdoc->namespace,"http://www.topografix.com/GPX/1/1");
    gpxdoc->version = version;
    gpxdoc->creator = malloc(sizeof(char)*strlen(creator)+1);

    if(gpxdoc->creator == NULL) {
        return NULL;
    }

    strcpy(gpxdoc->creator, creator);

    List *wpts = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    gpxdoc->waypoints = wpts;

    if(gpxdoc->waypoints == NULL) {
        return NULL;
    }
   
    List *rte = initializeList(&routeToString, &deleteRoute, &compareRoutes);
    gpxdoc->routes = rte;

    if(gpxdoc->routes == NULL) {
        return NULL;
    }

    List *trks = initializeList(&trackToString, &deleteTrack, &compareTracks);
    gpxdoc->tracks = trks; 

    if(gpxdoc->tracks == NULL) {
        return NULL;
    }

    return gpxdoc;
}

Waypoint* JSONtoWaypoint(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }

    char *str = (char *)gpxString;
    const char s[MAX_LEN] = "{}:,\" ";
    char *token = strtok(str, s);
    double lat = 0.0;
    double lon = 0.0;
    int counter = 0;

    while( token != NULL ) {
        if(counter == 1) {
            lat = atof(token);
        } else if (counter == 3) {
            lon = atof(token);
        }
            
        token = strtok(NULL, s);

      counter++;
   }

    if(lat == 0.0 || lon == 0.0) {
        return NULL;
    } 

    Waypoint *wpt = malloc(sizeof(Waypoint)); 

    if(wpt == NULL) {
        return NULL;
    }

    wpt->name = malloc(sizeof(char)*MAX_LEN);
    
    if(wpt->name == NULL){
        return NULL;
    }

    strcpy(wpt->name, "\0");
    wpt->latitude = lat;
    wpt->longitude = lon;
    wpt->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    if(wpt->otherData == NULL) {
        return NULL;
    }

    return wpt;
}

Route* JSONtoRoute(const char* gpxString) {
    if (gpxString == NULL) {
        return NULL;
    }

    char *str = (char *)gpxString;
    const char s[MAX_LEN] = "{}:,\"";
    char *token = strtok(str, s);
    char *name = NULL;
    int counter = 0;

    while(token != NULL ) {
        if(counter == 1) {
           name = token;
        } 
        token = strtok(NULL, s);
      
      counter++;
   }

    if (name == NULL) {
        return NULL;
    }

    Route *rte = malloc(sizeof(Route)); 

    if(rte == NULL) {
        return NULL;
    }

    rte->name = malloc(sizeof(char)*MAX_LEN);
    
    if(rte->name == NULL){
        return NULL;
    }

    strcpy(rte->name, name);
    rte->waypoints = initializeList(&waypointToString, &deleteWaypoint, &compareWaypoints);
    rte->otherData = initializeList(&gpxDataToString, &deleteGpxData, &compareGpxData);

    if(rte->otherData == NULL || rte->waypoints == NULL) {
        return NULL;
    }

    return rte;
}

/* ********* A3 Functions ************** */


char* otherDataToJSON(const GPXData *rt) {
        //if null return empty string
    if(rt == NULL) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"{}");
        return ret;
    }

    int len = strlen(rt->name)+strlen(rt->value)+MAX_LEN;
    char *str = (char *)malloc(sizeof(char) * len);
    sprintf(str, "{\"name\":\"%s\",\"value\":\"%s\"}", rt->name, rt->value);

    return str;
}

char *otherDataListToJSON(const List *list) {
    if(list == NULL || getLength((List *)list) == 0) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"[]}");
        return ret;
    }

    int len = MAX_LEN * getLength((List *)list);

    //create the return string
    char *str = (char *)malloc(sizeof(char)*len);
    strcpy(str, "[");
    ListIterator Jiter = createIterator((List*)list);
    void *elem;
    int max = getLength((List *)list);
    int c = 0;
    while ((elem = nextElement(&Jiter)) != NULL) {
        char *tempStr2 = otherDataToJSON((GPXData *)elem);
        strcat(str, tempStr2);
        if(c+1 != max) {
            strcat(str, ",");
        }
        free(tempStr2);
        c++;
    }

    strcat(str, "]");
    strcat(str, "}");
    return str;

}

char *pointToJSON(const Waypoint *rt) {
        //if null return empty string
    if(rt == NULL) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"{}");
        return ret;
    }

    int len = strlen(rt->name)+MAX_LEN;
    char *str = (char *)malloc(sizeof(char) * len);
    sprintf(str, "{\"name\":\"%s\",\"lat\":\"%lf\",\"lon\":\"%lf\"}", rt->name, rt->latitude, rt->longitude);

    return str;
}

char *pointListToJSON(const List *list) {
    if(list == NULL || getLength((List *)list) == 0) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"[]");
        return ret;
    }

    int len = MAX_LEN * getLength((List *)list);

    //create the return string
    char *str = (char *)malloc(sizeof(char)*len);
    strcpy(str, "[");
    ListIterator Jiter = createIterator((List*)list);
    void *elem;
    int max = getLength((List *)list);
    int c = 0;
    while ((elem = nextElement(&Jiter)) != NULL) {
        char *tempStr2 = pointToJSON((Waypoint *)elem);
        strcat(str, tempStr2);
        if(c+1 != max) {
            strcat(str, ",");
        }
        free(tempStr2);
        c++;
    }

    strcat(str, "]");
    //strcat(str, "}");
    return str;

}

char* completeRouteToJSON(const Route *rt) {
        //if null return empty string
    if(rt == NULL) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"{}");
        return ret;
    }

    int len = MAX_LEN;
    //get the name
    char *tempName;
    if(strcmp(rt->name, "\0") == 0 || rt->name == NULL) {
        tempName = "None";
    } else {
        tempName = rt->name;
    }

    //get the loopStat
    char *tempStat;
    bool isLoop = isLoopRoute(rt,10); 
    if(isLoop) {
       tempStat = "true";
    } else {
       tempStat = "false";
    }

    //get the trackLen
    float total = getRouteLen(rt);
    char *otherData = otherDataListToJSON(rt->otherData);
    char *wptData = pointListToJSON(rt->waypoints);
    //track to JSON
    len+=strlen(tempName)+strlen(tempStat)+strlen(otherData)+strlen(wptData);
    char *str = (char *)malloc(sizeof(char) * len);
    sprintf(str, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%0.1lf,\"loop\":%s", tempName, getLength(rt->waypoints),round10(total), tempStat);
    strcat(str, ",\"waypoints\":");
    strcat(str, wptData);
    strcat(str, ",\"otherData\":");
    strcat(str, otherData);
    free(otherData);
    free(wptData);
    return str;
}

char* completeRouteListToJSON(const List *list){
    //get the size of the new string to malloc
    if(list == NULL || getLength((List *)list) == 0) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"[]");
        return ret;
    }

    int len = MAX_LEN * getLength((List *)list);

    //create the return string
    char *str = (char *)malloc(sizeof(char)*len);
    strcpy(str, "[");
    ListIterator Jiter = createIterator((List*)list);
    void *elem;
    int max = getLength((List *)list);
    int c = 0;
    while ((elem = nextElement(&Jiter)) != NULL) {
        char *tempStr2 = completeRouteToJSON((Route *)elem);
        strcat(str, tempStr2);
        if(c+1 != max) {
            strcat(str, ",");
        }
        free(tempStr2);
        c++;
    }

    strcat(str, "]");
    return str;
}

char* completeTrackToJSON(const Track *tr) {
    //if null return empty string
    if(tr == NULL) {
        char *ret = malloc(sizeof(char)*256);
        strcpy(ret, "{}");
        return ret;
    }

    int len = MAX_LEN;
    //get the name
    char *tempName;
    if(strcmp(tr->name, "\0") == 0 || tr->name == NULL) {
        tempName = "None";
    } else {
        tempName = tr->name;
    }

    //get the  loopStat
    char *tempStat;
    bool isLoop = isLoopTrack(tr,10); 
    if(isLoop) {
       tempStat = "true";
    } else {
       tempStat = "false";
    }

    //get the trackLen
    float total = getTrackLen(tr);

    //track to JSON
    len+=strlen(tempName)+strlen(tempStat);
    char *str = (char *)malloc(sizeof(char) * len);
    sprintf(str, "{\"name\":\"%s\",\"numPoints\":%d,\"len\":%0.1lf,\"loop\":%s", tempName, numPoints(tr->segments),round10(total), tempStat);
    char *otherData = otherDataListToJSON(tr->otherData);
    strcat(str, ",\"otherData\":");
    strcat(str, otherData);
    free(otherData);
    return str;
}

char* completeTrackListToJSON(const List *list) {
        //get the size of the new string to malloc
    if(list == NULL || getLength((List *)list) == 0) {
        char *ret = malloc(sizeof(char *)*256);
        strcpy(ret,"[]");
        return ret;
    }

    int len = MAX_LEN * getLength((List *)list);
    //create the return string
    char *str = (char *)malloc(sizeof(char)*len);
    strcpy(str, "[");
    ListIterator Jiter = createIterator((List*)list);
    void *elem;
    int max = getLength((List *)list);
    int c = 0;
    while ((elem = nextElement(&Jiter)) != NULL) {
        char *tempStr2 = completeTrackToJSON((Track *)elem);
        strcat(str, tempStr2);
        if(c+1 != max) {
            strcat(str, ",");
        }
        free(tempStr2);
        c++;
    }

    strcat(str, "]");
    return str;
}

char* GPXFILEtoJSON(char *fileName, char *xsd) {
     if (fileName == NULL || xsd == NULL) {
         return NULL;
     }
     
     GPXdoc *gpx = createValidGPXdoc(fileName, xsd);
     if(gpx == NULL) {
        char *ret = malloc(sizeof(char)*256);
        strcpy(ret, "{}");
        return ret;
    }

    char *routes =  completeRouteListToJSON(gpx->routes);
    char *tracks = completeTrackListToJSON(gpx->tracks);

    int n = strlen(gpx->namespace)+strlen(routes)+strlen(tracks);
    int len = 2048+n;
    char *str = malloc(sizeof(char)*len);
    sprintf(str, "{\"version\":%0.1f,\"creator\":\"%s\",\"numWaypoints\":%d,\"numRoutes\":%d,\"numTracks\":%d", gpx->version, gpx->creator, 
    getLength(gpx->waypoints), getLength(gpx->routes), getLength(gpx->tracks));
    strcat(str, ",\"routes\":");
    strcat(str, routes);
    strcat(str, ",\"tracks\":");
    strcat(str, tracks);
    strcat(str, "}");
    free(routes);
    free(tracks);

    return str;
}

int covertJSONtoGPX(char *json, char *fileName, char *xsd) {
    GPXdoc *doc = JSONtoGPX(json);
    bool validation = validateGPXDoc(doc, xsd);
    bool writeValidation;
    if (validation == true) {
        writeValidation = writeGPXdoc(doc, fileName);
    }

    if (writeValidation){
        return 1;
    } else {
        return 0;
    }
 
}

char *getRoutesinJSON(char *fileName, char *xsd, float sLat, float sLon, float dLat, float dLon, float delta) {
    GPXdoc *doc = createValidGPXdoc(fileName, xsd);
    if (doc == NULL) {
        return "0";
    }
    char *routes = routeListToJSON(getRoutesBetween(doc, sLat, sLon, dLat, dLon, delta));
    deleteGPXdoc(doc);
    return routes;
}

char *getTracksinJSON(char *fileName, char *xsd, float sLat, float sLon, float dLat, float dLon, float delta) {
    GPXdoc *doc = createValidGPXdoc(fileName, xsd);
    if (doc == NULL) {
        return "0";
    }
    char *tracks = trackListToJSON(getTracksBetween(doc, sLat, sLon, dLat, dLon, delta));
    deleteGPXdoc(doc);
    return tracks;
}

int addRte(char *file, char *xsd, char *str) {
    if (file == NULL || xsd == NULL || str == NULL) {
        return 0;
    }

    GPXdoc *doc = createValidGPXdoc(file, xsd);

    if(doc == NULL) {
        return 0;
    }

    Route *route = JSONtoRoute(str);
    if (route == NULL) {
        return 0;
    }

    addRoute(doc, route);
    bool validate = writeGPXdoc(doc, file);
    deleteGPXdoc(doc);

    if (validate == false) {
        return 0;
    }

    return 1;
}

int addWpt(char *file, char *xsd, char *str) {
    if(file == NULL || xsd == NULL || str == NULL) {
        return 0;
    }

    GPXdoc *doc = createValidGPXdoc(file, xsd);
    if (doc == NULL) {
        return 0;
    }

    Route *rte = getFromBack(doc->routes);
    if(rte == NULL) {
        return 0;
    }

    Waypoint *wpt = JSONtoWaypoint(str);
    if(wpt == NULL) {
        return 0;
    }

    addWaypoint(rte, wpt);
    bool validate = writeGPXdoc(doc, file);
    deleteGPXdoc(doc);

    if (validate == false) {
        return 0;
    }

    return 1;
}

int validateFile(char *file, char *xsd) {
    GPXdoc *doc = createValidGPXdoc(file, xsd);
    if (doc == NULL) {
        return 0;
    }

    deleteGPXdoc(doc);
    return 1;
}