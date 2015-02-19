drop table if exists Edges;

create table Edges(
a integer,
b integer
);


.separator "\t"
.import data/arxiv-gr.txt Edges
