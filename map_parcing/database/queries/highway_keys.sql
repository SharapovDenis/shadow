SELECT *
FROM way_tags
WHERE tag_key = 'highway' GROUP BY tag_val;
