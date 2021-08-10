#ifndef GPX_HELPER_H
#define GPX_HELPER_H

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>
#include "LinkedListAPI.h"

Waypoint *parseWayPoint(xmlNode *cur_node);
void createWayPointsList(xmlNode *rootNode, List *wpts);
Route *parseRoute(xmlNode *cur_node);
void createRoutesList(xmlNode *rootNode, List *rtes);
void getTrkSegWpts(xmlNode *node, List *list);
Track *parseTrack(xmlNode *cur_node); 
void createTrackskList(xmlNode *rootNode, List *tracks);
/**
returns 1 if it's a valid xml file
returns 0 if invalid
*/
int validateXMLTree(xmlDoc *doc, char* gpxSchemaFile);
bool GpxToXml(GPXdoc* doc, xmlNodePtr root);
void attachWpts(List *wpts, xmlNodePtr root, int type);
void attachRoutes(List *routes, xmlNodePtr root);
void attachTracks(List *tracks, xmlNodePtr root);
void attachOtherData(List *data, xmlNodePtr node);
void attachTrkSegments(List *segments, xmlNodePtr node);
bool validateDocContents(GPXdoc *doc);
bool validateWaypoints (List *wpts);
bool valideOtherData(List *data); 
bool validateRoutes(List *routes); 
bool validateTracks(List *tracks);
bool validateSegments(List *segments);
float haversine(double lat1, double lon1, double lat2, double lon2);
float getWptLength(List *waypoints);
void dummyDelete();


#endif