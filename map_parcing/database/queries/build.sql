SELECT WayBuildings.way_id, nodes.node_id, nodes.lat, nodes.lon, WayBuildings.tag_key, WayBuildings.tag_val
FROM WayBuildings
JOIN way_nodes ON way_nodes.way_id = WayBuildings.way_id
JOIN nodes ON nodes.node_id = way_nodes.node_id
LIMIT 30;
