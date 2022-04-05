-- Receive all ways in the given rectangle

SELECT ways.way_id
FROM ways
JOIN way_nodes ON ways.way_id = way_nodes.way_id
JOIN nodes ON way_nodes.node_id = nodes.node_id
WHERE (nodes.lat BETWEEN 55.7263 AND 55.7816) AND (nodes.lon BETWEEN 37.6503 AND 37.7864)
GROUP BY ways.way_id;
