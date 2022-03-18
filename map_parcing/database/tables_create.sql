.open shadow.db

DROP TABLE IF EXISTS relation_members;
DROP TABLE IF EXISTS relation_tags;
DROP TABLE IF EXISTS way_nodes;
DROP TABLE IF EXISTS way_tags;
DROP TABLE IF EXISTS node_tags;
DROP TABLE IF EXISTS relations;
DROP TABLE IF EXISTS ways;
DROP TABLE IF EXISTS nodes;

CREATE TABLE nodes (
    node_id     INT8 UNSIGNED NOT NULL,
    lat         DOUBLE NOT NULL,
    lon         DOUBLE NOT NULL,
    PRIMARY KEY (node_id)
);

CREATE TABLE node_tags (
    node_id     INT8 UNSIGNED NOT NULL,
    tag_key     VARCHAR (255) NOT NULL,
    tag_val     VARCHAR (255) NOT NULL,
    PRIMARY KEY (node_id, tag_key),
    FOREIGN KEY (node_id) REFERENCES nodes(node_id)
);

CREATE TABLE ways (
    way_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (way_id)
);

CREATE TABLE way_tags (
    way_id      INT8 UNSIGNED NOT NULL,
    tag_key     VARCHAR (255) NOT NULL,
    tag_val   VARCHAR (255) NOT NULL,
    PRIMARY KEY (way_id, tag_key),
    FOREIGN KEY (way_id) REFERENCES ways(way_id)
);

CREATE TABLE way_nodes (
    way_id      INT8 UNSIGNED NOT NULL,
    node_id     INT8 UNSIGNED NOT NULL,
    sequence_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (way_id, sequence_id),
    FOREIGN KEY (way_id) REFERENCES ways(way_id),
    FOREIGN KEY (node_id) REFERENCES nodes(node_id)
);

CREATE TABLE relations (
    rel_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (rel_id)
);

CREATE TABLE rel_tags (
    rel_id INT8 UNSIGNED NOT NULL,
    tag_key     VARCHAR (255) NOT NULL,
    tag_val   VARCHAR (255) NOT NULL,
    PRIMARY KEY (rel_id, tag_key),
    FOREIGN KEY (rel_id) REFERENCES relations(rel_id)
);

CREATE TABLE rel_members (
    rel_id INT8 UNSIGNED NOT NULL,
    member_type VARCHAR (255) NOT NULL,
    member_id INT8 UNSIGNED NOT NULL,
    member_role VARCHAR (255) NOT NULL,
    sequence_id INT4 UNSIGNED NOT NULL,
    PRIMARY KEY (rel_id, member_type, member_id, member_role, sequence_id),
    FOREIGN KEY (rel_id) REFERENCES relations(rel_id),
    FOREIGN KEY (member_id) REFERENCES nodes(node_id),
    FOREIGN KEY (member_id) REFERENCES ways(way_id),
    FOREIGN KEY (member_id) REFERENCES relations(rel_id)
);
