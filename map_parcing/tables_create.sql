.open shadow.db

DROP TABLE IF EXISTS member_node;
DROP TABLE IF EXISTS member_way;
DROP TABLE IF EXISTS member_rel;
DROP TABLE IF EXISTS RelTags;
DROP TABLE IF EXISTS rel_values;
DROP TABLE IF EXISTS rel_keys;
DROP TABLE IF EXISTS WayTags;
DROP TABLE IF EXISTS way_values;
DROP TABLE IF EXISTS way_keys;
DROP TABLE IF EXISTS NodeTags;
DROP TABLE IF EXISTS node_values;
DROP TABLE IF EXISTS node_keys;
DROP TABLE IF EXISTS NodeWay;
DROP TABLE IF EXISTS relations;
DROP TABLE IF EXISTS ways;
DROP TABLE IF EXISTS nodes;

CREATE TABLE nodes (
    node_id INT8 UNSIGNED NOT NULL,
    lat DOUBLE NOT NULL,
    lon DOUBLE NOT NULL,
    PRIMARY KEY (node_id)
);

CREATE TABLE ways (
    way_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (way_id)
);

CREATE TABLE relations (
    rel_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (rel_id)
);

CREATE TABLE NodeWay (
    node_id INT8 UNSIGNED NOT NULL,
    way_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (node_id, way_id),
    FOREIGN KEY (node_id) REFERENCES nodes(node_id),
    FOREIGN KEY (way_id) REFERENCES ways(way_id)
);

CREATE TABLE node_keys (
    key_id INT8 UNSIGNED NOT NULL,
    tag_key VARCHAR (255) NOT NULL,
    PRIMARY KEY (key_id) 
);

CREATE TABLE node_values (
    value_id INT8 UNSIGNED NOT NULL,
    tag_val VARCHAR (255) NOT NULL,
    PRIMARY KEY (value_id)
);

CREATE TABLE NodeTags (
    node_id INT8 UNSIGNED NOT NULL,
    key_id INT8 UNSIGNED NOT NULL,
    value_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (node_id, key_id, value_id),
    FOREIGN KEY (node_id) REFERENCES nodes(node_id),
    FOREIGN KEY (key_id) REFERENCES node_keys(key_id),
    FOREIGN KEY (value_id) REFERENCES node_values(value_id)
);

CREATE TABLE way_keys (
    key_id INT8 UNSIGNED NOT NULL,
    tag_key VARCHAR (255) NOT NULL,
    PRIMARY KEY (key_id) 
);

CREATE TABLE way_values (
    value_id INT8 UNSIGNED NOT NULL,
    tag_val VARCHAR (255) NOT NULL,
    PRIMARY KEY (value_id)
);

CREATE TABLE WayTags (
    way_id INT8 UNSIGNED NOT NULL,
    key_id INT8 UNSIGNED NOT NULL,
    value_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (way_id, key_id, value_id),
    FOREIGN KEY (way_id) REFERENCES ways(way_id),
    FOREIGN KEY (key_id) REFERENCES way_keys(key_id),
    FOREIGN KEY (value_id) REFERENCES way_values(value_id)
);

CREATE TABLE rel_keys (
    key_id INT8 UNSIGNED NOT NULL,
    tag_key VARCHAR (255) NOT NULL,
    PRIMARY KEY (key_id) 
);

CREATE TABLE rel_values (
    value_id INT8 UNSIGNED NOT NULL,
    tag_val VARCHAR (255) NOT NULL,
    PRIMARY KEY (value_id)
);

CREATE TABLE RelTags (
    rel_id INT8 UNSIGNED NOT NULL,
    key_id INT8 UNSIGNED NOT NULL,
    value_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (rel_id, key_id, value_id),
    FOREIGN KEY (rel_id) REFERENCES relations(rel_id),
    FOREIGN KEY (key_id) REFERENCES rel_keys(key_id),
    FOREIGN KEY (value_id) REFERENCES rel_values(value_id)
);

CREATE TABLE member_node (
    [id]               INT8 UNSIGNED NOT NULL,
    [rel_id]           INT8 UNSIGNED NOT NULL,
    [type]             VARCHAR (20)  NOT NULL,
    [ref_to_node]      INT8 UNSIGNED NOT NULL,
    [role]             VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (rel_id)      REFERENCES relations(id),
    FOREIGN KEY (ref_to_node) REFERENCES nodes(id)
);

CREATE TABLE member_way (
    [id]               INT8 UNSIGNED NOT NULL,
    [rel_id]           INT8 UNSIGNED NOT NULL,
    [type]             VARCHAR (20)  NOT NULL,
    [ref_to_way]       INT8 UNSIGNED NOT NULL,
    [role]             VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (rel_id)     REFERENCES relations(id),
    FOREIGN KEY (ref_to_way) REFERENCES ways(id)
);

CREATE TABLE member_rel (
    [id]               INT8 UNSIGNED NOT NULL,
    [rel_id]           INT8 UNSIGNED NOT NULL,
    [type]             VARCHAR (20)  NOT NULL,
    [ref_to_rel]      INT8 UNSIGNED  NOT NULL,
    [role]             VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (rel_id)     REFERENCES relations(id),
    FOREIGN KEY (ref_to_rel) REFERENCES relations(id)
);
