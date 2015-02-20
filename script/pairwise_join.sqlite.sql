select count(*)
from Edges e1, Edges e2
where e1.b = e2.a;
