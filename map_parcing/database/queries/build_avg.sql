-- EXPLAIN QUERY PLAN
SELECT WayBuildings.way_id, AVG(nodes.lat) as avg_lat, AVG(nodes.lon) as avg_lon, WayBuildings.tag_key, WayBuildings.tag_val
FROM WayBuildings
JOIN way_nodes ON way_nodes.way_id = WayBuildings.way_id
JOIN nodes ON nodes.node_id = way_nodes.node_id
WHERE tag_key = 'building:levels' AND (lat BETWEEN 55.74286387918377 AND 55.743322866803005) AND (lon BETWEEN 37.612852787454855 AND 37.63375398201489)
GROUP BY WayBuildings.way_id
LIMIT 30;
