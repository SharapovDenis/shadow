DROP VIEW IF EXISTS FootPath;
DROP VIEW IF EXISTS WayBuildings;
DROP VIEW IF EXISTS RelBuildings;

CREATE VIEW FootPath AS
SELECT ways.way_id, way_tags.tag_key, way_tags.tag_val 
FROM ways
JOIN way_tags ON ways.way_id = way_tags.way_id
WHERE (way_tags.tag_key = 'living_street' OR way_tags.tag_key = 'highway') AND (way_tags.tag_val = 'yes' OR way_tags.tag_val = 'footway');

CREATE VIEW WayBuildings AS
SELECT ways.way_id, way_tags.tag_key, way_tags.tag_val 
FROM ways
JOIN way_tags ON ways.way_id = way_tags.way_id
WHERE way_tags.tag_key = 'building' OR way_tags.tag_key = 'building:levels';

CREATE VIEW RelBuildings AS
SELECT relations.rel_id, rel_tags.tag_key, rel_tags.tag_val 
FROM relations
JOIN rel_tags ON relations.rel_id = rel_tags.rel_id
WHERE rel_tags.tag_key = 'building' OR rel_tags.tag_key = 'building:levels';
