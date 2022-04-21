WITH a AS ( 
SELECT way_id, COUNT(way_id) as c
FROM WayBuildings
GROUP BY way_id 
)

SELECT * FROM a
JOIN WayBuildings ON WayBuildings.way_id = a.way_id 
WHERE c = 1 AND tag_key = 'building';
