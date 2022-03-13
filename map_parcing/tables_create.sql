DROP TABLE IF EXISTS node_tags;
DROP TABLE IF EXISTS way_tags;
DROP TABLE IF EXISTS rel_tags;
DROP TABLE IF EXISTS way_ref;
DROP TABLE IF EXISTS members;
DROP TABLE IF EXISTS relations;
DROP TABLE IF EXISTS ways;
DROP TABLE IF EXISTS nodes;

CREATE TABLE nodes (
    id  INT8 UNSIGNED NOT NULL,
    lat DOUBLE        NOT NULL,
    lon DOUBLE        NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE ways (
    id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE relations (
    id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (id)
);

CREATE TABLE way_ref (
    id      INT8 UNSIGNED NOT NULL,
    way_id  INT8 UNSIGNED NOT NULL,
    node_id INT8 UNSIGNED NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (way_id) REFERENCES ways(id),
    FOREIGN KEY (node_id) REFERENCES nodes(id)
);

CREATE TABLE node_tags (
    id      INT8 UNSIGNED NOT NULL,
    node_id INT8 UNSIGNED NOT NULL,
    tag_key VARCHAR (255) NOT NULL,
    tag_val VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (node_id) REFERENCES nodes(id)
    );

CREATE TABLE way_tags (
    id      INT8 UNSIGNED NOT NULL,
    way_id  INT8 UNSIGNED NOT NULL,
    tag_key VARCHAR (255) NOT NULL,
    tag_val VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (way_id) REFERENCES ways(id)
    );

CREATE TABLE rel_tags (
    id      INT8 UNSIGNED NOT NULL,
    rel_id  INT8 UNSIGNED NOT NULL,
    tag_key VARCHAR (255) NOT NULL,
    tag_val VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (rel_id) REFERENCES relations(id)
    );

CREATE TABLE members (
    id      INT8 UNSIGNED NOT NULL,
    rel_id  INT8 UNSIGNED NOT NULL,
    type    VARCHAR (255) NOT NULL,
    ref_id  INT8 UNSIGNED NOT NULL,
    role    VARCHAR (255) NOT NULL,
    PRIMARY KEY (id),
    FOREIGN KEY (rel_id) REFERENCES relations(id)
)
